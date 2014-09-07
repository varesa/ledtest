#include <stm32f4xx.h>

#ifndef DRV_LEDS_H_
#define DRV_LEDS_H_

#define NUM_OF_LEDS 240

// (0.85 / 1.25) * 120
#define PULSE_HIGH 41

// (0.45 / 1.25) * 120
#define PULSE_LOW 22

typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} RGB;

void dma_start(void);

void buffer_clear(void);

void leds_clear(void);
void leds_init(void);
void leds_set_color(uint8_t index, uint8_t r, uint8_t g, uint8_t b);

void leds_to_buffer(void);



#endif
