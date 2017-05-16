#include <wiringSerial.h>
#include "motor.h"
#include <stdio.h>

/* global motor pid struct */
pid_param_t motor_pid;

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

		printf("%d", (char) speed);

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

e_state move(int rpm)
{
	//if(rpm > 80) return E_NOT_OK;
	//else
	//{
		motor_pid.setpoint = rpm;
		return E_OK;
	//}
}

e_state moveKph(float speed)
{
	if(speed > 19) return E_NOT_OK;
	else
	{
		move(speed*25/6);
		return E_OK;
	}
}
e_state moveMps(float speed)
{
	if(speed > 5.27) return E_NOT_OK;
	else
	{
		move(speed*15);
		return E_OK;
	}
}
