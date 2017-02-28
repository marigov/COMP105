#include "stdrobot.h"


double* getDistanceWheels(){
  int* ticksList = malloc(2*sizeof(int));
  drive_getTicks(ticksList, (ticksList+1));
  double* distanceWheels = malloc(2*sizeof(double));
  *distanceWheels = ((double)*ticksList/ENCODER_CLICKS_PER_TURN)*PI*WHEEL_DIAMETRE;
  *(distanceWheels+1) = ((double)*(ticksList+1)/ENCODER_CLICKS_PER_TURN)*PI*WHEEL_DIAMETRE;
  free(ticksList);
  return distanceWheels;
}

double getAngleChange(double* distanceWheels){
  return (*distanceWheels - (*(distanceWheels+1)))/ WHEEL_BASE;
}


void calcDispalcement(double* distanceWheels, double* lastAngle, double* dx, double* dy) {
    double angle = getAngleChange(distanceWheels);
    printf("Angle: %f \n", angle);
    printf("Last angle: %f \n", *lastAngle);
    printf("dx: %f \n", *dx);
    printf("dy: %f \n", *dy);
    printf("Distance: L: %f R: %f \n",*(distanceWheels) ,*(distanceWheels+1));
    fflush(stdout);
 
    double rl = *(distanceWheels) / angle;
    double rr = *(distanceWheels + 1) / angle;
    double rm = (rl + rr) / 2;

    if (*lastAngle == 0) {
        *dx = rm * (1 - cos(angle));
        *dy = rm * sin(angle);
    }
    else {
        *dy = rm * (sin(*lastAngle + angle)) - sin(*lastAngle);
        *dx = rm * (cos(*lastAngle + angle)) - cos(*lastAngle);
    }
    *lastAngle = angle;
}

double distance_travelled(double* positionCoordinates) {
  return sqrt(pow(*positionCoordinates, 2) + pow(*(positionCoordinates + 1), 2));
}

int cmToTicks(double cm) {
  return (cm/((WHEEL_DIAM/64)));
}

double degToRad(double deg) {
  return (deg * PI / 180);
} 

void turnSpecificAngle(double angle) {
  int distanceDrive = angle * cmToTicks((ROBOT_WIDTH / 2));
  drive_goto(-(distanceDrive),(distanceDrive));
}

void turn(int angle) {

  switch (angle) {
    case 180:
      drive_goto(-52,51);
      break;
    case -180:
      drive_goto(52, -51);
      break;
    case 90:
      drive_goto(-26,25);
      break;
    case -90: 
      drive_goto(26,-25);
      break;
    case 360:
      drive_goto(-103, 102);
      break;
    case -360: 
      drive_goto(103, -102);
      break;
    default:
      turnSpecificAngle(angle);
      break;
    }
}

void push(node_correction ** head, double val, double angle) {
    node_correction * new_node;
    new_node = malloc(sizeof(node_correction));

    new_node->val = val;
    new_node->angle = angle;
    new_node->next = *head;
    *head = new_node;
}