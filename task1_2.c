#include "simpletools.h"                        // Library includes
#include "abdrive.h"

#define ROBOT_WIDTH 10.58
#define WHEEL_DIAM 20.8 

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

//spinRad enables to spin around a point, given the nubmer of sides of a polygon (n) and the radius (r)
void spinRad(int n, int r) {
  float angle = 360 / n;
  float side = 2 * r * sin (PI / n);
  
  for (int i = 1; i <= n; i++) {
   drive_goto(cmToTicks(round(side)),cmToTicks(round(side)));
   turn(angle);
  }   
}

void simpleSpin(){
  turn(360);
}

int main() {
  simpleSpin();
  return 0;
}                                    