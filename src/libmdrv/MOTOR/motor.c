#include <wiringSerial.h>
#include "motor.h"
#include <stdio.h>

/* global motor pid struct */
pid_param_t motor_pid;
e_vehicle_state e_driving_state;
int uart;
int direction;
e_state setSpeed(signed char speed)
{
	if(speed > 100 || speed < -100) return E_NOT_OK;
	else
	{
		if(speed < 0)
		{
			speed = speed*-1;
			speed |= 128;
		}
		serialPutchar(uart, speed);
		return E_OK;
	}
}

int motorInit()
{
	uart = serialOpen("/dev/ttyAMA0", 9600);
	printf("%d",uart);
	return uart;
}

e_state MotorMoveRpm(int rpm)
{
	if(rpm > 4700) return E_NOT_OK;
	else
	{
		motor_pid.setpoint = rpm;
		return E_OK;
	}
}

e_state MotorMoveKph(float speed)
{
	if(speed > 19) return E_NOT_OK;
	else
	{
		return MotorMoveRpm(speed*25/6);
	}
}
e_state MotorMoveMps(float speed)
{
	if(speed > 5.27) return E_NOT_OK;
	else
	{
		return MotorMoveRpm(speed*15);
	}
}
