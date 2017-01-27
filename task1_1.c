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

int main() {
  
  //No space to start from the center in the simulator
  drive_goto(cmToTicks(-50),cmToTicks(-50));
  turn(-90);
  drive_goto(cmToTicks(50),cmToTicks(50));
  turn(90);
  
  //Starting square
  int meterTicks = cmToTicks(100);
  drive_goto(meterTicks,meterTicks);
  turn(90);
  drive_goto(meterTicks,meterTicks);
  drive_goto(-25,27); //modified for accurate turn
  drive_goto(meterTicks,meterTicks);
  turn(90);
  drive_goto(meterTicks,meterTicks);
  drive_goto(-25,27); //modified for accurate turn
    
  return 0;
}                                    