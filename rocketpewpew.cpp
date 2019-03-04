
//##############################################################################
//inclusions, constants and namespace
//##############################################################################
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <time.h>
#include <math.h>
#include <fstream>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "GPIOClass.h"
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

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

  // I2C stuff
  int file;
  char *bus = "/dev/i2c-1";

  char config[2] = {0};

  

  char reg[1] = {0x00};
	char data[6] = {0};

  int tHeight, temp, pres;
  float altitude, cTemp, fTemp, pressure;


  //output file stream
  ofstream wikiHow;
  ofstream altimeterData;

  //Position, Velocity, and Acceleration values
  double preH, curH, maxH = 0;
  double preV, curV, maxV = 0;
  double preA, curA, maxA = 0;

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
  double totT=0;
  clock_t totalT;
  clock_t dt;

  //second interval between arming beeps
  const double byap = 1;

  //averaging window
  const static int window = 100;

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

  //int endFlight();

  void altimeterGather();

  void clearScreen();


  public:

  //test
  int endFlight();

  //returns elapsed time
  double getTotTime();

  //boolean to determine if the rocket has "landed"
  bool landed = 0;

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
    mean += points[i]/(float)numPnts;
  }

  dt = clock() - dt;
  return mean;
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
  altimeterData << curH << ", " << getTotTime() << endl;
  //cout << curH << endl;
  //clearScreen();

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
    maxA = curA;
  }
}

//appends an input to a circular array, avgArray
void Jimbo::append(double newInput){
  dt = clock();
  avgArray[counter] = newInput;
  counter++;
  if(!(counter < window)){
     counter = 0;
  }
}

//fires drogue charge
void Jimbo::fireDrogue(){
  //turn pin attached to drogue charge to HIGH
  int drogueT = clock() - totalT;
  wikiHow << "Drogue fired at time: " << (int)(((double)drogueT/CLOCKS_PER_SEC)/60) << " minute(s) ";
  wikiHow << ((int)((double)drogueT/CLOCKS_PER_SEC)%60) << " second(s)" << endl;
  wikiHow << "Drogue fired at a height of: " << curH-initH << "m;" << endl;
  drogueFired = 1;
  droguePin->setval_gpio("1");
}

//fire main charge
void Jimbo::fireMain(){
  //turn pin attached to main charge to HIGH
  int mainT = clock() - totalT;
  wikiHow << "Main fired at time: " << (int)(((double)mainT/CLOCKS_PER_SEC)/60) << " minute(s) ";
  wikiHow << ((int)((double)mainT/CLOCKS_PER_SEC)%60) << " second(s)" << endl;
  wikiHow << "Main fired at a height of: " << curH-initH << "m;" << endl;
  mainFired = 1;
  mainPin->setval_gpio("1");
}

int Jimbo::endFlight(){
  totalT = clock() - totalT;
  wikiHow << "Total time elapsed: " << (int)(getTotTime()/60) << " minute(s)";
  wikiHow << ((int)getTotTime()%60) << " second(s);" << endl;
  wikiHow << "Max height achieved: " << maxH << "m (that's a lot!);" << endl;
  wikiHow << "Max velocity achieved: " << maxV << "m/s (woah, slow down there, Speed Racer);" << endl;
  wikiHow << "Max acceleration achieved: " << maxA << "m/s/s (I think my neck hurts);" << endl;
  wikiHow.close();
  
  altimeterData.close();
  
  delete droguePin;
  delete mainPin;
  droguePin = NULL;
  mainPin = NULL;

  landed = 1;

  return 0;
}

