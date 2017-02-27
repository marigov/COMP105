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
	return (*distanceWheels-*(distanceWheels+1))/ WHEEL_BASE;
}

double getRadMiddle(double* distanceWheels, double angleChange){
	double radLeft = *distanceWheels/angleChange;
	double radRight = *(distanceWheels+1)/angleChange;
	return (radLeft+radRight)/2;

}

double* getCoordinates(double* distanceWheels, double currentAngle){
	double* positionCoordinates = malloc(3*sizeof(double));
	double angleChange = getAngleChange(distanceWheels);
	if(angleChange==0){
		*positionCoordinates = *distanceWheels;
		*(positionCoordinates + 1) = 0;
	} 
	else {
		double radM = getRadMiddle(distanceWheels, angleChange);
		*positionCoordinates = radM*sin(currentAngle)-radM*sin(currentAngle+angleChange);
		*(positionCoordinates+1) = -(-radM*cos(currentAngle)+radM*cos(currentAngle+angleChange));
	}
	*(positionCoordinates + 2) = angleChange;
	return positionCoordinates;
}

int cmToTicks(double cm) {
  return (cm/((WHEEL_DIAM/64)));
}

double degToRad(double deg) {
  return (deg * PI / 180);
} 

void turnSpecificAngle(int angle) {
  double radAngle = degToRad(angle);
  int distanceDrive = radAngle * cmToTicks((ROBOT_WIDTH / 2));
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

//spinRad enables to spin around a point, given the nubmer of sides of a polygon (n) and the radius (r). Unused for now.
void spinRad(int n, int r) {
  float angle = 360 / n;
  float side = 2 * r * sin (PI / n);
  
  for (int i = 1; i <= n; i++) {
   drive_goto(cmToTicks(side),cmToTicks(side));
   turn(angle);
  }   
}

void push(node_correction ** head, int val, double time) {
    node_correction * new_node;
    new_node = malloc(sizeof(node_correction));

    new_node->val = val;
    new_node->time = time;
    new_node->next = *head;
    *head = new_node;
}