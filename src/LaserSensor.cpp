/*
 ============================================================================
 Name        : LaserSensor.c
 Author      : Lukas Geck
 Version     : 1.0
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../include/LaserSensor.h"
#include <sys/time.h>

LaserSensor::LaserSensor(Map* _map, int* _nearObstacle)
{
	obstacleLeft = 0;	//Hindernis zw. -45? & -15?
	obstacleRight = 0;	//Hindernis zw. 15? & 45?
	obstacleFront = 0;	//Hindernis zw. -15? & 15?
	nearObstacle = _nearObstacle;
	int i;
	for(i = 0; i<CountArrayElements; i++)
	{
		laserArray[0][i] = -1;
		laserArray[1][i] = -1;
	}
	row = 0;
	ArrayPosition = 0;
	counter = 0;
	buffer = new char[BUFFERSIZE];
	map = _map;
	ms_callEnded = 0;
	_return = 0;
}

void LaserSensor::fillLaserArray(char string[])
{
	int i;
	for(i = 0; i<CountArrayElements; i++)
	{
		laserArray[0][i] = -1;
		laserArray[1][i] = -1;
	}


	char delimiter[] = ",{}:";
	char *ptr;

	// initialisieren und ersten Abschnitt erstellen
	ptr = strtok(string, delimiter);

	while(ptr != NULL) {

		if(counter == 0 || counter == 1) 	//nur jeden dritten Wert beachten
		{
			//printf("Abschnitt gefunden: %s\n", ptr);
			if(ArrayPosition == 0) 			//Winkel in Array eintragen
			{
				laserArray[0][row] = strtod(ptr, NULL);
				ArrayPosition = 1;
			}
			else							//Entfernung in Array eintragen
			{
				laserArray[1][row] = strtod(ptr, NULL);
				row++;
				ArrayPosition = 0;
				
				if(laserArray[1][row - 1] < 600 && laserArray[1][row - 1] > 100)
				{
					if(laserArray[0][row - 1] > -45 && laserArray[0][row - 1] < -15 && !obstacleLeft) //Hindernis links!
					{
						obstacleLeft = 1;
					}
					else if(laserArray[0][row - 1] >= -15 && laserArray[0][row - 1] <= 15 && !obstacleFront) //Hindernis vorne!
					{
						obstacleFront = 1;
					}
					else if(laserArray[0][row - 1] > 15 && laserArray[0][row - 1] < 45 && !obstacleRight) //Hindernis vorne!
					{
						obstacleRight = 1;
					}
				}
			}
			counter++;
		}
		else								//zwei Werte ?berspringen
		{
			counter++;
			if(counter == 6)
			{
				counter = 0;
			}
		}
		// naechsten Abschnitt erstellen
		ptr = strtok(NULL, delimiter);
	}
}

void LaserSensor::getLaserData()
{
	//printf("im get Laser data\n");
	PythonExecuter* py = new PythonExecuter;

	int argc;
	char* argv[3];

	argc = 3;
	argv[1] = (char*) "laser";
	argv[2] = (char*) "laser_scan";

	buffer = new char [BUFFERSIZE];
	py->executeWithCharPointer(argc, argv, &buffer);
	//printf("ende getLaserData");
}

double LaserSensor::calculateCarRotation(Vector* currentDir)
{
	Position centrum = {0,0};
	Position yValueHigh = {0,1};
	Vector *yAxis = new Vector(yValueHigh,centrum);
	double carRotation = currentDir->getAngleTo(yAxis);
	if(currentDir->getX() < 0) //Arccos kann nur positive Werte zwischen 0° und 180° berechnen
	{
		carRotation = carRotation * -1;
	}
	std::cout << "######## ########## 	########### 	######## 	winkel: " << carRotation << std::endl;

	return carRotation;
}

bool LaserSensor::calculateObstaclePosition(double degree, int distance, short *x, short *y, double carRotation)
{
	//printf("in Calc: \n");
	//printf("degree: %f, distance: %d\n",degree, distance);

	if(distance > 100 ) 											// muss groesser 0 sein, da kein Hindernis = 0, 100 wegen Grï¿½ï¿½e des Fahrzeugs
	{
		double a = (degree + carRotation) * Pi / 180; //Grad in Rad umrechnen

		//printf("%f\n",a);
		//printf("sinus: %f\n", sin(a));
		//printf("cosinus: %f\n", cos(a));

		*x = (int)(sin(a) * distance)/(MapRasterWidth_cm * 10); 	//mit Satz des Pythagoras x
		*y = (int)(cos(a) * distance)/(MapRasterWidth_cm * 10);		//und y berechnen

		//printf("x: %d\n", *x);
		//printf("y: %d\n", *y);

		if(degree + carRotation < -90) 		//left and down
		{

		}
		else if(degree + carRotation < 0) 	//left and up
		{
			*y = *y * -1;
		}
		else if(degree + carRotation <= 90) 	//right and up
		{}
		else if(degree + carRotation >=90)	//right and down
		{
			*y = *y * -1;
		}

		//printf("nach Spiegelung: 	x = %d, y = %d\n", *x, *y);

		return true;
	}
	else //Kein Hindernis
	{
		return false;
	}
}

void LaserSensor::UpdateMapWithLaserData(Vector *currentDir)
{
	//std::cout << "in upadte map" << std::endl;
	short xDiffToCurrentPos, yDiffToCurrentPos;

	//Testing purpose
	/*calculateObstaclePosition(-110,1500, &xDiffToCurrentPos, &yDiffToCurrentPos , 110);
	std::cout << "x: " << xDiffToCurrentPos << " y: " << yDiffToCurrentPos << std::endl;
	printf("Erwartet: 		x = -9, y = -3\n\n");
	calculateObstaclePosition(-90,1500, &xDiffToCurrentPos, &yDiffToCurrentPos , 90);
	std::cout << "x: " << xDiffToCurrentPos << " y: " << yDiffToCurrentPos << std::endl;
	printf("Erwartet: 		x = -10, y = 0\n\n");
	calculateObstaclePosition(-45,1500, &xDiffToCurrentPos, &yDiffToCurrentPos , 45);
	std::cout << "x: " << xDiffToCurrentPos << " y: " << yDiffToCurrentPos << std::endl;
	printf("Erwartet: 		x = -7, y = -7\n\n");
	calculateObstaclePosition(0,1500, &xDiffToCurrentPos, &yDiffToCurrentPos , 0);
	std::cout << "x: " << xDiffToCurrentPos << " y: " << yDiffToCurrentPos << std::endl;
	printf("Erwartet: 		x = 0, y = 10\n\n");
	calculateObstaclePosition(45,1500, &xDiffToCurrentPos, &yDiffToCurrentPos , -45);
	std::cout << "x: " << xDiffToCurrentPos << " y: " << yDiffToCurrentPos << std::endl;
	printf("Erwartet: 		x = 7, y = 7\n\n");
	calculateObstaclePosition(90,1500, &xDiffToCurrentPos, &yDiffToCurrentPos , -90);
	std::cout << "x: " << xDiffToCurrentPos << " y: " << yDiffToCurrentPos << std::endl;
	printf("Erwartet: 		x = 10, y = 0\n\n");
	calculateObstaclePosition(110,1500, &xDiffToCurrentPos, &yDiffToCurrentPos , -110);
	std::cout << "x: " << xDiffToCurrentPos << " y: " << yDiffToCurrentPos << std::endl;
	printf("Erwartet: 		x = 9, y = 3\n");*/

	//if(car == nullptr) std::cout << "NULLPOINTER!!!!!!" << std::endl;

	//calculate Car Rotation for obstacle positions
	double carRotation = calculateCarRotation(currentDir);

	int i;
	Position* _currentPos = currentDir->getFoot();
	for(i = 0; i<CountArrayElements;i++) 	//f?r jedes Arrayelementepaar berechenen ob und in welchen Koordinaten ein Hindernis ist
	{
		if(laserArray[0][i] == -1)
			break;
		xDiffToCurrentPos = 0, yDiffToCurrentPos = 0;
		bool IsNotFree = calculateObstaclePosition(laserArray[0][i],(int)laserArray[1][i], &xDiffToCurrentPos, &yDiffToCurrentPos, carRotation);
		//printf("degree: %f; distance: %d; xDiff: %d; yDiff: %d\n",laserArray[0][i],(int)laserArray[1][i], xDiffToCurrentPos, yDiffToCurrentPos);
		if(IsNotFree )
		{
			//std::cout << "hindernis wird in map geschrieben" << std::endl;
			//std::cout << "current x: " << _currentPos->x << "current y: " << _currentPos->y << std::endl;
			//std::cout << "hindernis wird geschrieben auf position: " << _currentPos->x + xDiffToCurrentPos << " | " <<  _currentPos->y + yDiffToCurrentPos <<std::endl;
			map->updateField(_currentPos->x + xDiffToCurrentPos + 1, _currentPos->y + yDiffToCurrentPos, isObstacle);
			map->updateField(_currentPos->x + xDiffToCurrentPos, _currentPos->y + yDiffToCurrentPos, isObstacle);
			map->updateField(_currentPos->x + xDiffToCurrentPos - 1, _currentPos->y + yDiffToCurrentPos, isObstacle);
		}
		else
		{
			map->updateField(_currentPos->x + xDiffToCurrentPos, _currentPos->y + yDiffToCurrentPos, isFree);
		}

	}
}

