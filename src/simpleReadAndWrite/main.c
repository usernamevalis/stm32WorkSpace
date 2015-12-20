#include "stm32l1xx_conf.h"
#include <stdio.h>

int main(void);
void delay(int a);

int main(void)
{
	GPIO_InitTypeDef gpio;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

	GPIO_StructInit(&gpio);
	gpio.GPIO_Pin = GPIO_Pin_10;	//input
	gpio.GPIO_Mode = GPIO_Mode_IN;
	gpio.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOB, &gpio);

	gpio.GPIO_Pin = GPIO_Pin_8;	// output
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_Speed = GPIO_Speed_2MHz;
	gpio.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &gpio);


	static int led_state=0;

	while (1)
	{
		// port output - alternate between green and blue
		led_state=GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10);

		if(led_state)
		{
			GPIO_WriteBit(GPIOC, GPIO_Pin_8,led_state);

		}else{

			GPIO_WriteBit(GPIOC, GPIO_Pin_8,led_state);
		}
		
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
