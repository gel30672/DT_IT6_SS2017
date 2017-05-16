/*
 * pid.c
 *
 *  Created on: 01.04.2017
 *      Author: andreas
 */

#include "pid.h"

void calc_pid(pid_param_t* pid)
{
	int err = pid->actual_setpoint - pid->in;

	/* p */
	double p = err * pid->kp;

	/* i */
	pid->integral += err;
	double i = pid->integral * pid->ki;

	/* d */
	double d = pid->kd * (err - pid->old_err);

	/* calc pid */
	pid->out = (int)(p + i + d);

	/* output limitation */
	if(pid->out > pid->out_max)
		pid->out = pid->out_max;
	else if(pid->out < pid->out_min)
		pid->out = pid->out_min;

	pid->old_err = err;
}

void reset_pid(pid_param_t* pid)
{
	pid->integral = 0;
	pid->old_err = 0;
	pid->actual_setpoint = 0;
	pid->out = 0;
}
