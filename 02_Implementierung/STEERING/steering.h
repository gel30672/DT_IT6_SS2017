/*
 * Steering.h
 *
 *  Created on: 14.04.2017
 *      Author: pwina
 */

#ifndef STEERING_H_
#define STEERING_H_

#include "../PROJECT_ADAPTER/project_adapter.h"

#define STEERING_STRAIGHT 81 	//0 degrees = 81/1000 PWM DC
#define HW_PWM_CLOCK 350
#define HW_PWM_RANGE 1000

void HwPwmSteeringInit();
e_state SetDegrees(int degree);

#endif /* STEERING_H_ */
