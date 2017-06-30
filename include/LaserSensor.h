/*
 * LaserSensor.h
 *
 *  Created on: 21.05.2017
 *      Author: lukas
 */

#ifndef INCLUDE_LASERSENSOR_H_
#define INCLUDE_LASERSENSOR_H_

#include "Map.h"
#include "PythonExecuter.h"
#include "ErrorHandling.h"

#define isObstacle 1
#define isFree 0
#define CountArrayElements 300
#define Pi 3.141592653

class LaserSensor
{
private:
	double laserArray[2][300]; //Position 0: x = Winkel, Position 1: x Entfernung in mm
	int row;
	int ArrayPosition;
	int counter;
	Position* _currentPos;
	unsigned long getTimeStamp();
	//static constexpr double Pi = 3.141592653;

public:
	//LaserSensor(Map* _map, Car* _car, int* _nearObstacle);
	LaserSensor(Map* _map, int* _nearObstacle);
	int doLaserScanAndMapUpdate(Position *currentPos);
	char* buffer;
	void fillLaserArray(char string[]);
	void getLaserData();
	bool calculateObstaclePosition(double degree, int distance, short *x, short *y);
	void UpdateMapWithLaserData();
	char* readFile();
	Map* map;
	//Car* car;
	bool obstacleLeft;	//Hindernis zw. -45� & -15� 
	bool obstacleRight;	//Hindernis zw. 15� & 45� 
	bool obstacleFront;	//Hindernis zw. -15� & 15�
	int* nearObstacle;
	unsigned long ms_callEnded;
	int _return;
};

#endif /* INCLUDE_LASERSENSOR_H_ */
