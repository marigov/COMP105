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

void turn(double angle) {
  double radAngle = degToRad(angle);
  int distanceDrive = radAngle * cmToTicks((ROBOT_WIDTH / 2));
  drive_goto(-(distanceDrive+1),(distanceDrive+1));
}



int main() {
  int meterTicks = cmToTicks(50);
  for(int i = 0; i < 4; i++) {
    drive_goto(meterTicks,meterTicks);
    drive_goto(-26,25);
  }
  return 0;
}                                    