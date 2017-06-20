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
#include "../include/Map.h"
#include "../include/LaserSensor.h"
#include "../include/PythonExecuter.h"

LaserSensor::LaserSensor(Map* _map, Car* _car, int* _nearObstacle)
{
	obstacleLeft = 0;	//Hindernis zw. -45° & -15° 
	obstacleRight = 0;	//Hindernis zw. 15° & 45° 
	obstacleFront = 0;	//Hindernis zw. -15° & 15° 
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
	buffer = nullptr;
	map = _map;
	car = _car;
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
				
				if(laserArray[1][row - 1] < 300)
				{
					if(laserArray[0][row - 1] > -45 && laserArray[0][row - 1] < -15 && !obstacleLeft) //Hindernis links!
					{
						obstacleLeft = true;
					}
					else if(laserArray[0][row - 1] >= -15 && laserArray[0][row - 1] <= 15 && !obstacleFront) //Hindernis vorne!
					{
						obstacleFront = true;
					}
					else if(laserArray[0][row - 1] > 15 && laserArray[0][row - 1] < 45 && !obstacleRight) //Hindernis vorne!
					{
						obstacleRight = true;
					}
				}
				
				
			}
			counter++;
		}
		else								//zwei Werte überspringen
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
	char * argv[3];

	argc = 3;
	argv[0] = "a";
	argv[1] = "laser";
	argv[2] = "laser";

	py->executeWithCharPointer(argc, argv, &buffer);
	//printf("ende getLaserData");
}

bool LaserSensor::calculateObstaclePosition(double degree, int distance, short *x, short *y)
{

	//printf("in Calc: \n");
	//printf("degree: %f, distance: %d\n",degree, distance);

	if(distance > 100 ) 											// muss größer 0 sein, da kein Hindernis = 0, 100 wegen Größe des Fahrzeugs
	{
		double a = degree * Pi / 180; //Grad in Rad umrechnen

		//printf("%f\n",a);
		//printf("sinus: %f\n", sin(a));
		//printf("cosinus: %f\n", cos(a));

		*x = (int)(sin(a) * distance)/(MapRasterWidth_cm * 10); 	//mit Satz des Pythagoras x
		*y = (int)(cos(a) * distance)/(MapRasterWidth_cm * 10);		//und y berechnen

		//printf("x: %d\n", *x);
		//printf("y: %d\n", *y);

		if(degree < -90) 		//left and down
		{

		}
		else if(degree < 0) 	//left and up
		{
			*y = *y * -1;
		}
		else if(degree <= 90) 	//right and up
		{}
		else if(degree >=90)	//right and down
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

void LaserSensor::UpdateMapWithLaserData()
{
	//std::cout << "in upadte map" << std::endl;
	short xDiffToCurrentPos, yDiffToCurrentPos;

	//if(car == nullptr) std::cout << "NULLPOINTER!!!!!!" << std::endl;
	Position *currentPosition = car->getCurrentPosition(); // Hier Map-Objekt angeben!
	int i;
	for(i = 0; i<CountArrayElements;i++) 	//für jedes Arrayelementepaar berechenen ob und in welchen Koordinaten ein Hindernis ist
	{
		if(laserArray[0][i] == -1)
			break;
		xDiffToCurrentPos = 0, yDiffToCurrentPos = 0;
		bool IsNotFree = calculateObstaclePosition(laserArray[0][i],(int)laserArray[1][i], &xDiffToCurrentPos, &yDiffToCurrentPos);
		//printf("degree: %f; distance: %d; xDiff: %d; yDiff: %d\n",laserArray[0][i],(int)laserArray[1][i], xDiffToCurrentPos, yDiffToCurrentPos);
		if(IsNotFree )
		{
			map->updateField(currentPosition->x + xDiffToCurrentPos + 1, currentPosition->y + yDiffToCurrentPos, isObstacle);
			map->updateField(currentPosition->x + xDiffToCurrentPos, currentPosition->y + yDiffToCurrentPos, isObstacle);
			map->updateField(currentPosition->x + xDiffToCurrentPos - 1, currentPosition->y + yDiffToCurrentPos, isObstacle);
		}
		else
		{
			map->updateField(currentPosition->x + xDiffToCurrentPos, currentPosition->y + yDiffToCurrentPos, isFree);
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

int LaserSensor::doLaserScanAndMapUpdate()
{
	getLaserData();
	//printf("buffer gefuellt!\n");
	if(buffer == nullptr)
		printf("NULL FUUUUUUUU\n");
	//printf("Buffer %c\n",buffer);
	fillLaserArray(buffer);
	//printf("laserarray gefuellt!\n");
	UpdateMapWithLaserData();

	//Testing purpose
	/*calculateObstaclePosition(-110,1500, &xDiffToCurrentPos, &yDiffToCurrentPos);
	printf("Erwartet: 		x = -9, y = -3\n\n");
	calculateObstaclePosition(-90,1500, &xDiffToCurrentPos, &yDiffToCurrentPos);
	printf("Erwartet: 		x = -10, y = 0\n\n");
	calculateObstaclePosition(-45,1500, &xDiffToCurrentPos, &yDiffToCurrentPos);
	printf("Erwartet: 		x = -7, y = -7\n\n");
	calculateObstaclePosition(0,1500, &xDiffToCurrentPos, &yDiffToCurrentPos);
	printf("Erwartet: 		x = 0, y = 10\n\n");
	calculateObstaclePosition(45,1500, &xDiffToCurrentPos, &yDiffToCurrentPos);
	printf("Erwartet: 		x = 7, y = 7\n\n");
	calculateObstaclePosition(90,1500, &xDiffToCurrentPos, &yDiffToCurrentPos);
	printf("Erwartet: 		x = 10, y = 0\n\n");
	calculateObstaclePosition(110,1500, &xDiffToCurrentPos, &yDiffToCurrentPos);
	printf("Erwartet: 		x = 9, y = 3\n");*/
	int _return = 0;
	_return = obstacleLeft * 4 | obstacleFront * 2 | obstacleRight;
	obstacleLeft = 0;	
	obstacleRight = 0;
	obstacleFront = 0;
	*nearObstacle = _return; 
	return _return;
}
