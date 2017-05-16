/*
 * pid.h
 *
 *  Created on: 01.04.2017
 *      Author: andreas
 */

#ifndef PID_H_
#define PID_H_

typedef struct
{
	double kp;
	double ki;
	int integral;
	double kd;
	int old_err;
	int setpoint;
	int actual_setpoint;
	int in;
	int out;
	int out_min;
	int out_max;
} pid_param_t;

void calc_pid(pid_param_t* pid);
void reset_pid(pid_param_t* pid);

#endif /* PID_H_ */
