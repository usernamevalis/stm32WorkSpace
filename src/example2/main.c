#include "stm32l1xx_conf.h"
#include <stdio.h>

int main(void);
void delay(int a);

int main(void)
{
	GPIO_InitTypeDef gpio;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	GPIO_StructInit(&gpio);
	gpio.GPIO_Pin = GPIO_Pin_2;	// green led
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_Speed = GPIO_Speed_2MHz;
	gpio.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &gpio);

	gpio.GPIO_Pin = GPIO_Pin_3;	// blue led
	GPIO_Init(GPIOA, &gpio);

	static int led_state=0;

	while (1)
	{
		// port output - alternate between green and blue
		led_state = !led_state;
		GPIO_WriteBit(GPIOA, GPIO_Pin_2, led_state ? Bit_SET : Bit_RESET);
		GPIO_WriteBit(GPIOA, GPIO_Pin_3, led_state ? Bit_RESET : Bit_SET );
		delay(50000);
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
