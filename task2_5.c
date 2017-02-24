#include "simpletools.h"                        // Library includes
#include "abdrive.h"

#define ROBOT_WIDTH 10.58
#define WHEEL_DIAM 20.8 
#define PAPER 16 // width paper 
#define H_PAPER 8 // width half paper


int cmToTicks(double cm) {
    return (cm/((WHEEL_DIAM/64)));
}


void towerOfHanoi(int n, char initPeg, char auxPeg, char endPeg){
    if(n == 1){
        printf("%c to %c\n", initPeg, endPeg);
    
        if(initPeg == 'C' && endPeg == 'B'){
            high(26);
            drive_goto(cmToTicks(2*PAPER),cmToTicks(2*PAPER));
            low(26);
            drive_goto(cmToTicks(-2*PAPER),cmToTicks(-2*PAPER));
        }
        else if (initPeg == 'C' && endPeg == 'A'){
            high(26);
            drive_goto(cmToTicks(4*PAPER),cmToTicks(4*PAPER));
            low(26);
            drive_goto(cmToTicks(-4*PAPER),cmToTicks(-4*PAPER));
        }
        else if (initPeg == 'B' && endPeg == 'A'){
            drive_goto(cmToTicks(2*PAPER),cmToTicks(2*PAPER));
            high(26);
            drive_goto(cmToTicks(2*PAPER),cmToTicks(2*PAPER));
            low(26);
            drive_goto(cmToTicks(-4*PAPER),cmToTicks(-4*PAPER));
      
        }
        else if (initPeg == 'B' && endPeg == 'C'){
            drive_goto(cmToTicks(2*PAPER),cmToTicks(2*PAPER));
            high(26);
            drive_goto(cmToTicks(-2*PAPER),cmToTicks(-2*PAPER));
            low(26);
        }
        else if (initPeg == 'A' && endPeg == 'B'){
            drive_goto(cmToTicks(4*PAPER),cmToTicks(4*PAPER));
            high(26);
            drive_goto(cmToTicks(-2*PAPER),cmToTicks(-2*PAPER));
            low(26);
            drive_goto(cmToTicks(-2*PAPER),cmToTicks(-2*PAPER));
        }
        else if (initPeg == 'A' && endPeg == 'C'){
            drive_goto(cmToTicks(4*PAPER),cmToTicks(4*PAPER));
            high(26);
            drive_goto(cmToTicks(-4*PAPER),cmToTicks(-4*PAPER));
            low(26);
        }
    }
    else {
        towerOfHanoi(n-1,  initPeg, endPeg, auxPeg);
        towerOfHanoi(1,  initPeg, auxPeg, endPeg);
        towerOfHanoi(n-1,  auxPeg, initPeg, endPeg);
    }

}

int main() {
    int disks;
    print("Enter number of diks: \n");
    scan("%d\n",&disks); 

    char initPeg;
    char auxPeg;
    char endPeg;

    print("Enter the initial peg: \n");
    scan("%c\n",&initPeg);

    if(initPeg == 'A'){
        auxPeg = 'B';
        endPeg = 'C';
    } 
    else if (initPeg == 'B'){
        auxPeg = 'A';
        endPeg = 'C';
    }
    else {
        auxPeg = 'B';
        endPeg = 'A';
    }
    print("Moves: \n");

    towerOfHanoi(disks, initPeg, auxPeg, endPeg);
    return 0;
}                                    