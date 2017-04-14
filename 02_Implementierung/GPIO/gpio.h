/*
 * gpio.h
 *
 *  Created on: 04.04.2017
 *      Author: andreas
 */

#ifndef GPIO_GPIO_H_
#define GPIO_GPIO_H_

#define SET_PWM_VALUE(x)((1024/100)*x)

#define GPIO_PWM_1 18

void gpio_init(void);

#endif /* GPIO_GPIO_H_ */
