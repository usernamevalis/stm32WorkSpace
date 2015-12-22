#include "stm32l1xx_conf.h"
#include <stdio.h>

int main(void);
void delay(int a);
void gpioInit(void);
void clockInit(void);
void clockInitMCO(void);
void pwmInit(void);
void belgianWarehouse(int x,int pause, int speed);

//noise resources
void delayms(uint16_t millis);
void delayuss(unsigned int micros);
inline unsigned long Random(void);

//can i change the clock programatically whenever i want?

void clockInitMCO(){

    /* start hsi clock */
  RCC_HSICmd(ENABLE);
  RCC_PLLCmd(DISABLE);
  // wait for HSI to get ready
  while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);
   
  /* configure pll to give out 96 MHz */
  RCC_PLLConfig(RCC_PLLSource_HSI, RCC_PLLMul_12, RCC_PLLDiv_2);
  /* start pll */
  RCC_PLLCmd(ENABLE);
  /* stabilize pll */
  while ( RCC_GetFlagStatus( RCC_FLAG_PLLRDY ) == RESET );

  //set hsi as clock
  //RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);

  // set pll as system clock - this exceeds the IC capacbilities but worh experimenting
  //RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

}

void clockInit()
{
  // divide HCLK / 2
  RCC_HCLKConfig(RCC_SYSCLK_Div2);

  // enable HSI
  RCC_HSICmd(ENABLE);
  RCC_PLLCmd(DISABLE);
  // wait for HSI to get ready
  while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);

  // configure PLL - x4 /2 
  RCC_PLLConfig( RCC_PLLSource_HSI, RCC_PLLMul_8,  RCC_PLLDiv_4 );
  RCC_PLLCmd(ENABLE);

  while ( RCC_GetFlagStatus( RCC_FLAG_PLLRDY ) == RESET );

  // set hsi as clock
  //RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);

  // set pll as clock
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
}

void gpioInit(){
   //--------------------------GPIO init ----------------------------//
  //--------output---------//
  GPIO_InitTypeDef gpio;
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

  GPIO_StructInit(&gpio);
  gpio.GPIO_Pin = GPIO_Pin_2; // bitbang output
  gpio.GPIO_Mode = GPIO_Mode_OUT;
  gpio.GPIO_Speed = GPIO_Speed_40MHz;
  gpio.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &gpio);

  //PA3 is associated with TIM2_CH4
  gpio.GPIO_Pin =  GPIO_Pin_3; //pwm output
  gpio.GPIO_Mode = GPIO_Mode_AF;                //set pin mode to use an alternate function (AF)
  gpio.GPIO_OType = GPIO_OType_PP;              //who knows, think its 'push-pull mode'
  gpio.GPIO_PuPd  = GPIO_PuPd_UP;
  gpio.GPIO_Speed = GPIO_Speed_40MHz;

  GPIO_Init(GPIOA, &gpio);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_TIM2);     //configure alternate function to us TIM2

   //--------input---------//

  gpio.GPIO_Pin = GPIO_Pin_0; 
  gpio.GPIO_Mode = GPIO_Mode_IN;
  gpio.GPIO_Speed = GPIO_Speed_40MHz;
  gpio.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(GPIOA, &gpio);

  //--------------------------GPIO init ----------------------------//
}

void pwmInit(){

  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  uint16_t PrescalerValue = 0;
      /* TIM3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);



/*----------------------------------TIM2 Configuration -------------------------------------
  The TIM2CLK frequency is set to SystemCoreClock (Hz)
  SystemCoreClock is = 32MHz (32000000)
  The TIM2 is running at 32000000/1000000 

  Configure Timer (I sure hope this math is correct):

  PWM frequency = 1000 hz from a 32 000 000 hz system clock , using 1000 steps of the timer clock. 
  32000000/32 = 1000000
  1000000/1000 = 1000

  This allows us to define a pulse width output from 0-100% with a precision of 1%
----------------------------------------------------------------------- */

  /* Compute the prescaler value */
  PrescalerValue = (uint16_t) (SystemCoreClock / 1000000) - 1;        //0...31
  TIM_TimeBaseStructure.TIM_Period = 1000-1;                          //0...999
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;         //Timer counts up from 0

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  /* PWM1 Mode configuration: Channel2 
     PA1 is associated with TIM2_CH2
     The Channel number is in the function call: 
     TIM_OC2Init = channel 2 , 
     TIM_OC3Init = channel 3 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                   //pin output mode configured as PWM (repetitive)
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OC4Init(TIM2, &TIM_OCInitStructure);

    /* TIM2 enable counter */
  TIM_Cmd(TIM2, ENABLE);

}


