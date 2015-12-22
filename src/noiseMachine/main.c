#include "stm32l1xx_conf.h"
#include <stdio.h>

int main(void);
void delay(int a);

//noise resources
void delayms(uint16_t millis);
void delayuss(unsigned int micros);
inline unsigned long Random(void);

int main(void)
{
  //--------------------------GPIO init ----------------------------//
  //--------output---------//
	GPIO_InitTypeDef gpio;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	GPIO_StructInit(&gpio);
	gpio.GPIO_Pin = GPIO_Pin_2;	// green led
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_Speed = GPIO_Speed_40MHz;
	gpio.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &gpio);

	gpio.GPIO_Pin = GPIO_Pin_3;	// blue led
	GPIO_Init(GPIOA, &gpio);

   //--------input---------//

  gpio.GPIO_Pin = GPIO_Pin_0; 
  gpio.GPIO_Mode = GPIO_Mode_IN;
  gpio.GPIO_Speed = GPIO_Speed_40MHz;
  gpio.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(GPIOA, &gpio);

  gpio.GPIO_Pin = GPIO_Pin_1; 
  gpio.GPIO_Mode = GPIO_Mode_IN;
  gpio.GPIO_Speed = GPIO_Speed_40MHz;
  gpio.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(GPIOA, &gpio);
  //--------------------------GPIO init ----------------------------//

	static int led_state=0;


  //-------------------------- noise variables -------------------------//
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
      
    }else{
      
    // ~ = bitwise NOT: changes every single bit to its opposite
      GPIO_WriteBit(GPIOA, GPIO_Pin_2, Bit_RESET);
      GPIO_WriteBit(GPIOA, GPIO_Pin_3, Bit_RESET);     // PORTB = PORTB & b11100111 : turn off PB3 and PB4, 24 & 231 or  0 & 231
      
    }

    // if PINB (the portb data register used t read pins logical states)
    // & = bitwise AND if both input bits are 1, the resulting output is 1, otherwise the output is 0
    //if pin PB0 is low this evealutes as true because 1 is b00000001 in binary
    if (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) == 1)
    {     
       threeperiod = Random()/200;
       threelengthperiod = Random()/20; 
       threeplayperiod = Random()/5;
      
       twoperiod = Random()/200;
       twolengthperiod = Random()/20; 
       twoplayperiod = Random()/5;
      
       oneperiod = Random()/200;
       onelengthperiod = Random()/20; 
       oneplayperiod = Random()/5;
    }
    //if pin PB1 is low this evealutes as true because 2 is b00000010 in binary
    if (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1) == 1)
    {     
      threeperiod = Random()/400;
      threelengthperiod = Random()/5; 
      threeplayperiod = Random();
      
      twoperiod = Random()/400;
      twolengthperiod = Random()/5; 
      twoplayperiod = Random();
      
      oneperiod = Random()/200;
      onelengthperiod = Random()/5; 
      oneplayperiod = Random();
    
    }   
  }
  
  return 0;

}

void delay( int a )
{
	volatile int i, j;

	for ( i = 0; i < a; i++ )
	{
		j++;
	}
}

// /* at 8 MHz we get 1us per 8 instructions */
// inline void delayus() { asm volatile("nop\nnop\nnop\nnop\n"
//                                      "nop\nnop\nnop\nnop"); 
//Resources 

void delayuss(unsigned int micros)
{
  while (micros)
  {
    delayus();
    micros--;
  }
}

void delayms(uint16_t millis) {
  uint16_t loop;
  while ( millis ) {
    loop = 100;
    while (loop) {
      /* 20us of delays */
      delayus(); delayus(); delayus(); delayus(); delayus();
      delayus(); delayus(); delayus(); delayus(); delayus();
      loop --;
    }
    millis--;
  }
}

unsigned long NextRandom = 1;
inline unsigned long Random(void)
{
   NextRandom = NextRandom * 1103515245 + 12345;
   return ((NextRandom >> 16) & 32767);
}