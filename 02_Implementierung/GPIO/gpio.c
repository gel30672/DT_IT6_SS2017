/*
 * gpio.c
 *
 *  Created on: 04.04.2017
 *      Author: andreas
 */

#include "gpio.h"
#include "wiringPi.h"

void gpio_init(void)
{
    wiringPiSetupGpio(); // Initialize wiringPi -- using Broadcom pin numbers
    pinMode(GPIO_PWM_1, PWM_OUTPUT);
}


