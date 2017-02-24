#include "simpletools.h"                      // Include simpletools header
#include "abdrive.h"                          // Include abdrive header
#include "ping.h"
#include <time.h>


int irRight;
int irLeft;

double inputData, output, setPoint;
double errSum, lastInput, ITerm;
double kp = 1.105, ki = 0.0001, kd = 0.002;
int baseSpeed = 86;

clock_t now, lastTime, difference;

void getIR(){
    for (int dacVal = 0; dacVal < 160; dacVal += 8) {
        dac_ctr(26, 0, dacVal);
        freqout(11, 1, 38000);
        irLeft += input(10);                          // Left side IR-sensor

        dac_ctr(27, 1, dacVal);
        freqout(1, 1, 38000);
        irRight += input(2);                          // Right side IR-sensor
    }
}

void compute(){
    now = clock();
    difference = now-lastTime;
    if(difference > 10000){
    printf("Time: %ld \n", difference);
    inputData = irLeft - irRight;
    double error = 0 - inputData;
    ITerm += (ki * error);
    double dInput = inputData - lastInput;
    if ((ITerm > 0 && error <= 0) || (ITerm < 0 && error >= 0)) {
        ITerm = 0;
        printf("Resseet");
        fflush(stdout);
    }
    output = kp*error + ITerm - kd*dInput;
    lastInput = inputData;
    printf("Difference: %f",inputData);
    printf("Kp*error: %f\nITerm: %f\nkd*dInput: %f\n", kp*error,ITerm, -kd*dInput);
    fflush(stdout);
    lastTime = now;
}
}

int main(){
    low(26); //leftIR pin
    low(27); //rightIR pin
    while(1){
        int frontDistance = ping_cm(8);
        if(frontDistance > 10){
            getIR();
            compute();
            printf("%f\n", output);
            fflush(stdout);
            if(output==0){
                printf("--No turn \n");
                fflush(stdout);
                drive_speed(baseSpeed, baseSpeed);
            } else if(output > 0) {
                printf("--Right turn \n");
                fflush(stdout);
                drive_speed(baseSpeed+(output),baseSpeed+(output*-1));
            }
            else if(output < 0) {
                printf("--Left turn \n");
                fflush(stdout);
                drive_speed(baseSpeed+(output),baseSpeed+(output*-1));
            }
            irRight = 0;
            irLeft = 0;
        }
        else {
            drive_speed(0,0);
        }

    }
    return 0;
}
