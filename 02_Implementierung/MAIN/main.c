#include <stdio.h>
#include <wiringPi.h>

#include "../GPIO/gpio.h"

const int pwmValue = 25; // Use this to set an LED brightness

void main_init()
{
	gpio_init();
}

int main(void)
{
	main_init();
    printf("Press CTRL+C to quit.\n");

    while(1)
    {
    	pwmWrite(GPIO_PWM_1, pwmValue); // PWM LED at bright setting
    }

    return 0;
}
