#include "simpletools.h"                      // Include simpletools header
#include "abdrive.h"                          // Include abdrive header
#include "ping.h"
#include <time.h>
#include "stdrobot.h"

#define Kp 1.15
#define Ki 0.000
#define Kd 0.0
#define BASESPEED 8

volatile int irRight, irLeft, correction;
int pingDistance;
double lastError = 0;
double errorSum = 0;


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
    while (1) {
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
    while (1) {
        now = get_time_sec();
        timeSpan = now - init;
        if (timeSpan > 0.00) {
            error = (double)(irLeft - irRight);
            // errorSum += (error * timeSpan);
            // dError = (error - lastError) / timeSpan;

            correction = Kp * error; // + Ki * errorSum + Kd * dError;
            // printf("--\nC: %d\n--\nP: %f\nI: %f\nD: %f\n", correction, Kp * error, Ki * errorSum, Kd * dError);

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
    while (1) {
        if (pingDistance > 15) {
            if (correction == 0 || (absolute(correction) < 4 && absolute(correction) > -4)) {
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
            drive_speed(0, 0);
            pause(1000);
            turn(180);
            printf("stop!");
            fflush(stdout);
            printf("Stop\n");
        }
    }

    return 0;

}