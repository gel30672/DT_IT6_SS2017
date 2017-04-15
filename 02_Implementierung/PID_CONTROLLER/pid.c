/*
 * pid.c
 *
 *  Created on: 01.04.2017
 *      Author: andreas
 */

#include "pid.h"

void calc_pid(pid_param_t* pid)
{
	int err = pid->setpoint - pid->in;

	/* p */
	int p = err * pid->kp;

	/* i */
	pid->integral += err;
	int i = pid->integral * pid->ki;

	/* d */
	int d = pid->kd * (err - pid->old_err);

	/* calc pid */
	pid->out = p * i * d;

	/* output limitation */
	if(pid->out > pid->out_max)
		pid->out = pid->out_max;
	else if(pid->out < pid->out_min)
		pid->out = pid->out_min;

	pid->old_err = err;
}
