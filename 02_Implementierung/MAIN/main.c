#include <stdio.h>
#include <wiringPi.h>

#include "../GPIO/gpio.h"
#include "../PID_CONTROLLER/pid.h"
#include "../STEERING/steering.h"

pid_param_t motor_pid;

void main_init()
{
	gpio_init();

	/* motor pid params */
	motor_pid.kp = 0.5;
	motor_pid.ki = 0;
	motor_pid.kd = 0;

	motor_pid.out_max = 1000; /* rpm */
	motor_pid.out_min = 0;

	/* only testing ! */
	motor_pid.setpoint = 800; /* rpm */

	HwPwmSteeringInit();
}

int main(void)
{
	main_init();
    printf("Press CTRL+C to quit.\n");

    SetDegrees(-21);
    puts("-21");
    delay(5000);

    SetDegrees(21);
    puts("21");
    delay(5000);

    SetDegrees(0);
    puts("0");
    delay(5000);

    SetDegrees(25);
    puts("24");
    delay(5000);

    SetDegrees(-25);
    puts("-24");

    return 0;
}
