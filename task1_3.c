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
  drive_goto(-(distanceDrive),(distanceDrive));
}

void letter_A(int length) {
   int sideAngle = 70;
   turn(sideAngle);
   int sideL = length / sin(degToRad(sideAngle));
   drive_goto(cmToTicks(sideL),cmToTicks(sideL)); 
   turn(-(sideAngle*2));
   drive_goto(cmToTicks(sideL),cmToTicks(sideL)); 
   turn(184);
   drive_goto(cmToTicks(sideL/2),cmToTicks(sideL/2));
   turn(74);
   drive_goto(cmToTicks(sideL/2),cmToTicks(sideL/2));
   turn(184);
   drive_goto(cmToTicks(sideL/2),cmToTicks(sideL/2));
  turn(-74);
  drive_goto(cmToTicks(sideL/2),cmToTicks(sideL/2));
  turn(74);

   

   

    

   
}

int main() {
  letter_A(20);
}                                    