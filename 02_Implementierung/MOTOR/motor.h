#ifndef MOTOR_H_
#define MOTOR_H_

#define CMD_FORWARD 0x85
#define CMD_REVERSE 0x86
#define CMD_BRAKE 0x92
#define CMD_INIT 0x83

#define FORWARD_DIR 0
#define BACKWARD_DIR 1
#define BRAKE_DIR 2

#include "../PID_CONTROLLER/pid.h"
#include "../PROJECT_ADAPTER/project_adapter.h"

typedef enum { RAMP, FORWARD, BACKWARDS, BRAKE } e_vehicle_state;
e_vehicle_state e_driving_state;

extern pid_param_t motor_pid;

//uart file descriptor
int uart;

int direction;

e_state setSpeed(signed char speed);
int motorInit();
e_state move(int rpm);
e_state moveKph(float speed);
e_state moveMps(float speed);

#endif
