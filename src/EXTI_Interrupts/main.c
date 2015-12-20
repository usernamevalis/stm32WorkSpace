/* example showing how to use stm32 interrupts
 * This example alternates the status of the onboard leds when ever the interrupt on PA5 is triggered with a falling edge
 * In the main loop PB11 is continuously blinked on/off to show that this delay and operation has no effect on
 * the interrupts and vice versa*/
EXTI_InitTypeDef   EXTI_InitStructure;
GPIO_InitTypeDef   GPIO_InitStructure;
NVIC_InitTypeDef   NVIC_InitStructure;

void EXTI0_Config(void);
void EXTI9_5_Config(void);
void EXTI15_10_Config(void);
void GPIO_Config(void);

static int led_state=0;
static int led_state2=0;

int main(void)
{

  EXTI9_5_Config();
  EXTI0_Config();
  GPIO_Config();
  /* Use this function to generate software interrupt: simulate a falling edge applied on EXTI0 line */
 //EXTI_GenerateSWInterrupt(EXTI_Line0);

  while (1)
  {
    /* continuously blink PB11 */
    led_state2 = !led_state2;
    GPIO_WriteBit(GPIOB, GPIO_Pin_11, led_state2 ? Bit_RESET : Bit_SET );
    for(int i =0;i<50000;i++){
        
    }
  }
}
/*-----------------------configure GPIO Pins----------------------------*/
/* We will blink these useing interrupts */
void GPIO_Config(void){

  GPIO_InitTypeDef gpio;
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

  GPIO_StructInit(&gpio);
  gpio.GPIO_Pin = GPIO_Pin_7; // onboard green led
  gpio.GPIO_Mode = GPIO_Mode_OUT;
  gpio.GPIO_Speed = GPIO_Speed_40MHz;
  gpio.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &gpio);

  gpio.GPIO_Pin = GPIO_Pin_6; // onboard blue led
  GPIO_Init(GPIOB, &gpio);

  gpio.GPIO_Pin = GPIO_Pin_11; 
  gpio.GPIO_Mode = GPIO_Mode_OUT;
  gpio.GPIO_Speed = GPIO_Speed_40MHz;
  gpio.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &gpio);
}

/*-----------------------Configure Interrupts Pins----------------------------*/
/* Set and Configure the pins you want to use as interrupts*/
/* Set and Configure PA5 as interrupt*/
void EXTI9_5_Config(void)
{
  /* Enable GPIOx clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  /* Configure PA5 pin as input floating */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  /* Config PA5 pin as interrupt and connect it to EXTI_Line5*/
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource5);
  /* The Stm32l1xx interrupts can me mapped to any pins on the board so long as they are not used for internal purposes 
   * Available interrupts are numbered from EXTI_Line0 - EXTI_Line15, the interrupt line numbers needs to correspond to
   * the pin you are using. To use PA5 as an interrupt we connect PA5 to EXTI_Line5 
   * Configure EXTI5 line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line5;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set EXTI5 Interrupt to the lowest priority 
   * To access the interrupts you use one of 3 IRQChannels: EXTI0_IRQn(give access to EXTI_Lines 0-4), 
   * EXTI9_5_IRQn(give access to EXTI_Lines 5-9) and EXTI10_15_IRQn(give access to EXTI_Lines 10-15).
   * This function set PA5 as interupt and therefore uses EXTI_Line5 and IRQnChannel EXTI9_5_IRQn.
   * The function below uses PA0 and EXTI0_IRQn, not you can use any port just set the clock and port accordingly
   * Depending on the number of your interrupt pin, set the correct channel to confgure the interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/* Set and Configure PA0 as interrupt*/
void EXTI0_Config(void)
{
  /* Enable GPIOx clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  /* Configure PA0 pin as input floating */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  /* Connect EXTI0 Line to PA0 pin */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

  /* Configure EXTI0 line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set EXTI0 Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
/*-----------------------Interrupt Service Routines (ISR)------------------------*/
/* These are the functions that are called when interrupts are triggered
 * Use the IRQnChannel to access the Interupts flags, the EXTI9_5_IRQhandler gives access to EXTI_Lines5-9 etc.
*/

/* ISR for interrupts on EXTI_Lines9-5 */ 
void EXTI9_5_IRQHandler(void)
{
  /* Check the Interrupt flag status */
  if(EXTI_GetITStatus(EXTI_Line5) != RESET)
  {
    led_state = !led_state;
    GPIO_WriteBit(GPIOB, GPIO_Pin_7, led_state ? Bit_SET : Bit_RESET);
    GPIO_WriteBit(GPIOB, GPIO_Pin_6, led_state ? Bit_RESET : Bit_SET );
  /* clear the flag so it can be set again the next time the interrupt accurs*/  
    EXTI_ClearITPendingBit(EXTI_Line5);
  }
}

/* ISR for interrupts on EXTI_Lines0-4 */ 
void EXTI0_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line0) != RESET)
  {
    //code to run when interrupt is triggered

    EXTI_ClearITPendingBit(EXTI_Line0);
  }
}



