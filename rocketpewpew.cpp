​/* uh yeah lettuce and space buses
+ + ++ ---=== >>>|xxoxx|> ...zoom
*/

//##############################################################################
//inclusions, constants and namespace
//##############################################################################
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <time.h>
#include <math.h>

const double GRAVITY = -9.8;

using namespace std;

//##############################################################################
//classes
//##############################################################################
//class for 'altimeter' variable storage
class Jimbo{
  private:
  //Position, Velocity, and Acceleration values
  double preH, curH, maxH;
  double preV, curV, maxV;
  double preA, curA, maxA;

  //internal counter
  int counter = 0;

  //elapsed time
  clock_t dt;

  //second interval between arming beeps
  const double byap = 1;

  //averaging window
  const int window = 100;

  //averaging array
  double avgArray[window] = {0};

  //averages an array of points
  double average(double *points, int numPnts);

  //arming beeps
  void beep();

  //updates the previous height, vel., accl., and updates their respective values
  // based on average heights and the change in height and velocity respectively
  void setH();
  void setV();
  void setA();

  //appends a value, preferably the altimeter reading, to avgArray
  void append(double);

  //four is drogue; five is main
  void fireDrogue();
  void fireMain();


  public:

  //appends a value to avgArray and updates H, V, and A
  void updateAll();

  //class constructor and destructor
  Jimbo();
  ~Jimbo();
};

//##############################################################################
//class definitions
//##############################################################################
//averages an array of points
double Jimbo::average(double *points, int numPnts){
  double mean;
  for(int i = 0; i<numPnts; i++){
    mean += points[i]/numPnts;
  }

  return mean;
  dt = clock() - dt;
}

//unsure what to place here, so currently is used as a placeholder
void Jimbo::beep(){ //beepu

}

//takes an average of heights for the new current height as a parameter, and updates the
// current height and previous heights in measure. Similarly, checks to see if
// the max height has increased.
void Jimbo::setH(){
  preH = curH;
  curH = average(avgArray, window);

  if(maxH<curH){
    maxH = curH;
  }
}

//uses the change in height over the change in time to compute the current velocity
// whilst also updating the previous velocity. Similarly, checks to see if the
// max velocity has increased.
void Jimbo::setV(){
  preV = curV;
  curV = (curH-preH)/((float)dt/CLOCKS_PER_SEC);

  if(maxV<curV){
    maxV = curV;
  }
}

//uses the change in velocity over the change in time to compute the current
// acceleration whilst also updating the previous acceleration. Similarly, checks
// to see if the max acceleration has increased.
void Jimbo::setA(){
  preA = curA;
  curA = (curV-preV)/((float)dt/CLOCKS_PER_SEC);

  if(maxA<curA){
    maxA = curA
  }
}

//appends an input to a circular array, avgArray
void Jimbo::append(double newInput){
  dt = clock();
  avgArray[counter] = newInput;
  counter++;
}

//fires drogue charge
void Jimbo::fireDrogue(){
  //turn pin attached to drogue charge to HIGH
}

//fire main charge
void Jimbo::fireMain(){
  //turn pin attached to main charge to HIGH
}

//appends a value to avgArray and updates H, V, and A;
void Jimbo::updateAll();{
  //append(alt reading);
  setH();
  setV();
  setA();
}

// currently empty (possibly going to be parameterized). Will initialize height.
// velocity, and acceleration.
Jimbo::Jimbo(){
  for(int i = 0; i<window; i++){
    //append(altimeter reading)
  }
  //setH();
  //append(alt read);
  //setH();
  //setV();
  //append(alt read);
  //setH();
  //setV();
  //setA();
}

//##############################################################################
//main function
//##############################################################################
int main(){
  clock_t globalTime;
  Jimbo tonyTim;

  while(true){

  }

  globalTime = clock() - globalTime;
}
