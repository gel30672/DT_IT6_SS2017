//
// Created by Andreas Zinkl on 09.05.17.
//

#ifndef DT2017_DEVICECONFIGURATION_H
#define DT2017_DEVICECONFIGURATION_H

#include <math.h>

// Define the Destination coordinates
#define DESTINATION_X_COORDINATE 2
#define DESTINATION_Y_COORDINATE 0

// The wheelbase of the vehicle is 27.5 cm
#define WHEELBASE 27.5

// The maximum wheel lock is by 20°
#define MAXWHEELLOCK 20

// Calculation of the turnaround circle
#define CIRCLERADIUS WHEELBASE/sin(MAXWHEELLOCK)
#define CALC_DISTANCE_BY_ANGLE(x) (2*M_PI*CIRCLERADIUS*(x/360))
#define CALC_DROVEN_ANGLE_BY_LENGTH(x) ((x*360)/(2*M_PI*CIRCLERADIUS))

// The droven distance for the configuration and the general distance
#define INIT_CONFIG_DISTANCE 30
#define DROVENDISTANCE 0 //this will later contain an method call

// The device driving directions
#define DIRECTION_BACKWARD -1
#define DIRECTION_FORWARD 0
#define DIRECTION_LEFT 1
#define DIRECTION_RIGHT 2
#define DIRECTION_STOP 3

// Device specific configuration
#define FULLSPEED 2
#define BACKSPEED (-FULLSPEED)
#define WHEEL_ANGLE(x) (x < 0 ? (-MAXWHEELLOCK) : MAXWHEELLOCK)



//TODO THIS MAY NEED TO BE REMOVED BY REAL FUNCTION CALLS
//TODO THIS IS JUST FOR LATER MERGE PURPOSE
//This defines the needed changes or calls to functions in other modules
#define GET_CURRENT_DRIVEN_DISTANCE 10.0

#endif //ALGORITHMS_CPP_DEVICECONFIGURATION_H