void Jimbo::altimeterGather(){
  config[0] = 0x26;
	config[1] = 0xB9;
	write(file, config, 2);
	// Select data configuration register(0x13)
	// Data ready event enabled for altitude, pressure, temperature(0x07)
	config[0] = 0x13;
	config[1] = 0x07;
	write(file, config, 2);
	// Select control register(0x26)
	// Active mode, OSR = 128, altimeter mode(0xB9)
	config[0] = 0x26;
	config[1] = 0xB9;
	write(file, config, 2);
	//usleep(10000);
        //totT = totT+0.01;
	

	// Read 6 bytes of data from address 0x00(00)
	// status, tHeight msb1, tHeight msb, tHeight lsb, temp msb, temp lsb
	reg[1] = {0x00};
	write(file, reg, 1);
	data[6] = {0};
	read(file, data, 6);

	// Convert the data
	tHeight = ((data[1] * 65536) + (data[2] * 256 + (data[3] & 0xF0)) / 16);
	temp = ((data[4] * 256) + (data[5] & 0xF0)) / 16;
	altitude = tHeight / 16.0;
	cTemp = (temp / 16.0);
	fTemp = cTemp * 1.8 + 32;

	// Select control register(0x26)
	// Active mode, OSR = 128, barometer mode(0x39)
	config[0] = 0x26;
	config[1] = 0x39;
	write(file, config, 2);
	//usleep(10000);
        //totT = totT+0.01;

	// Read 4 bytes of data from register(0x00)
	// status, pres msb1, pres msb, pres lsb
	reg[0] = 0x00;
	write(file, reg, 1);
	read(file, data, 4);

	// Convert the data to 20-bits
	pres = ((data[1] * 65536) + (data[2] * 256 + (data[3] & 0xF0))) / 16;
	pressure = (pres / 4.0) / 1000.0;
}

//returns the elapsed time
double Jimbo::getTotTime(){
  
  return ((double)(clock()-totalT))/CLOCKS_PER_SEC + totT;
}


//appends a value to avgArray and updates H, V, and A;
void Jimbo::updateAll(){

  altimeterGather();
  append(altitude);
  setH();
  setV();
  setA();

  if((curV<0) && ((curH-initH)/(maxH-initH) <= drogueLRatio)){
    //fireDrogue(); //removed for testing purposes
  }

  if((drogueFired) && ((curH-initH) <= mainLHFt)){
    //fireMain(); //removed for testing purposes
  }

  if((curH<=1.01*initH) && drogueFired && mainFired && (curV <= 0.1 && curV >= -0.1) && (curA <= 0.1 && curA >= -0.1)){
    //endFlight();
  }
}

void Jimbo::clearScreen(){
  cout << flush;
  system("clear");
}

// currently empty (possibly going to be parameterized). Will initialize height.
// velocity, and acceleration.
Jimbo::Jimbo(){
  totalT = clock();
  wikiHow.open("wikiHow.txt");
  altimeterData.open("altimeterData.txt");
  //droguePin->export_gpio(); // not needed with GPIOClass_v2
  //mainPin->export_gpio(); // not needed with GPIOClass_v2
  cout << "I'm here!" << endl;
  //droguePin->setdir_gpio("out"); // for testing purposes, we are taking this out, so we dont need to reboot
  //mainPin->setdir_gpio("out");
  cout << "Now I'm here!" << endl;
  
  file = open(bus, O_RDWR);
  ioctl(file, I2C_SLAVE, 0x60);

  for(int i = 0; i<window; i++){
    altimeterGather();
    append(altitude);
  }
  setH();

  initH = curH;
  wikiHow << "Rocket starting at a height of: " << initH << "m. All proceeding heights";
  wikiHow << " will be adjusted accordingly.;" << endl;

  altimeterGather();
  append(altitude);
  setH();
  setV();
  altimeterGather();
  append(altitude);
  setH();
  setV();
  setA();
}

Jimbo::~Jimbo(){}

//##############################################################################
//main function
//##############################################################################
int main(){
  Jimbo tonyTim;
  cout << "initialization complete!" << endl;
  
  //run for 10 seconds
  while(tonyTim.getTotTime() < 0.5){
    cout << tonyTim.getTotTime() << endl;
    tonyTim.updateAll();
  }
  tonyTim.endFlight();
  if(tonyTim.landed){
     return 0;
  }
  return 0;
}
