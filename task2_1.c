#include "simpletools.h"
#include "abdrive.h"



int main() {
	int length = 0;
	print("Enter number of elements: \n");
	scan("%d",&length); 
	
	int list[length];
	print("Enter %d of elements: \n",length);
	for(int i = 0; i < length; i++) {
		scan("%d", list + i);
	}

	int i = 0;
	int n = 0;
	int unordered = 1; // flag if end of loop equals 0 then exit becuase the list is ordered

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
	
	print("Sorted list in ascending order: \n");
	
	for (int i = 0; i < length; i++) {
		print("%d ", list[i]);
	}
	
	print("\n");

	return 0;
}
