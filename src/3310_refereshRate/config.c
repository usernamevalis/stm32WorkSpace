#include "config.h"

void EnableClock(void)
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

void initializeGPIO(void){

    GPIO_InitTypeDef gpio;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

    GPIO_StructInit(&gpio);
    gpio.GPIO_Pin = GPIO_Pin_7; // green led
    gpio.GPIO_Mode = GPIO_Mode_OUT;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &gpio);

    gpio.GPIO_Pin = GPIO_Pin_6; // blue led
    GPIO_Init(GPIOB, &gpio);

}


void initializeSerial(uint32_t baudrate){

        GPIO_InitTypeDef gpio_init;         //for the RX TX pins 
        USART_InitTypeDef usart_init;
        USART_ClockInitTypeDef usart_clk_init;

        NVIC_InitTypeDef NVIC_InitStructure; // this is used to configure the NVIC (nested vector interrupt controller)
 
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
        //you have to use a different clock for USART2 and 3
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
 
        // PA9 = Tx, PA10 = Rx
        gpio_init.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
        gpio_init.GPIO_Mode = GPIO_Mode_AF;
        gpio_init.GPIO_Speed = GPIO_Speed_40MHz;
        gpio_init.GPIO_OType = GPIO_OType_PP;
        gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(GPIOA, &gpio_init);

        //convert IO into alternate functions as Rx and Tx
        //so usart can take them over
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
 
        //define properties of USART
        USART_ClockStructInit(&usart_clk_init);
        USART_ClockInit(USART2, &usart_clk_init);
 
        usart_init.USART_BaudRate =            baudrate;
        usart_init.USART_WordLength =          USART_WordLength_8b;
        usart_init.USART_StopBits =            USART_StopBits_1;
        usart_init.USART_Parity =              USART_Parity_No ;
        usart_init.USART_Mode =                USART_Mode_Rx | USART_Mode_Tx;
        usart_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_Init(USART2, &usart_init);

        /* Here the USART2 receive interrupt is enabled
         * and the interrupt controller is configured 
         * to jump to the USART2_IRQHandler() function
         * if the USART2 receive interrupt occurs
         */
        USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // enable the USART2 receive interrupt 
        
        NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;        // we want to configure the USART1 interrupts
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;// this sets the priority group of the USART1 interrupts
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;       // this sets the subpriority inside the group
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;          // the USART1 interrupts are globally enabled
        NVIC_Init(&NVIC_InitStructure);                          // the properties are passed to the NVIC_Init function which takes care of the low level stuff 

        // finally this enables the complete USART1 peripheral
        USART_Cmd(USART2, ENABLE);

}