#include "simpletools.h"                      // Include simpletools header
#include "abdrive.h"                          // Include abdrive header
#include "ping.h"
#include <time.h>

int irRight;
int irLeft;
double kp = 1, ki = 0.1, kd = 0.01;
int baseSpeed = 86;
double error, lastError, output;
double P, I = 0, D;

double get_time_ms()
{
struct timeval t;
gettimeofday(&t, NULL);
return (t.tv_sec + (t.tv_usec / 1000000.0)) * 1000.0;
}

double init, now;



void getIR(){
    for (int dacVal = 0; dacVal < 160; dacVal += 8) {
        dac_ctr(26, 0, dacVal);
        freqout(11, 1, 38000);
        irLeft += input(10);                  // Left side IR-sensor

        dac_ctr(27, 1, dacVal);
        freqout(1, 1, 38000);
        irRight += input(2);                  // Right side IR-sensor
    }
}

// void compute(){
//     printf("Now: %f \n", now);
//     printf("lastTime: %f \n", lastTime);
//     printf("Difference: %f \n", difference);
//
//     fflush(stdout);
//     printf("Working!");
//     error = 0 - (double)(irLeft - irRight);
//     P = kp * error;
//     I += (difference * error);
//     D = kd*((error-lastError)/difference);
//     I *= ki;
//     printf("P: %f\n I: %f\nD: %f\n", P, I, D);
//     fflush(stdout);
//     output = P + I + D;         //+ I + D;+
//     fflush(stdout);
//
//     fflush(stdout);
//     //lastTime = now;
// }

int main(){
    low(26); //leftIR pin
    low(27); //rightIR pin
    // lastTime = time(NULL);
    init = get_time_ms();
    while(1) {
            now = get_time_ms();
            double difference = (now-init);
            if(difference>500){
                printf("Uep!\n");
                init = get_time_ms();
            }
        }
    return 0;
}
