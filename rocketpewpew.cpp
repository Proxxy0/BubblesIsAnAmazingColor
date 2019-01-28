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
#include <ofstream>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "GPIOClass.h"

const double GRAVITY = -9.8;
const double METER_TO_FEET = 3.28084;
const double FEET_TO_METER = 1/METER_TO_FEET;

using namespace std;

//##############################################################################
//classes
//##############################################################################
//class for 'altimeter' variable storage
class Jimbo{
  private:
  ofstream wikiHow;

  //Position, Velocity, and Acceleration values
  double preH, curH, maxH;
  double preV, curV, maxV;
  double preA, curA, maxA;

  //Initial height
  double initH;

  //height to fire main charge
  double mainLHFt = 500;
  double mainLHM = FEET_TO_METER*mainLHFt;

  //max height ratio to fire drogue charge
  double drogueLRatio = 0.9;

  //boolean to determine if the drogue 'chute has fired.
  bool drogueFired = 0;
  bool mainFired = 0;

  //internal counter
  int counter = 0;

  //elapsed time
  clock_t totalT;
  clock_t dt;

  //second interval between arming beeps
  const double byap = 1;

  //averaging window
  const int window = 100;

  //averaging array
  double avgArray[window] = {0};

  //GPIO pins
  GPIOClass* droguePin = new GPIOClass("4");
  GPIOClass* mainPin = new GPIOClass("5");

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

  int endFlight();


  public:

  //appends a value to avgArray and updates H, V, and A
  int updateAll();

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
  int drogueT = clock() - totalT;
  wikiHow << "Drogue fired at time: " << (int)((drogueT/CLOCKS_PER_SEC)/60) << " minute(s) ";
  wikiHow << ((drogueT/CLOCKS_PER_SEC)%60) << " second(s)" << endl;
  wikiHow << "Drogue fired at a height of: " << curH-initH << ";" << endl;
  drogueFired = 1;
  droguePin->setval_gpio("1");
}

//fire main charge
void Jimbo::fireMain(){
  //turn pin attached to main charge to HIGH
  int mainT = clock() - totalT;
  wikiHow << "Main fired at time: " << (int)((mainT/CLOCKS_PER_SEC)/60) << " minute(s) ";
  wikiHow << ((mainT/CLOCKS_PER_SEC)%60) << " second(s)" << endl;
  wikiHow << "Main fired at a height of: " << curH-initH << ";" << endl;
  mainFired = 1;
  mainPin->setval_gpio("1");
}

int Jimbo::endFlight(){
  totalT = clock() - totalT;
  wikiHow << "Total time elapsed: " << (int)((totalT/CLOCKS_PER_SEC)/60) << " minute(s)";
  wikiHow << ((totalT/CLOCKS_PER_SEC)%60) << " second(s);" << endl;
  wikiHow << "Max height achieved: " << maxH << " (that's a lot!);" << endl;
  wikiHow << "Max velocity achieved: " << maxV << " (woah, slow down there, Speed Racer);" << endl;
  wikiHow << "Max acceleration achieved: " << maxA << " (I think my neck hurts);" << endl;
  wikiHow.close()

  return 0;
}

//appends a value to avgArray and updates H, V, and A;
int Jimbo::updateAll();{
  //append(alt reading);
  setH();
  setV();
  setA();

  if((curV<0) && ((curH-initH)/(maxH-initH) <= drogueLRatio)){
    fireDrogue();
  }

  if((drogueFired) && ((curH-initH) <= mainLHFt)){
    fireMain();
  }

  if((curH<=1.01*initH) && drogueFired && mainFired && abs(curV)<= 0.1 && abs(curA)<= 0.1){
    return endFlight()
  }
}

// currently empty (possibly going to be parameterized). Will initialize height.
// velocity, and acceleration.
Jimbo::Jimbo(){
  totalT = clock();
  wikiHow.open("wikiHow.txt");
  droguePin->export_gpio();
  mainPin->export_gpio();
  droguePin->setdir_gpio("out");
  mainPin->setdir_gpio("out");
  for(int i = 0; i<window; i++){
    //append(altimeter reading)
  }
  setH();

  initH = curH;
  wikiHow << "Rocket starting at a height of: " << initH << ". All proceeding heights";
  wikiHow << " will be adjusted accordingly.;" << endl;

  //append(alt read);
  setH();
  setV();
  //append(alt read);
  setH();
  setV();
  setA();
}

//##############################################################################
//main function
//##############################################################################
int main(){
  Jimbo tonyTim;

  while(true){
    updateAll();
  }
}
