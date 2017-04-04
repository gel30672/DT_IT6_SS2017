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
	double kc;
	double kd;
	int out;
	int in;
} pid_param_t;

void calc_pid(pid_param_t* pid);

#endif /* PID_H_ */
