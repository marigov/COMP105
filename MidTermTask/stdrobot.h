#include "simpletools.h"
#include "abdrive.h"
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#define ROBOT_WIDTH 10.58
#define WHEEL_DIAM 20.8 

#define WHEEL_BASE 105.8
#define TICK_LENGTH 3.25
#define WHEEL_DIAMETRE 66.2
#define ENCODER_CLICKS_PER_TURN 64


typedef struct node {
    double val;
    double angle;
    struct node * next;
} node_correction;


// Driving

void turnSpecificAngle(double angle);
void turn(int angle);
void spinRad(int n, int r);
int cmToTicks(double cm);
double degToRad(double deg); 


// Linked list

void push(node_correction ** head, double val, double angle);


// Logging

double* getDistanceWheels();
double getAngleChange(double* distanceWheels);
double* getCoordinates(double* distanceWheels, double currentAngle);
double distance_travelled(double* positionCoordinates);
void calcDispalcement(double* distanceWheels, double* lastAngle, double* dx, double* dy);



