#include "simpletools.h"                      // Include simpletools header
#include "abdrive.h"                          // Include abdrive header
#include "ping.h"
#include <time.h>


//1.35 0.001 0.35

#define Kp 1.25
#define Ki 0.01
#define Kd 0.45

#define BASESPEED 48

//IR sensor values
volatile int irRight, irLeft;
unsigned int stack[40 + 25];                  // Stack vars for other cog
void getIR();

//delay in seconds for measurament
double delay = 0.5;

//pid variables

double errorSum = 0;
double lastError = 0;
int correction;

void computePID();
void getPing();

volatile int ping_distance;

double get_time_sec() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)t.tv_sec + 1.0e-9 * t.tv_nsec;
}

void getPing() {
    while (1) {
        ping_distance = ping_cm(8);
    }
}

void getIR() {
    low(26);
    low(27);
    while (1) {
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
    while (1) {
        double now = get_time_sec();
        double timeSpan = now - init;

        if (timeSpan > 0.05) {
            double error = (double)(irLeft - irRight);
            errorSum += (error * timeSpan);
            double dError = (error - lastError) / timeSpan;


            correction = Kp * error + Ki * errorSum + Kd * dError;
            printf("--\nC: %d\n--\nP: %f\nI: %f\nD: %f\n", correction, Kp * error, Ki * errorSum, Kd * dError);

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
    // cogstart(getIR, NULL, stack, sizeof(stack));
    // cogstart(computePID, NULL, stack, sizeof(stack));
    // cogstart(getPing, NULL, stack, sizeof(stack));

    cog_run(getIR, 128);
    cog_run(computePID, 128);
    cog_run(getPing, 64);
    while (1) {
        // pid correction value calculation with a specific time delay
        //printf("IR values: R: %d, L: %d \n", irRight, irLeft);
        while (ping_distance > 10) {
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
        }
        printf("stop!");
        fflush(stdout);
        drive_speed(0, 0);
    }

    return 0;
}
