#include <stdio.h>
#include <wiringPi.h>
#include <wiringSerial.h>

#include <errno.h>

#include "../GPIO/gpio.h"
#include "../PID_CONTROLLER/pid.h"
#include "../STEERING/steering.h"
#include "../SENSING/current_sensing.h"
#include "../MOTOR/motor.h"

int shaft_rpm;
unsigned int shaft_rotations;
int last_input;

void main_init()
{
	gpio_init();
	distanceSinceStart = 0;

	/* motor pid params */
	motor_pid.kp = 0.013;//0.009
	motor_pid.ki = 0.002;//0.001
	motor_pid.kd = 0.0008;//0.0006

	motor_pid.out_max = 100; //~70% of maximum
	motor_pid.out_min = -100;

	/* init hardware pwm for steering servo */
	HwPwmSteeringInit();

	/* init motor */
	motorInit();

	e_driving_state = RAMP;
}

void vehicle_state_machine()
{
	switch(e_driving_state)
	{
	case (RAMP):
		if(motor_pid.setpoint < 0)
		{
			if(motor_pid.actual_setpoint > motor_pid.setpoint)
				motor_pid.actual_setpoint += (motor_pid.setpoint / 1);
			else
				e_driving_state = BACKWARDS;
		}
		else if(motor_pid.setpoint > 0)
		{
			if(motor_pid.actual_setpoint < motor_pid.setpoint)
				motor_pid.actual_setpoint += (motor_pid.setpoint / 1);
			else
				e_driving_state = FORWARD;
		}
		else if(motor_pid.setpoint == 0)
			e_driving_state = BRAKE;

		setSpeed(motor_pid.out);

	break;
	case (FORWARD):
		if(motor_pid.setpoint <= 0)
		{
			e_driving_state = BRAKE;
			break;
		}
		setSpeed(motor_pid.out);

	break;
	case (BACKWARDS):
		if(motor_pid.setpoint >= 0)
		{
			e_driving_state = BRAKE;
			break;
		}
		setSpeed(motor_pid.out);

	break;
	case (BRAKE):
		/* brake until vehicle stops */
		if(motor_pid.in == 0 && motor_pid.setpoint != 0)
			e_driving_state = RAMP;
		else
		{
			reset_pid(&motor_pid);
			setSpeed(-5);
		}
	break;
	}
}

void task_100_ms()
{

	/* rotations / 2700ms */
	shaft_rotations = serialGetchar(uart);
	/*/2.7 /10 == ration/100ms; *6.666 == ration of wheel */
	/*in cm*/
	distanceSinceStart += ((shaft_rotations/2.7) /10) * 6.735; //6.83
	serialFlush(uart);

	/* average filter shaft_rpm for pid controller input */
	last_input = shaft_rpm;
	shaft_rpm = (int)((double)(shaft_rotations * (double)(60/2.7)));
	shaft_rpm = (shaft_rpm + last_input) / 2;

	//@TODO get direction from rotary sensor
	/* works for now */
	if(motor_pid.setpoint < 0)
		motor_pid.in = shaft_rpm * -1;
	else
		motor_pid.in = shaft_rpm;

	/* set states of vehicle */
	vehicle_state_machine();

	/* pid controller rpm */
	calc_pid(&motor_pid);

	printf("out: %d, state: %d, sp: %d, rpm %d, dist: %lf, err: %d, rot: %d \n", motor_pid.out,
			e_driving_state, motor_pid.actual_setpoint, motor_pid.in, distanceSinceStart, motor_pid.old_err, shaft_rotations);
}

int main(void)
{
	main_init();

    int old_time, time;
    while(1)
    {
    	time = millis();
    	if(time >= old_time + 100)
    	{
        	old_time = time;
        	task_100_ms();
    	}
    }

    return 0;
}
