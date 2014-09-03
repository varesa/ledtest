#include <stm32f4xx.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_dma.h>

#include <stdlib.h>
#include <string.h>

#include "setup.h"
#include "drv_leds.h"
#include "utils.h"

extern uint8_t DMABuffer[];

uint8_t DMABuffer[] = {
	10
};


int main(void) {
	initClocks();
	SystemCoreClockUpdate();
	
	initGPIO();
	initTimer();
	initDMA();
	
	while(1) {
	}
}
