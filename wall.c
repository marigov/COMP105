#include "simpletools.h"                      // Include simpletools header
#include "abdrive.h"                          // Include abdrive header
#include "ping.h"
#include <time.h>
#include "stdrobot.h"

#define Kp 1.15
#define Ki 0.001
#define Kd 0.02
#define BASESPEED 40

volatile int irRight, irLeft, correction, notAtWall = 1;
int pingDistance;
double lastError = 0;
double errorSum = 0;



int rightDistance, leftDistance;

volatile int irCog;
volatile int pingCog;
volatile int pidCog;

unsigned int irStack[128];
unsigned int pingStack[64];
unsigned int pidStack[128];

double get_time_sec() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)t.tv_sec + 1.0e-9 * t.tv_nsec;
}

void getPing() {
    pingDistance = ping_cm(8);
}

void getIR() {
    low(26);
    low(27);
    while (notAtWall) {
        getPing();
        int tmpRight = 0;
        int tmpLeft = 0;
        for (int dacVal = 0; dacVal < 160; dacVal += 8) {
            dac_ctr(26, 0, dacVal);
            freqout(11, 1, 38000);
            tmpLeft += input(10);                  // Left side IR-sensor

            dac_ctr(27, 1, dacVal);
            freqout(1, 1, 38000);
            tmpRight += input(2);                  // Right side IR-sensor
        }
        irRight = tmpRight;
        irLeft = tmpLeft;
    }
}

void computePID() {
    double init = get_time_sec();
    double timeSpan;
    double now;
    double error;
    double dError;
    while (notAtWall) {
        now = get_time_sec();
        timeSpan = now - init;
        if (timeSpan > 0.005) {
            error = (double)(irLeft - irRight);
            errorSum += (error * timeSpan);
            dError = (error - lastError) / timeSpan;

            correction = Kp * error + Ki * errorSum + Kd * dError;
            //printf("--\nC: %d\n--\nP: %f\nI: %f\nD: %f\n", correction, Kp * error, Ki * errorSum, Kd * dError);

            lastError = error;
            init = now;
        }
    }
}

int absolute(int x) {
    if (x < 0) { x *= -1;}
    return x;
}



int main() {
    irCog = cogstart(getIR, NULL, irStack, sizeof(irStack));
    pidCog = cogstart(computePID, NULL, pidStack, sizeof(pidStack));
    getPing();
    double prevAngle = 0;
    while (1) {

        // leftDistance = *(getDistanceWheels());
        // rightDistance = *(getDistanceWheels() + 1);
        // printf("%f, %f\n", *(getDistanceWheels()), *(getDistanceWheels() + 1));
       // printf("Angle: %f\n",getAngleChange(getDistanceWheels()));
        double* coordenades = getCoordinates(getDistanceWheels(),0);
        printf("%f, %f\n",*coordenades,*(coordenades+1));
        fflush(stdout);
        prevAngle = getAngleChange(getDistanceWheels());
        prevAngle = getAngleChange(getDistanceWheels());
        if (pingDistance > 15) {
            if (correction == 0 || (absolute(correction) < 1 && absolute(correction) > -1)) {
                drive_speed(BASESPEED, BASESPEED);
            }
            else {
                if (correction > 0) { // needs to turn left
                    drive_speed(BASESPEED - correction, BASESPEED + correction);

                }
                else if (correction < 0) { // needs to turn right
                    drive_speed(BASESPEED - correction, BASESPEED + correction);
                }
            }
        } else {
            notAtWall = 0;
            break;
        }
    }
    fflush(stdout);
    return 0;

}