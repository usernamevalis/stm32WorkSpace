#include "stm32l1xx_conf.h"
#include <stdio.h>
#include "mini-printf.h"
#include "discoverylcd.h"

/*
  Reworked example 7 to use the onboard lcd as a debugging tool
*/

char strDisp[20] ;
void RCC_Configuration();

int main()
{
  RCC_Configuration();

  RCC_ClocksTypeDef RCC_Clocks;
  RCC_GetClocksFreq(&RCC_Clocks);

  //SysTick_Config((RCC_Clocks.SYSCLK_Frequency / 2) / 1000); // Cannot exceed 16,777,215
 

	LCD_GLASS_Configure_GPIO();
	LCD_GLASS_Init();
  
  LCD_GLASS_DisplayString("display a string");  //to display strings
  LCD_GLASS_Clear();
 
  

  while (1)
  {
    
      mini_snprintf( strDisp, 10,  "%d", (SystemCoreClock/ 2) / 1000);   //to display int's etc
      LCD_GLASS_DisplayString(strDisp);
      
  }
}


void RCC_Configuration()
{  
  
  /* Enable comparator, LCD and PWR mngt clocks */
  // RCC_APB1Periph_COMP |
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_LCD | RCC_APB1Periph_PWR,ENABLE);
    
  /* Enable ADC & SYSCFG clocks */

  /* Allow access to the RTC */
  PWR_RTCAccessCmd(ENABLE);

  /* Reset RTC Backup Domain */
  RCC_RTCResetCmd(ENABLE);
  RCC_RTCResetCmd(DISABLE);

  /* LSE Enable */
  RCC_LSEConfig(RCC_LSE_ON);

  /* Wait until LSE is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
  
   /* RTC Clock Source Selection */ 
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE); 
  
  /* Enable the RTC */
  RCC_RTCCLKCmd(ENABLE);   

  // // divide HCLK / 2
  // RCC_HCLKConfig(RCC_SYSCLK_Div2);

  // // enable HSI
  // RCC_HSICmd(ENABLE);
  // RCC_PLLCmd(DISABLE);
  // // wait for HSI to get ready
  // while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);

  // // configure PLL - x4 /2 
  // RCC_PLLConfig( RCC_PLLSource_HSI, RCC_PLLMul_8,  RCC_PLLDiv_4 );
  // RCC_PLLCmd(ENABLE);

  // while ( RCC_GetFlagStatus( RCC_FLAG_PLLRDY ) == RESET );

  // // set hsi as clock
  // RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);

  // // set pll as clock
  // RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  
}