int main(void)
{

  gpioInit();
  //pwmInit();

  // Last thing to do is to find a way to control the RF emission by enabling/disabling MCO. The best way to do it is to use bit-band mapped to PA8 MODER bit number 1 (not 0). 
  // By altering it’s value user can switch between Anlternate Function (which is MCO) and General Output which will just set pin low. Here’s the mapping, done as a global variable:
  // GPIOA->MODER ^= (1<<17); // use this to toggle the MCO pin - it is uses the MODE register, look it up in the manual. setting 17 to 1 makes PA8 =MCO and to 0 mackes PA_8 = input pin turned low (default reset state)
  //-------------------------- noise variables -------------------------//

  //variableto hold button increemnt and change settings
  unsigned long buttonCounter = 0;

  unsigned int onestate = 1;
  unsigned int onelengthstate = 0;
  unsigned int oneplaystate = 0;
  
  unsigned int oneperiod = 1000;
  unsigned int oneperiodcount = 0;

  unsigned int onelengthperiod = 50000; 
  unsigned int onelengthperiodcount = 0;

  unsigned int oneplayperiod = 60000;
  unsigned int oneplayperiodcount = 0;

  unsigned int oneplayperiodcountperiod = 5;
  unsigned int oneplayperiodcountperiodcount = 0; 
  
  
  unsigned int twostate = 1;
  unsigned int twolengthstate = 0;
  unsigned int twoplaystate = 0;
  
  unsigned int twoperiod = 500;
  unsigned int twoperiodcount = 0;

  unsigned int twolengthperiod = 10000; 
  unsigned int twolengthperiodcount = 0;

  unsigned int twoplayperiod = 40000;
  unsigned int twoplayperiodcount = 0;

  unsigned int twoplayperiodcountperiod = 5;
  unsigned int twoplayperiodcountperiodcount = 0;   
  
  unsigned int threestate = 1;
  unsigned int threelengthstate = 0;
  unsigned int threeplaystate = 0;
  
  unsigned int threeperiod = 500;
  unsigned int threeperiodcount = 0;

  unsigned int threelengthperiod = 10000; 
  unsigned int threelengthperiodcount = 0;

  unsigned int threeplayperiod = 40000;
  unsigned int threeplayperiodcount = 0;

  unsigned int threeplayperiodcountperiod = 5;
  unsigned int threeplayperiodcountperiodcount = 0;     


  //-------------------------- noise variables -------------------------//



	while (1)
	{
		

  //----------------------- Noise procedure -------------------------//

  if (oneplayperiodcount >= oneplayperiod)
    {
      oneplaystate = 1 - oneplaystate;
      oneplayperiodcount = 0;
    }
    if (oneplayperiodcountperiodcount >= oneplayperiodcountperiod)
    {
      oneplayperiodcount++;
      oneplayperiodcountperiodcount = 0;
    }
    oneplayperiodcountperiodcount++;
    
    if (oneplaystate)
    {
      if (onelengthperiodcount >= onelengthperiod)
      {
        onelengthstate = 1 - onelengthstate;
        onelengthperiodcount = 0;
      }
      onelengthperiodcount++;
      
      if (onelengthstate > 0)
      { 
        if(oneperiodcount >= oneperiod)
        {
          onestate = 1 - onestate;
          
          oneperiodcount = 0;     
        }
        oneperiodcount++;
      }
      else
      {
        onestate = 0;
      }
    }
    else 
    {
      onestate = 0;
    }
    
    
    if (twoplayperiodcount >= twoplayperiod)
    {
      twoplaystate = 1 - twoplaystate;
      twoplayperiodcount = 0;
    }
    if (twoplayperiodcountperiodcount >= twoplayperiodcountperiod)
    {
      twoplayperiodcount++;
      twoplayperiodcountperiodcount = 0;
    }
    twoplayperiodcountperiodcount++;
    
    if (twoplaystate)
    {
      if (twolengthperiodcount >= twolengthperiod)
      {
        twolengthstate = 1 - twolengthstate;
        twolengthperiodcount = 0;
      }
      twolengthperiodcount++;
      
      if (twolengthstate > 0)
      { 
        if(twoperiodcount >= twoperiod)
        {
          twostate = 1 - twostate;
          
          twoperiodcount = 0;     
        }
        twoperiodcount++;
      }
      else
      {
        twostate = 0;
      }
    }
    else 
    {
      twostate = 0;
    }
    
    if (twoplayperiodcount >= twoplayperiod)
    {
      twoplaystate = 1 - twoplaystate;
      twoplayperiodcount = 0;
    }
    if (twoplayperiodcountperiodcount >= twoplayperiodcountperiod)
    {
      twoplayperiodcount++;
      twoplayperiodcountperiodcount = 0;
    }
    twoplayperiodcountperiodcount++;
    
    
    
    
    if (threeplaystate)
    {
      if (threelengthperiodcount >= threelengthperiod)
      {
        threelengthstate = 1 - threelengthstate;
        threelengthperiodcount = 0;
      }
      threelengthperiodcount++;
      
      if (threelengthstate > 0)
      { 
        if(threeperiodcount >= threeperiod)
        {
          threestate = 1 - threestate;
          
          threeperiodcount = 0;     
        }
        threeperiodcount++;
      }
      else
      {
        threestate = 0;
      }
    }
    else 
    {
      threestate = 0;
    }   
    
    if (threeplayperiodcount >= threeplayperiod)
    {
      threeplaystate = 1 - threeplaystate;
      threeplayperiodcount = 0;
    }
    if (threeplayperiodcountperiodcount >= threeplayperiodcountperiod)
    {
      threeplayperiodcount++;
      threeplayperiodcountperiodcount = 0;
    }
    threeplayperiodcountperiodcount++;    
    
    
    // ^ = XOR:bitwise XOR each bit will result in a 1 if the input bits are different, or 0 if they are the same.
    // if any of the states are different this will evaluate as true
    if (twostate ^ onestate ^ threestate) {
    
      GPIO_WriteBit(GPIOA, GPIO_Pin_2, Bit_SET);
      GPIO_WriteBit(GPIOA, GPIO_Pin_3, Bit_SET );         // PORTB = PORTB | b00011000 : turn on PB3 and PB4, 24 | 24 or 0 | 24
      // GPIOA->MODER |= (1<<17);
      //TIM_SetCompare4(TIM2,Random()/100); 
    }else{
      
    // ~ = bitwise NOT: changes every single bit to its opposite
      GPIO_WriteBit(GPIOA, GPIO_Pin_2, Bit_RESET);
      GPIO_WriteBit(GPIOA, GPIO_Pin_3, Bit_RESET);     // PORTB = PORTB & b11100111 : turn off PB3 and PB4, 24 & 231 or  0 & 231
      
      //to get the mco to do anything cool i need to modulate the clock speed coming from it other wise im just turning it on and off like the others
      // GPIOA->MODER &= ~(1 << 17);
      
    }


    // if PINB (the portb data register used t read pins logical states)
    // & = bitwise AND if both input bits are 1, the resulting output is 1, otherwise the output is 0
    //if pin PB0 is low this evealutes as true because 1 is b00000001 in binary
    if (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) == 0)
    { 
      if(buttonCounter>65535){
        buttonCounter=0;
      }    
      buttonCounter=buttonCounter+(Random()/100);

      if(buttonCounter%4==0){
      

      threeperiod = Random()/400;
      threelengthperiod = Random()/5; 
      threeplayperiod = Random();
      
      twoperiod = Random()/400;
      twolengthperiod = Random()/5; 
      twoplayperiod = Random();
      
      oneperiod = Random()/200;
      onelengthperiod = Random()/5; 
      oneplayperiod = Random();
    }else if (buttonCounter%2==0){
      
      threeperiod = Random()/200;
       threelengthperiod = Random()/20; 
       threeplayperiod = Random()/5;
      
       twoperiod = Random()/200;
       twolengthperiod = Random()/20; 
       twoplayperiod = Random()/5;
      
       oneperiod = Random()/200;
       onelengthperiod = Random()/20; 
       oneplayperiod = Random()/5;
    }else if(buttonCounter%6 ==0){

        threeperiod = Random()/100;
       threelengthperiod = Random()/10; 
       threeplayperiod = Random()/3;
      
       twoperiod = Random()/100;
       twolengthperiod = Random()/10; 
       twoplayperiod = Random()/3;
      
       oneperiod = Random()/100;
       onelengthperiod = Random()/10; 
       oneplayperiod = Random()/3;
    }else if(buttonCounter%3==0){

        threeperiod = Random()/1000;
      threelengthperiod = Random()/1; 
      threeplayperiod = Random()/15;
      
      twoperiod = Random()/10;
      twolengthperiod = Random()/8; 
      twoplayperiod = Random();
      
      oneperiod = Random()/2500;
      onelengthperiod = Random()/12; 
      oneplayperiod = Random()/5;

    }else{
      threeperiod = Random()/1000;
      threelengthperiod = Random()/10; 
      threeplayperiod = Random();
      
      twoperiod = Random()/800;
      twolengthperiod = Random()/8; 
      twoplayperiod = Random();
      
      oneperiod = Random()/200;
      onelengthperiod = Random()/4; 
      oneplayperiod = Random();
    }

  }   
}
  return 0;

}

void belgianWarehouse(int x,int pause, int speed){
      
      for (int i = 0; i < x; ++i)
      {
      //fade up
      for (int i = 0; i <999; i ++){

        TIM_SetCompare4(TIM2,i);                                         //Pulse width is set with this function
        delay(speed);                                                      // the '2' relates to the channel in use
      }

      delay(pause);

      //fade down
      for (int i = 999; i >0; i --){

        TIM_SetCompare4(TIM2,i);
        delay(speed);
      }

      delay(pause);
    }
  
}

void delay( int a )
{
	volatile int i, j;

	for ( i = 0; i < a; i++ )
	{
		j++;
	}
}


unsigned long NextRandom = 1;
inline unsigned long Random(void)
{
   NextRandom = NextRandom * 1103515245 + 12345;
   return ((NextRandom >> 16) & 32767);
}