#include "simpletools.h"                      // Include simpletools header
#include "abdrive.h"                          // Include abdrive header
#include "ping.h"
#include "stdrobot.h"

#define Kp 1.25
#define BASESPEED 20

int irRight, irLeft, correction, goal = 11;
int pingDistance;

void getIR() {
        pingDistance = ping_cm(8);
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

void computePID() {
    double error;
    error = (double)(goal - irRight);
    correction = Kp * error;
}


int absolute(int x) {
    if (x < 0) { x *= -1;}
    return x;
}

int main() {

    low(26);
    low(27);

    while (1) {
        getIR();
        computePID();
        printf("Hoal");
        printf("Correction: %d", correction);
        fflush(stdout);

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