#include <stdlib.h>
#include <string.h>

#include "drv_leds.h"

extern uint32_t DMABuffer[];
uint32_t DMABuffer[24*NUM_OF_LEDS];

RGB leds[NUM_OF_LEDS];

void dma_start(void) {
	DMA_SetCurrDataCounter(DMA1_Stream2, NUM_OF_LEDS*24 + 64);
	DMA_Cmd(DMA1_Stream2, ENABLE);
	TIM_Cmd(TIM5, ENABLE);
}

void buffer_clear(void) {
	memset(DMABuffer, 0, NUM_OF_LEDS*3*8 + 64);
}

void buffer_init(void) {
	buffer_clear();
}

void leds_clear(void) {
	memset(leds, 0, 3*NUM_OF_LEDS);
}

void leds_init(void) {
	leds_clear();
	
	leds_set_color(0, 255, 0, 0);
	leds_set_color(1, 0, 255, 0);
	leds_set_color(2, 0, 0, 255);
	
	leds_to_buffer();
}

void leds_set_color(uint8_t index, uint8_t r, uint8_t g, uint8_t b) {
	leds[index].r = r;
	leds[index].g = g;
	leds[index].b = b;
}

void leds_to_buffer(void) {
	int index, i, r, g, b;
	
	for(index = 0; index < NUM_OF_LEDS; index++) {
		r = leds[index].r;
		g = leds[index].g;
		b = leds[index].b;
		
		// Green
		for(i = 0; i < 8; i++) {
			if(g & (1 << i))
				DMABuffer[24 * index + i] = PULSE_HIGH;
			else
				DMABuffer[24 * index + i] = PULSE_LOW;
		}
		
		// Red
		for(i = 0; i < 8; i++) {
			if(r & (1 << i))
				DMABuffer[24 * index + 8 + i] = PULSE_HIGH;
			else
				DMABuffer[24 * index + 8 + i] = PULSE_LOW;
		}
		
		// Blue
		for(i = 0; i < 8; i++) {
			if(b & (1 << i))
				DMABuffer[24 * index + 16 + i] = PULSE_HIGH;
			else
				DMABuffer[24 * index + 16 + i] = PULSE_LOW;
		}
	}
}
