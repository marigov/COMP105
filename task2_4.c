#include "simpletools.h"                        // Library includes
#include "abdrive.h"

void towerOfHanoi(int n, char initPeg, char auxPeg, char endPeg){

  if(n == 1){
    printf("%c to %c\n", initPeg, endPeg);
    
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