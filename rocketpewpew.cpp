​/* uh yeah lettuce and space buses
+ + ++ ---=== >>>|xxoxx|> ...zoom
*/

const double GRAVITY = -9.8;
const int WINDOW = 50; //iirc

#include <iostream>
#include <ctime>
#include <cstdlib>

using namespace std;

//class for 'altimeter' variable storage
class Jimbo{
    public: // placeholder until everything is put into functions
    //private:
    //Position, Velocity, and Acceleration values
    double preH, curH, maxH;
    double preV, curV, maxV;
    double preA, curA, maxA;

    //placeholder for elapsed time
    double dt;

    //second interval between arming beeps
    const double beepInterval = 1;

    //averaging variables
    const int threshhold = 100;
    const int gap = 10;
    const int numPoints = threshhold/gap;
    const double alpha = 0.1;

    double Avg[numPoints] = {0};
    double sumH;
    double movingAvg;

    //averages an array of points
    double average(double *points, int numPnts);

    //arming beeps
    void beep();

    //updates the previous vel. and accl., and updates their respective values
    // based on the change in height and velocity respectively
    void setV();
    void setA();


    //public:
    //updates the previous height, and sets the current height to input
    void setH(double);

    //class constructor and destructor
    tinyTimb();
    ~tinyTimb();

    //returns the current height of the object
    double getH();
};

//averages an array of points
double tinyTimb::average(double *points, int numPnts){
  double mean;
  for(int i = 0; i<numPnts; i++){
    mean += points[i]/numPnts;
  }

  return mean;
}

//unsure what to place here, so currently is used as a placeholder
void tinyTimb::beep(){ //beepu

}

//uses the change in height over the change in time to compute the current velocity
// whilst also updating the previous velocity. Similarly, checks to see if the
// max velocity has increased.
void tinyTimb::setV(){
  preV = curV;
  curV = (curH-preH)/dt;

  if(maxV<curV){
    maxV = curV;
  }
}


//uses the change in velocity over the change in time to compute the current
// acceleration whilst also updating the previous acceleration. Similarly, checks
// to see if the max acceleration has increased.
void tinyTimb::setA(){
  preA = curA;
  curA = (curV-preV)/dt;

  if(maxA<curA){
    maxA = curA
  }
}

//takes an input for the new current height as a parameter, and updates the
// current height and previous heights in measure. Similarly, checks to see if
// the max height has increased.
void tinyTimb::setH(double newH){
  preH = curH;
  curH = newH;

  if(maxH<curH){
    maxH = curH;
  }
}


// currently empty (possibly going to be parameterized). Will initialize height.
// velocity, and acceleration.
tinyTimb::tinyTimb(){

}

//returns the current height of the object.
double tinyTimb::getH(){
  return curH;
}

/*
##################################################################
psst. things get nasty(-er) after this point. you've been warned.
##################################################################
*/


int main()
{
    Jimbo tinyTimb;

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
       //beep();
     }
   }

   //I've never done a timer before hopefully this is close

   //INITIALIZATION--------------------------------------------------

   for(int k=0;k<tinyTimb.gap;k++){
      for(int i=0;i<tinyTimb.threshhold;i++){
         tinyTimb.sumH+=tinyTimb.getH();
      }
      A[k]=tinyTimb.sumH/tinyTimb.threshhold;
   }

   for(int i=0;i<tinyTimb.gap;i++){
      for(int j=0;j<WINDOW;j++){
       tinyTimb.movingAvg=tinyTimb.alpha*tinyTimb.getH()+(1-tinyTimb.alpha)*tinyTimb.movingAvg;
      }
      tinyTimb.A[i] = tinyTimb.movingAvg;

      //case ??9 maybe 0 idk
      if(i==9){
         tinyTimb.curH = (A[0]+A[i])/2;
      }else{
         tinyTimb.curH = (A[i+1]+A[i])/2;
      }

      //the cake is a lie
      if(i==tinyTimb.gap-1){
         i = 0;
      }
   }

   //big lööp
   for(int j=0;j<tinyTimb.gap;j++)
   {
      //case 0
      if(j==0){
         tinyTimb.curH = (A[0]+A[9])/2;
       }

      //A[0] = uhh is this where you calculate the average of the 50 points in the window
      for(int i=0;i<WINDOW;i++)
      {
         tinyTimb.movingAvg = tinyTimb.getH(); //idk how altimeters work
      }

      A[j] = tinyTimb.movingAvg;
      tinyTimb.curH = (A[j+1] + A[j])/2;

      //the cake is a lie
      if(j==tinyTimb.gap-1)
         j = 0;
   }
   return 0;
}
