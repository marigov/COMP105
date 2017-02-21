#include "simpletools.h"                      // Include simpletools header
#include "abdrive.h"                          // Include abdrive header
#include "ping.h"                             // Include ping header

int irLeft, irRight, error, integral = 0, last_error, derivative;
int speed = 0, kp = 1, ki=1, kd=1;



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

                         
int main()                                    // main function
{   
  
  
        long currentTime = dt_getms();
  

        
                
        /*
        
         
         
         
         
        drive_setRampStep(10);                      // 10 ticks/sec / 20 ms
        drive_ramp(50, 50);                       // Forward 2 RPS
        
        detectIR();
        printf("Ping: %d, left IR: %d, right IR: %d \n", ping_cm(8),irLeft, irRight);
        fflush(stdout);                          // rotate right
        while(ping_cm(8) >= 30) pause(5); 
                                                    // Wait until object in range
        drive_ramp(0, 0);                           // Then stop
        
        drive_speed(25, -25);
        detectIR();
        printf("Ping: %d, left IR: %d, right IR: %d \n", ping_cm(8),irLeft, irRight);
        fflush(stdout);                          // rotate right
        while(ping_cm(8) < 30);                     // Turn till object leaves view
        
        drive_ramp(0, 0);  
        
        */       
    
}
