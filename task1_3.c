#include "simpletools.h"                        // Library includes
#include "abdrive.h"
#include <stdio.h>

#define ROBOT_WIDTH 10.58
#define WHEEL_DIAM 20.8 

const int Fsize=50;


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

void letter_A(int length) {
  turn(90);
  drive_goto(cmToTicks(length),cmToTicks(length)); 
  turn(-90);
  drive_goto(cmToTicks(length/2),cmToTicks(length/2)); 
  turn(-90);
  drive_goto(cmToTicks(length),cmToTicks(length));
  drive_goto(cmToTicks(-length/2),cmToTicks(-length/2));
  turn(90);
  drive_goto(cmToTicks(-length/2),cmToTicks(-length/2));
  drive_goto(cmToTicks(length/2),cmToTicks(length/2));
  turn(-90);
  drive_goto(cmToTicks(length/2),cmToTicks(length/2));
  turn(90);
}

void letter_E(int length) {
  turn(90);
  drive_goto(cmToTicks(length),cmToTicks(length)); 
  turn(-90);
  drive_goto(cmToTicks(length/2),cmToTicks(length/2)); 
  drive_goto(cmToTicks(-length/2),cmToTicks(-length/2));
  turn(-90);
  drive_goto(cmToTicks(length/2),cmToTicks(length/2));
  turn(90);
  drive_goto(cmToTicks(length/2),cmToTicks(length/2));
  drive_goto(cmToTicks(-length/2),cmToTicks(-length/2));
  turn(-90);
  drive_goto(cmToTicks(length/2),cmToTicks(length/2));
  turn(90);
  drive_goto(cmToTicks(length/2),cmToTicks(length/2));
}

void letter_F(int length) {
  turn(90);
  drive_goto(cmToTicks(length),cmToTicks(length)); 
  turn(-90);
  drive_goto(cmToTicks(length/2),cmToTicks(length/2)); 
  drive_goto(cmToTicks(-length/2),cmToTicks(-length/2));
  turn(-90);
  drive_goto(cmToTicks(length/2),cmToTicks(length/2));
  turn(90);
  drive_goto(cmToTicks(length/3),cmToTicks(length/3));
  drive_goto(cmToTicks(-length/3),cmToTicks(-length/3));
  turn(-90);
  drive_goto(cmToTicks(length/2),cmToTicks(length/2));
  turn(90);
  drive_goto(cmToTicks(length/2),cmToTicks(length/2));
}

void letter_H(int length) {
  turn(90);
  drive_goto(cmToTicks(length),cmToTicks(length)); 
  drive_goto(cmToTicks(-length/2),cmToTicks(-length/2)); 
  turn(-90);
  drive_goto(cmToTicks(length/2),cmToTicks(length/2));
  turn(90);
  drive_goto(cmToTicks(length/2),cmToTicks(length/2));
  drive_goto(cmToTicks(-length),cmToTicks(-length));
  turn(-90);
}

void letter_I(int length) {
  turn(90);
   drive_goto(cmToTicks(length),cmToTicks(length));
   drive_goto(cmToTicks(-length),cmToTicks(-length));
   turn(-90);
}

void letter_L(int length) {
   int sideL = length / 2;
  turn(90);
   drive_goto(cmToTicks(length),cmToTicks(length));
   drive_goto(cmToTicks(-length),cmToTicks(-length));
  turn(-90);
   drive_goto(cmToTicks(length/2),cmToTicks(length/2));
}

void letter_K(int length) {
   int sideL = length * sin(degToRad(45));
   turn(90);
   drive_goto(cmToTicks(length),cmToTicks(length));
   drive_goto(cmToTicks(-length/2),cmToTicks(-length/2));
   turn(-45);
   drive_goto(cmToTicks(sideL),cmToTicks(sideL));
   drive_goto(cmToTicks(-sideL),cmToTicks(-sideL));
   turn(-90);
   drive_goto(cmToTicks(sideL),cmToTicks(sideL));
   turn(45);
}



void wordMove(char *g) {
  while(*g){
    drive_goto(cmToTicks(10),cmToTicks(10));
    switch(*g) {
      case 'a':
      case 'A':
        print("A\n");
        letter_A(30);
        break;
      case 'e':
      case 'E':
        print("E\n");
        letter_E(30);
        break;
      case 'h':
      case 'H':
        print("H\n");
        letter_H(30);
        break;
      case 'f':
      case 'F':
        print("F\n");
       letter_F(30);
        break;
      case 'i':
      case 'I':
        print("I\n");
        letter_I(30);
        break;
      case 'l':
      case 'L':
        print("L\n");
        letter_L(30);
        break;
      case 'k':
      case 'K':
        print("K\n");
        letter_K(30);
        break;
  
        break;
      default:
        break;
    }
    g++;
  }
}


int main() {
  char word[20];
  printf("Please, enter a word: \n");
  scanf("%s", word);
  drive_goto(cmToTicks(-50),cmToTicks(-50));
  wordMove(word);
  return 0;
 
}                                    