/*
 * Steering.c
 *
 *  Created on: 14.04.2017
 *      Author: pwina
 */
#include "steering.h"

#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include "../GPIO/gpio.h"

void HwPwmSteeringInit()
{
	pinMode(GPIO_PWM_HW, PWM_OUTPUT);
	pwmSetMode(PWM_MODE_MS);
	pwmSetClock(HW_PWM_CLOCK);
	pwmSetRange(HW_PWM_RANGE);
}

e_state SteerDegrees(int degree)
{
	if(degree>-26 && degree<26)
	{
		pwmWrite(GPIO_PWM_HW, degree + STEERING_STRAIGHT);
		return E_OK;
	}
	else return E_NOT_OK;
}
