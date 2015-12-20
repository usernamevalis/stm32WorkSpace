/* read infor from serial port and display on lcd*/
#include "stm32l1xx_conf.h"
#include "nokialcd.h"
#include "main.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

// RX = PA3
// TX = PA2

//USART_FLAG_RXNE - recieve data register not empty
//USART_FLAG_TXE -   transmit data register empty 
//USART_FLAG_TC - Transmission Complete flag.
static volatile uint32_t TimingDelay;
void TimingDelay_Decrement(void);
void delayms(uint32_t nTime);
void SysTick_Handler(void);

#define MAX_STRLEN 504 // 
volatile uint8_t serialImage [MAX_STRLEN+1];
uint16_t cnt =0;

void USART2_IRQHandler(void);
void usart_writeChar(USART_TypeDef* USARTx, uint8_t ch);
void usart_writeString(USART_TypeDef* USARTx, char *msg );
uint8_t usart_read(void);
uint8_t usart_available(void);
void lcdImage(void);

int main(void) {

    EnableClock();
    RCC_ClocksTypeDef RCC_Clocks;
    RCC_GetClocksFreq(&RCC_Clocks);

    SysTick_Config((RCC_Clocks.SYSCLK_Frequency / 2) / 1000); // Cannot exceed 16,777,215

    /* Set SysTick Preemption Priority, it's a system handler rather than a regular interrupt */
    NVIC_SetPriority(SysTick_IRQn, 0x04);

    initializeGPIO();
    lcdInit();
    lcdClear();
    lcdXY(1,1);
    lcdStr("working");
    initializeSerial(921600);
    
    //usart_writeString(USART2, "\r\nInit complete! Hello World!\r\n"); // just send a message to indicate that it works
    //usart_writeChar(USART2,'A');
     usart_writeChar(USART2,'A');
lcdXY(0,0);
    while (1){  
    /*
     * You can do whatever you want in here 
     */
    }
}

void lcdImage(void)
{

  //lcdClear();
    
  for ( int i = 0; i < ( 84 * 48 ) / 8; i++)
  {
    lcdWrite(LCD_DATA, serialImage[i]);
  }

    memset(serialImage,0,MAX_STRLEN+1);
    usart_writeChar(USART2,'A');
    
     
}

void USART2_IRQHandler(void){
    
    // check if the USART1 receive interrupt flag was set
    if( USART_GetITStatus(USART2, USART_IT_RXNE) ){
        
        // this counter is used to determine the string length
        uint8_t t = usart_read(); // the character from the USART1 data register is saved in t
        
        /* check if the received character is not the LF character (used to determine end of string) 
         * or the if the maximum string length has been been reached 
         */
        if(cnt < MAX_STRLEN){ 
            serialImage[cnt] = t;
            cnt++;
        }
        else{ // otherwise reset the character counter and print the received string
            cnt = 0;

            lcdImage(); 
        }
    }
}

void delayms(uint32_t nTime)
{
  TimingDelay = nTime;
  while(TimingDelay != 0);
}

// Called from SysTick_Handler
 
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00){
    TimingDelay--;}
}
 
void SysTick_Handler(void)
{
    TimingDelay_Decrement();
}
// this is the interrupt request handler (IRQ) for ALL USART1 interrupts

void usart_writeChar(USART_TypeDef* USARTx, uint8_t ch)
{

    while(USART_GetFlagStatus(USARTx,USART_FLAG_TXE)==RESET) //wait untill register is empty
    {
    }
    USART_SendData(USARTx, (uint8_t) ch);                    //write data to register and send
    while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET) //wait untill transmission is complete 
    {
    }
}

void usart_writeString(USART_TypeDef* USARTx, char *msg )
{
    int len = strlen( msg );

    for ( int c = 0; c < len; c++ )
        usart_writeChar(USARTx, (uint8_t)*msg++ );
}

uint8_t usart_read(void){
     while ( USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);
     return (uint8_t)USART_ReceiveData(USART2);
}

uint8_t usart_available(void)
{
    if ( USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == SET ){
        return 1;
    }else{
      return 0;   
    }     
}

















