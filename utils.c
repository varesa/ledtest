#include <stm32f4xx.h>

#include "utils.h"

extern uint32_t DMABuffer[];

int32_t constrain(int32_t min, int32_t value, int32_t max) {
	if(value < min)
		return min;
	if(value > max)
		return max;
	return value;
}

unsigned char reverse(unsigned char b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}
