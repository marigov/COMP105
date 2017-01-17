#include "simpletools.h"                        // Library includes
#include "abdrive.h"

int main()                                      // main function
{
  while(1)
    {
      drive_goto(26, -27);
      pause(1000);
      drive_goto(-26, 27);
      pause(1000);
    }
}