char* LaserSensor::readFile()
{
	char *buffer;
	FILE *fp;
	long lSize;

	fp = fopen ( "Laser.txt" , "rb" );
	if( !fp ) perror("Laser.txt"),exit(1);

	fseek( fp , 0L , SEEK_END);
	lSize = ftell( fp );
	rewind( fp );

	// Speicherallokation
	buffer = (char*) calloc( 1, lSize+1 );
	if( !buffer ) fclose(fp),fputs("memory alloc fails",stderr),exit(1);

	// Datei in Puffer kopieren
	if( 1!=fread( buffer , lSize, 1 , fp) )
		fclose(fp),free(buffer),fputs("entire read fails",stderr),exit(1);

	return buffer;
}

unsigned long LaserSensor::getTimeStamp()
{
	unsigned long ms = 0;
	struct timeval ms_struct;
	gettimeofday(&ms_struct, NULL);
	ms = ms_struct.tv_sec * 1000 + ms_struct.tv_usec / 1000;
	return ms;
}

int LaserSensor::doLaserScanAndMapUpdate(Vector* currentDir) {

	//return 0;
	unsigned long ms_call = getTimeStamp();
	long long timediff = ms_call - ms_callEnded;
	if(timediff < 200) {
		return _return;
	}
	getLaserData();

	if(buffer != nullptr) {
		fillLaserArray(buffer);
		//printf("laserarray gefuellt!\n");
		//UpdateMapWithLaserData(currentDir);
	}

    //std::cout << obstacleLeft << "|" << obstacleFront << "|" << obstacleRight << std::endl;

	_return = 0;
	if(map->isObstacleInRoute())
	{
		_return |= NEW_ROUTE_NEEDED;
	}
	_return = _return |obstacleLeft * 4 | obstacleFront * 2 | obstacleRight;
	obstacleLeft = 0;
	obstacleRight = 0;
	obstacleFront = 0;
	*nearObstacle = _return;

	ms_callEnded = getTimeStamp();
	std::cout << " ####### 			########		######## ende Lasersensor return: " << _return << std::endl;
	return _return;
}
