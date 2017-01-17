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

void spinRad(int n, int r) {
  float angle = 360 / n;
  float side = 2 * r * sin (PI / n);
  
  for (int i = 1; i <= n; i++)         // repeats forward and right turn commands  
  {
   drive_goto(cmToTicks(round(side)),cmToTicks(round(side)));   // Move forward
   turn(angle);  // Turn right or left
  }  
  
}

void simpleSpin(){
  turn(370);
}

int main() {
  spinRad(20, 20);
}                                    