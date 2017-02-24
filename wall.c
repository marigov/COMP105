#include "simpletools.h"                      // Include simpletools header
#include "abdrive.h"                          // Include abdrive header
#include "ping.h"
#include <time.h>                             // Include ping heade

int irLeft = 0;
int irRight = 0;
int baseSpeed = 48;
float actual_error, error_previous, P, I, D, Kp = 1.45, Ki = 0, Kd = 0;

int main() {
    low(26);
    low(27);

    while (1) {
        for (int dacVal = 0; dacVal < 160; dacVal += 8) {
            dac_ctr(26, 0, dacVal);
            freqout(11, 1, 38000);
            irLeft += input(10);                          // Left side IR-sensor

            dac_ctr(27, 1, dacVal);
            freqout(1, 1, 38000);
            irRight += input(2);                          // Right side IR-sensor
        }

        error_previous = actual_error;
        actual_error = 0 - (irRight - irLeft);

        P  = actual_error;   //Current error
        I += error_previous;  //Sum of previous errors
        D  = actual_error - error_previous;  //Difference with previous error

        float correction = Kp * P + Ki * I + Kd * D;

        printf("%f \n", correction);
        printf("irLeft: %d, irRight: %d \n", irLeft, irRight);
        printf("irLeft: %d, irRight: %d \n", irLeft, irRight);

        if (correction == 0) {
            drive_speed(baseSpeed, baseSpeed);
        }
        else if (correction < 0) {
            drive_speed(baseSpeed + (correction * -1), baseSpeed + (correction));
        }
        else if (correction > 0) {
            drive_speed(baseSpeed + (correction * -1), baseSpeed + (correction));
        }
        irLeft = 0;
        irRight = 0;
    }
}
