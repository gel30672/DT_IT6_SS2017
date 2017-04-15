#include "isr.h"
#include "../GPIO/gpio.h"

#include <wiringPi.h>

void isr_init()
{
	/* isr for rotary sensor */
	wiringPiISR (GPIO_ROTARY, INT_EDGE_RISING, &isr_gpio_pin27);
}

void isr_gpio_pin27()
{
	/* do rotary sensor work here */
}
