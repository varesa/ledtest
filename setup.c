#include <stm32f4xx.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_dma.h>

#include "setup.h"
#include "drv_leds.h"

extern uint8_t DMABuffer[];

void initClocks(void) {	
	/*
	HSE: 8 MHz
	PLL: HSE / 4 * 192 => 384 MHz
	SysCLK:  PLL / 4 => 96 MHz
	PrphCLK: PLL / 8 => 48 MHz
	*/
	RCC_DeInit();
	
	RCC_HSEConfig(RCC_HSE_ON);
	
	RCC_PLLConfig(RCC_PLLSource_HSE, 4, 192, 4, 8);
	RCC_PLLCmd(ENABLE);
	
	// Wait
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) 
		continue;

	RCC_HCLKConfig(RCC_SYSCLK_Div1);
	RCC_PCLK1Config(RCC_HCLK_Div4);
	RCC_PCLK2Config(RCC_HCLK_Div2);
	
	// Use PLL
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

	while (RCC_GetSYSCLKSource() != 0x08) continue;
}

void initGPIO(void) {
	GPIO_InitTypeDef GPIO_init;
	
	// Initialize clock
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	// PWM Output (TIM5 - CC1)
	GPIO_StructInit(&GPIO_init);
	GPIO_init.GPIO_Mode = GPIO_Mode_AF;
	GPIO_init.GPIO_OType = GPIO_OType_PP;
	GPIO_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_init.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_init.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA, &GPIO_init);
}

void initTimer(void) {
	
	NVIC_InitTypeDef NVIC_init;
	TIM_TimeBaseInitTypeDef TIM_init;
	TIM_OCInitTypeDef OC_init;
	
	/* NVIC */
	
	NVIC_init.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_init.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_init.NVIC_IRQChannelSubPriority = 1;
	NVIC_init.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_init);
	
	/* Timers */
		
	// Pattern update timer
	/*RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	TIM_init.TIM_Prescaler = 48000; // 48 MHz -> 1 kHz
	TIM_init.TIM_Period = 20; // 100 cycles @ 1kHz => 0.05ms
	TIM_init.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_init.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_init);

	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM2, ENABLE);*/
	
	// PWM timer
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	
	TIM_init.TIM_Prescaler = 0;
	TIM_init.TIM_Period = 60; // 60 cycles => 1.25µS
	TIM_init.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_init.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM5, &TIM_init);
	
	// PWM Output
	TIM_OCStructInit(&OC_init);
	OC_init.TIM_OCMode = TIM_OCMode_PWM1;
	OC_init.TIM_Pulse = 50; // Higher than PULSE_HIGH to test on scope
	OC_init.TIM_OutputState = TIM_OutputState_Enable;
	OC_init.TIM_OCPolarity = TIM_OCPolarity_High;
	
	TIM_OC1Init(TIM5, &OC_init);
	//TIM_OC1PreloadConfig(TIM5, TIM_OCPreload_Enable);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_TIM5);	
}

void initDMA() {
	DMA_InitTypeDef DMA_init;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	
	DMA_DeInit(DMA1_Stream2);
	DMA_StructInit(&DMA_init);
	
	DMA_init.DMA_Channel = DMA_Channel_6; 										// DMA1 Stream2 Channel6 = TIM5_CH1
	DMA_init.DMA_PeripheralBaseAddr = (uint32_t) TIM5->CCR1;	// Timer compare register
	DMA_init.DMA_Memory0BaseAddr = (uint32_t) DMABuffer;
	DMA_init.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_init.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_init.DMA_MemoryInc = DMA_MemoryInc_Disable;
	DMA_init.DMA_BufferSize = 1;
	DMA_init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_init.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;
	DMA_init.DMA_Mode = DMA_Mode_Normal;
	DMA_init.DMA_Priority = DMA_Priority_High;
  DMA_init.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_init.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_init.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	
	DMA_Init(DMA1_Stream2, &DMA_init);
	
	TIM_Cmd(TIM5, ENABLE); 					
	DMA_Cmd(DMA1_Stream2, ENABLE);
	TIM_DMACmd(TIM5, TIM_DMA_CC1, ENABLE);
}
