/* generate signals with DAC using DMA transfer
 * this example comes with
 * the std perip lib as has been reworked to run on the stm32l152 discovery board
*/

#define DAC_DHR12R2_Address      0x40007414
#define DAC_DHR8R1_Address       0x40007410

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
DAC_InitTypeDef            DAC_InitStructure;
DMA_InitTypeDef            DMA_InitStructure;
EXTI_InitTypeDef   EXTI_InitStructure;
GPIO_InitTypeDef   GPIO_InitStructure;
NVIC_InitTypeDef   NVIC_InitStructure;

const uint16_t Sine12bit[32] = {
                      2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056, 4095, 4056,
                      3939, 3750, 3495, 3185, 2831, 2447, 2047, 1647, 1263, 909, 
                      599, 344, 155, 38, 0, 38, 155, 344, 599, 909, 1263, 1647};	  
const uint8_t Escalator8bit[6] = {0x0, 0x33, 0x66, 0x99, 0xCC, 0xFF};
uint8_t Idx = 0;
__IO uint8_t  SelectedWavesForm = 1;
__IO uint8_t WaveChange = 1; 
int lightState = 0;


/* Private function prototypes -----------------------------------------------*/
void debugLights(void);
void DAC_PreConfig(void);
void EXTI15_10_Config(void);
void TIM2_config(void);

int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32l1xx_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32l1xx.c file
     */     

  /* Preconfiguration before using DAC----------------------------------------*/
  TIM2_config(); 
  debugLights();
  EXTI15_10_Config();
  DAC_PreConfig();
  

  while (1)
  {
    /* If the wave form is changed */
    if (WaveChange == 1)
    {  
      /* Switch the selected waves forms according the Button status */
      if (SelectedWavesForm == 1)
      {
          /* The sine wave and the escalator wave has been selected */
          /* Sine Wave generator ---------------------------------------------*/
          DAC_DeInit(); 
          /* DAC channel1 and channel2 Configuration */
          DAC_InitStructure.DAC_Trigger = DAC_Trigger_T2_TRGO;
          DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
          DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;

          DMA_DeInit(DMA1_Channel3); 
          DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12R2_Address;
          DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&Sine12bit;
          DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
          DMA_InitStructure.DMA_BufferSize = 32;
          DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
          DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
          DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
          DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
          DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
          DMA_InitStructure.DMA_Priority = DMA_Priority_High;
          DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
          DMA_Init(DMA1_Channel3, &DMA_InitStructure);

          /* Enable DMA1 Channel3 */
          DMA_Cmd(DMA1_Channel3, ENABLE);

            /* DAC Channel2 Init */
          DAC_Init(DAC_Channel_2, &DAC_InitStructure);

          /* Enable DAC Channel2 */
          DAC_Cmd(DAC_Channel_2, ENABLE);

          /* Enable DMA for DAC Channel2 */
          DAC_DMACmd(DAC_Channel_2, ENABLE);
          
          /* Escalator Wave generator ----------------------------------------*/

          /* DAC channel1 Configuration */
          DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
          DAC_Init(DAC_Channel_1, &DAC_InitStructure);
  
          /* DMA1 channel2 configuration */
          DMA_DeInit(DMA1_Channel2);
  
          DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR8R1_Address;
          DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&Escalator8bit;
          DMA_InitStructure.DMA_BufferSize = 6;
          DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
          DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
          DMA_Init(DMA1_Channel2, &DMA_InitStructure);
    
          /* Enable DMA1 Channel2 */
          DMA_Cmd(DMA1_Channel2, ENABLE);
    
          /* Enable DAC Channel1: Once the DAC channel1 is enabled, PA.05 is 
             automatically connected to the DAC converter. */
          DAC_Cmd(DAC_Channel_1, ENABLE);

          /* Enable DMA for DAC Channel1 */
          DAC_DMACmd(DAC_Channel_1, ENABLE);
      }
          /* The triangle wave and the noise wave has been selected */
       else
       {
          /* Noise Wave generator --------------------------------------------*/
          /* DAC channel1 Configuration */
          DAC_InitStructure.DAC_Trigger = DAC_Trigger_T2_TRGO;
          DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_Noise;
          DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bits8_0;
          DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
          DAC_Init(DAC_Channel_1, &DAC_InitStructure);

          /* Enable DAC Channel1: Once the DAC channel1 is enabled, PA.04 is 
             automatically connected to the DAC converter. */
          DAC_Cmd(DAC_Channel_1, ENABLE);

          /* Set DAC Channel1 DHR12L register */
          DAC_SetChannel1Data(DAC_Align_12b_L, 0x7FF0);
    
          /* Enable DAC channel1 wave generator */
          DAC_WaveGenerationCmd(DAC_Channel_1, DAC_Wave_Noise , ENABLE);
          
          /* Triangle Wave generator -----------------------------------------*/
          DAC_DeInit();
          
          /* DAC channel2 Configuration */
          DAC_InitStructure.DAC_Trigger = DAC_Trigger_T2_TRGO;
          DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_Triangle;
          DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_TriangleAmplitude_1023;
          DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
          DAC_Init(DAC_Channel_2, &DAC_InitStructure);

          /* Enable DAC Channel2: Once the DAC channel2 is enabled, PA.05 is 
             automatically connected to the DAC converter. */
          DAC_Cmd(DAC_Channel_2, ENABLE);

          /* Set DAC channel2 DHR12RD register */
          DAC_SetChannel2Data(DAC_Align_12b_R, 0x100);
      }
      WaveChange = !WaveChange;
    }
  }

}

void debugLights(){

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; // green led
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

}

void TIM2_config(){

  /* TIM2 Configuration ------------------------------------------------------*/
  /* TIM2 Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  /* Time base configuration */
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 
  TIM_TimeBaseStructure.TIM_Period = 255;          
  TIM_TimeBaseStructure.TIM_Prescaler = 0;       
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;    
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  /* TIM2 TRGO selection */
  TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);
  
  /* TIM2 enable counter */
  TIM_Cmd(TIM2, ENABLE);
  
}

//configure line pa12 as interrupt
void EXTI15_10_Config(void)
{
  /* Enable GPIOx clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  /* Configure PA12 pin as input floating */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  /* Config PA5 pin as interrupt and connect it to EXTI_Line12*/
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource12);

  EXTI_InitStructure.EXTI_Line = EXTI_Line12;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  PrecConfiguration: configure PA4 and PA5 in analog,
  *                           enable DAC clock, enable DMA1 clock
  * @param  None
  * @retval None
  */
void DAC_PreConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* DMA1 clock enable (to be used with DAC) */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  /* DAC Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

  /* GPIOA clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  /* Configure PA.04 (DAC_OUT1), PA.05 (DAC_OUT2) as analog */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}


void EXTI15_10_IRQHandler(void){

  if(EXTI_GetITStatus(EXTI_Line12) != RESET)
  { 
    /* Change the wave */
    WaveChange = !WaveChange;
    
    lightState = !lightState;
    GPIO_WriteBit(GPIOB, GPIO_Pin_7, lightState? Bit_SET : Bit_RESET);

     //Change the selected waves forms 
    SelectedWavesForm = !SelectedWavesForm;

    /* Clear the Right Button EXTI line pending bit */
    EXTI_ClearITPendingBit(EXTI_Line12);
  }
}

