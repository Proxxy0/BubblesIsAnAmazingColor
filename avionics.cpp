/* uh yeah lettuce and space buses
 +  + ++ ---=== >>>|xxoxx|>      ...zoom
*/


const GRAVITY = -9.82;
const WINDOW  = 50 //iirc 
const beepInterval = 1;

#include <iostream>
#include <ctime>
#include <cstdlib>

using namespace std;

int main() 
{
   //VARIABLES
   double curV;
   double prevV;

   double curAccel;
   double prevAccel;

   double curH;
   double prevH;

   double movingAvg;
   
   double A[10] = {0};
   
   double sumH;
   int threshhold = 100;
   int gap = 10;
   double alpha = 0.1;
   //THE STUFF

   //TIMER----------------------------------------------------------
   
   clock_t startTime = clock(); //Start timer

	clock_t testTime;
	clock_t timePassed;
	double secondsPassed;

	while(true)
	{
		testTime = clock();
		timePassed = startTime - testTime;
		secondsPassed = timePassed / (double)CLOCKS_PER_SEC;

		if(secondsPassed >= beepInterval)
		{
			//cout << secondsToDelay << "seconds have passed" << endl;
			beep();
		}
	}
//I've never done a timer before hopefully this is close

   //INITIALIZATION--------------------------------------------------
   for(int k=0;k=gap-1;k++){
      for(int i=0;i<threshhold;i++){
         sumH+=altimeter.getH();
      }
      A[i]=sumH/threshhold;
   }

   for(int i=0;i=gap-1;i++){
      for(int j=0;j=WINDOW-1;j++){
       movingAvg=alpha*altimeter.getH()+(1-alpha)*movingAvg;
      }
      A[i] = movingAvg;
      
      //case ??9 maybe 0 idk
      if(i==9)
         curH = (A[0]+A[i])/2;      
      else{
         curH = (A[i+1]+A[i])/2
      }
      //the cake is a lie
      if(i==gap-1){      
         i = 0;
      }
   }






   //big lööp
   for(j=0;j=gap-1;j++)
   {
      //case 0
      if(j==0)
         curH = (A[0]+A[9])/2;      
      
      //A[0] = uhh is this where you calculate the average of the 50 points in the window 
      for(int i=0;i<WINDOW;i++)
      {
         movingAvg = Altimeter.getH();  //idk how altimeters work
      }
      A[j] = movingAvg;
      curH = (A[j+1] + A[j])/2;

      //the cake is a lie
      if(j==gap-1)      
         j = 0;
   }

   return 0;
}
