#include <stm32f4xx.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_dma.h>

#include "setup.h"
#include "utils.h"
#include "drv_leds.h"

void init_peripherals(void);

uint8_t advance = 0;
void advance_pattern(void);

void init_peripherals(void) {
	initClocks();
	SystemCoreClockUpdate();
	
	initGPIO();
	initTimer();
	initDMA();
	initInterrupts();
	
}

int main(void) {
	init_peripherals();
	
	buffer_clear();
	dma_start();
	
	leds_init();
	
	while(1) {
		if(advance) {
			advance = 0;
			advance_pattern();
		}
	}
}

void TIM2_IRQHandler(void) {
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	
	// Advance pattern
	advance = 1;
}

uint8_t red_pos = 0;
uint8_t red_dir = 1;

uint8_t green_pos = NUM_OF_LEDS;
uint8_t green_dir = 0;

uint8_t blue_pos = NUM_OF_LEDS/2;
uint8_t blue_dir = 0;


void advance_pattern(void) {
	leds_set_color(red_pos, 0, 0, 0);
	leds_set_color(green_pos, 0, 0, 0);
	leds_set_color(blue_pos, 0, 0, 0);
	
	if(red_dir)
		red_pos++;
	else
		red_pos--;
	
	if(red_pos == 0 || red_pos == NUM_OF_LEDS)
		red_dir = !red_dir;
	
	
	if(green_dir)
		green_pos++;
	else
		green_pos--;
	
	if(green_pos == 0 || green_pos == NUM_OF_LEDS)
		green_dir = !green_dir;
	
	if(blue_dir)
		blue_pos++;
	else
		blue_pos--;
	
	if(blue_pos == 0 || blue_pos == NUM_OF_LEDS)
		blue_dir = !blue_dir;
	
	
	leds_set_color(red_pos, 255, 0, 0);
	leds_set_color(green_pos, 0, 255, 0);
	leds_set_color(blue_pos, 0, 0, 255);
	
	leds_to_buffer();
}
