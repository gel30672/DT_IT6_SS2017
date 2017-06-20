#include <stdio.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <pthread.h>

#include <errno.h>
#include "../../../include/Vector.h"
#include "../../../include/CarSystem.h"
#include "../../../include/CarCommandTestInterface.h"

extern "C"{
#include "../GPIO/gpio.h"
#include "../SENSING/current_sensing.h"
}

int shaft_rpm;
unsigned int shaft_rotations;
int last_input;

void main_init()
{
	std::cout << "TEST 1" << std::endl;
	gpio_init();
	std::cout << "TEST 2" << std::endl;
	distanceSinceStart = 0;

	/* Wmotor pid params */
	motor_pid.kp = 0.013;//0.009
	motor_pid.ki = 0.002;//0.001
	motor_pid.kd = 0.0008;//0.0006

	motor_pid.out_max = 70; //~70% of maximum
	motor_pid.out_min = -70;

	/* init hardware pwm for steering servo */
	std::cout << "TEST 3" << std::endl;
	HwPwmSteeringInit();
	std::cout << "TEST 4" << std::endl;

	/* init motor */
	std::cout << "TEST 5" << std::endl;
	motorInit();
	std::cout << "TEST 6" << std::endl;

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

	//printf("out: %d, state: %d, sp: %d, rpm %d, dist: %lf, err: %d, rot: %d \n", motor_pid.out,
	//		e_driving_state, motor_pid.actual_setpoint, motor_pid.in, distanceSinceStart, motor_pid.old_err, shaft_rotations);
}

void *testCommands(void*) {

	CarCommandTestInterface* testing  = new CarCommandTestInterface();
	testing->runTest();

}

void *callRouteCalculation(void *) {

	//Route Driver Initialization
	CarSystem* carSystem = new CarSystem();

    // Error code history
    short lastErrorCode = SUCCESS;

	// We drove the init track
	if(PRINT_ERROR_CODE) std::cout << "MAIN - CALL INIT CAR SYSTEM START" << std::endl;
	short errCode = carSystem->initialize();
    if(PRINT_ERROR_CODE) std::cout << "MAIN - CALL INIT CAR SYSTEM (" << errCode << ")" << std::endl;

	// Now we can check continuously our drive
	std::cout << "CARSYSTEM: RUN" << std::endl;
	while(1) {
		short status = carSystem->run();
		if (!carSystem->checkSensor()){
			carSystem->emergencystop(true);
			continue;

		}
		if(PRINT_ERROR_CODE && status != lastErrorCode) {
			std::cout << "checkdrive(" << status << ")" << std::endl;
			lastErrorCode = status;
		}
	}
}

void *car(void*) {

	// Run the car
	int old_time = 0;
	int time = 0;
	while(1) {

		time = millis();
		if(time >= old_time + 100)
		{
			old_time = time;
			task_100_ms();
		}
	}
}

int main() {
    short threadCount = 2;
    pthread_t threads[threadCount];
    int rc_1;
    int rc_2;


	std::cout << "TEST XX" << std::endl;
    main_init();

	std::cout << "Using Testsystem? (y/n) " << std::endl;
	bool useTestSystem = false;
	string val;
	std::cin >> val;

	if(val == "y" || val == "Y") useTestSystem = true;

	if(useTestSystem) {
		rc_1 = pthread_create(&threads[0], NULL, testCommands, (void*) NULL);
	} else {
		rc_1 = pthread_create(&threads[0], NULL, callRouteCalculation, (void *) NULL);
	}
    rc_2 = pthread_create(&threads[1], NULL, car, (void *) NULL);

    if (rc_1){
        cout << "Error:unable to create thread route," << rc_1 << endl;
        exit(-1);
    }
    if (rc_2){
        cout << "Error:unable to create thread car," << rc_2 << endl;
        exit(-1);
    }

    pthread_exit(NULL);
}
