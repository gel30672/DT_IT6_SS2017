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
#include "Vector.h"

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
	unsigned long getTimeStamp();
	void fillLaserArray(char string[]);
	void getLaserData();
	bool calculateObstaclePosition(double degree, int distance, short *x, short *y, double carRotation);
	double calculateCarRotation(Vector* currentDir);
	char* readFile();
	bool obstacleLeft;	//Hindernis zw. -45� & -15� 
	bool obstacleRight;	//Hindernis zw. 15� & 45� 
	bool obstacleFront;	//Hindernis zw. -15� & 15�
	int _return;
	unsigned long ms_callEnded;
	char* buffer;
	void UpdateMapWithLaserData(Vector *currentDir);
	Map* map;
	int* nearObstacle;

public:
	LaserSensor(Map* _map, int* _nearObstacle);
	int doLaserScanAndMapUpdate(Vector *currentDir);
};

#endif /* INCLUDE_LASERSENSOR_H_ */
