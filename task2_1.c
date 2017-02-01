#include <stdio.h>
#include "simpletools.h"                        // Library includes
#include "abdrive.h"


int main() {
  int length;
  printf("Enter number of elements: \n");
  scanf("%d",&length); 
  int list[length];
  
  printf("Enter %d of elements: \n",length);
  
  for(int i = 0; i < length; i++) {
      scanf("%d", list + i);
  }
  
  int i = 1;
  int n = 0;
  int unordered = 1;
    
  while(unordered==1){
    unordered = 0;
    i = 0;
    n += 1;
    while(i<(length-n)){
      if(list[i] > list[i+1]) {

        int temp = list[i];
        list[i] = list[i+1];
        list[i+1] = temp;
        unordered = 1;
      }
      
    i++;
  }
}
  
  return 0;
}
