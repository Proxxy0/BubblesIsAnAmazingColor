
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
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <wiringPi.h>
#include <softTone.h>

#define droguePin 5
#define mainPin 6
#define beepPin 18

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
  double prevAlt = 0;

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

  
  

  //averages an array of points
  double average(double *points, int numPnts);

  //arming beeps
  void beep(int);

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
