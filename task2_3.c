#include "simpletools.h"                        // Library includes
#include "abdrive.h"

#define ROBOT_WIDTH 10.58
#define WHEEL_DIAM 20.8 
#define PAPER 16


int cmToTicks(double cm) {
  return (cm/((WHEEL_DIAM/64)));
}

double degToRad(int deg) {
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
  int length;
  print("Enter number of elements: \n");
  scan("%d\n",&length); 
  int list[length];

  print("Enter %d of elements: \n",length);

  for(int i = 0; i < length; i++) {
      scan("%d", &list[i]);
  }

  int i = 1;
  int n = 0;
  int unordered = 1;
  int counter = 0;
  int goBack = 0;

  while(unordered==1){
    unordered = 0;
    i = 0;
    n += 1;
    int counter = 0;
    goBack = 0;
    while(i<(length-n)){
      if(list[i] > list[i+1]) {
        int temp = list[i];
        if (counter==0){
        drive_goto(cmToTicks(PAPER+PAPER/2),cmToTicks(PAPER+PAPER/2));
        goBack += PAPER+PAPER/2;
        } else {
          drive_goto(cmToTicks(PAPER*2),cmToTicks(PAPER*2));
          goBack += PAPER*2;

        }
        high(26);
        pause(1000);
        low(26);
        counter += 1;
        list[i] = list[i+1];
        list[i+1] = temp;
        unordered = 1;
        /*for(int i = 0; i < length; i++) {
          print("%d ", list[i]);
        }
        print("\n");*/
      }
    i++;
  }
  drive_goto(-cmToTicks(goBack),-cmToTicks(goBack));
}
  int halfWidth = ((PAPER*length)+(PAPER*(length-1)))/2;
  drive_goto(cmToTicks(halfWidth),cmToTicks(halfWidth));
  turn(90);
  return 0;
}
