/*This is an example for the stm32l152 that uses interrupts to read USART data
 * this code is a combiantion of devthrash (https://github.com/devthrash/STM32F4-examples/blob/master/USART/main.c)
 * and tomvdb (https://github.com/tomvdb)
 */

#include "stm32l1xx_conf.h"
#include "stm32l1xx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

// RX = PA10
// TX = PA9

//USART_FLAG_RXNE - recieve data register not empty
//USART_FLAG_TXE -   transmit data register empty 
//USART_FLAG_TC - Transmission Complete flag.

#define MAX_STRLEN 8 // this is the maximum string length of our string in characters
volatile char received_string[MAX_STRLEN+1]; // this will hold the recieved string


void Delay(__IO uint32_t nCount) {

  while(nCount--) {
  
  }
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
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
 
        // PA9 = Tx, PA10 = Rx
        gpio_init.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
        gpio_init.GPIO_Mode = GPIO_Mode_AF;
        gpio_init.GPIO_Speed = GPIO_Speed_40MHz;
        gpio_init.GPIO_OType = GPIO_OType_PP;
        gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(GPIOA, &gpio_init);

        //convert IO into alternate functions as Rx and Tx
        //so usart can take them over
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
 
        //define properties of USART
        USART_ClockStructInit(&usart_clk_init);
        USART_ClockInit(USART1, &usart_clk_init);
 
        usart_init.USART_BaudRate =            baudrate;
        usart_init.USART_WordLength =          USART_WordLength_8b;
        usart_init.USART_StopBits =            USART_StopBits_1;
        usart_init.USART_Parity =              USART_Parity_No ;
        usart_init.USART_Mode =                USART_Mode_Rx | USART_Mode_Tx;
        usart_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_Init(USART1, &usart_init);

        /* Here the USART1 receive interrupt is enabled
         * and the interrupt controller is configured 
         * to jump to the USART1_IRQHandler() function
         * if the USART1 receive interrupt occurs
         */
        USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // enable the USART1 receive interrupt 
        
        NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;        // we want to configure the USART1 interrupts
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;// this sets the priority group of the USART1 interrupts
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;       // this sets the subpriority inside the group
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;          // the USART1 interrupts are globally enabled
        NVIC_Init(&NVIC_InitStructure);                          // the properties are passed to the NVIC_Init function which takes care of the low level stuff 

        // finally this enables the complete USART1 peripheral
        USART_Cmd(USART1, ENABLE);

}

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
     while ( USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
     return (uint8_t)USART_ReceiveData(USART1);
}

uint8_t usart_available(void)
{
    if ( USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET ){
        return 1;
    }else{
      return 0;   
    }     
}

int main(void) {

    initializeGPIO();
    initializeSerial(9600);
    
    usart_writeString(USART1, "\r\nInit complete! Hello World!\r\n"); // just send a message to indicate that it works

    while (1){  
    /*
     * You can do whatever you want in here 
     */
    }
}


// this is the interrupt request handler (IRQ) for ALL USART1 interrupts
void USART1_IRQHandler(void){
    
    // check if the USART1 receive interrupt flag was set
    if( USART_GetITStatus(USART1, USART_IT_RXNE) ){
        
        static uint8_t cnt = 0; // this counter is used to determine the string length
        char t = USART1->DR; // the character from the USART1 data register is saved in t
        
        /* check if the received character is not the LF character (used to determine end of string) 
         * or the if the maximum string length has been been reached 
         */
        if( (t != '\n') && (cnt < MAX_STRLEN) ){ 
            received_string[cnt] = t;
            cnt++;
        }
        else{ // otherwise reset the character counter and print the received string
            cnt = 0;

            if(strstr(received_string,"on")!=NULL){      //search string for substring and return pointer to the first occurence of the serach term 

                GPIO_WriteBit(GPIOB, GPIO_Pin_7, 1);

            }else if(strstr(received_string,"off")!=NULL){

                GPIO_WriteBit(GPIOB, GPIO_Pin_7, 0);
            }
            usart_writeString(USART1, received_string); //you will get warning about the volitale argument. you should do this elsewhere anyway its just POC
            usart_writeString(USART1, "\r\n");
            memset(received_string,0,MAX_STRLEN+1);

        }
           
    }
}


















