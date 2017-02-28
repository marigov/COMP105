#include "simpletools.h"                      // Include simpletools header
#include "abdrive.h"                          // Include abdrive header
#include "ping.h"                             // Include ping header

int irLeft, irRight, error, integral = 0, last_error=0, derivative;
int correction=0;
int speed = 0;
#define BASESPEED 40
double kp = -1.5, ki=2, kd=-0.1;
// double kp = 4, ki=2, kd=2;
int MAXSIZE = 50;
int stack[50];
int top = -1;

int isempty() {

   if(top == -1)
      return 1;
   else
      return 0;
}

int isfull() {

   if(top == MAXSIZE)
      return 1;
   else
      return 0;
}

int peek() {
   return stack[top];
}

int pop() {
   int data;

   if(!isempty()) {
      data = stack[top];
      top = top - 1;
      return data;
   } else {
      printf("Could not retrieve data, Stack is empty.\n");
   }
}

int push(int data) {

   if(!isfull()) {
      top = top + 1;
      stack[top] = data;
   } else {
      printf("Could not insert data, Stack is full.\n");
   }
}



void detectIR(){
    low(26);
    low(27);

    irLeft = 0;
    irRight = 0;
    for(int dacVal = 0; dacVal < 160; dacVal += 8)
    {
      dac_ctr(26, 0, dacVal);
      freqout(11, 1, 38000);
      irLeft += input(10);

      dac_ctr(27, 1, dacVal);
      freqout(1, 1, 38000);
      irRight += input(2);
    }
}
int absolute(int x){
    if (x < 0){ x *= -1;}
    return x;
}

int main(){
  while (1) {
   detectIR();
   error = irRight - irLeft;
   derivative = error - last_error;
   speed = (kp * error) + (kd * derivative);
   correction = speed;
   if(error==0 || (absolute(correction) < 4 && absolute(correction) > -4)) {
       drive_speed(BASESPEED, BASESPEED);
   }else {
       if (error > 0) { // needs to turn left
           drive_speed(BASESPEED-correction, BASESPEED+correction);

       }
       else if (error < 0){ // needs to turn right
           drive_speed(BASESPEED-correction, BASESPEED+correction);
       }
   }
   printf("Speed %d IR right %d\n", speed, irLeft);
   fflush(stdout);
   last_error = error;
 }

}
