
#include "Jimbo.h"



using namespace std;
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
void Jimbo::beep(int freq){ //beepu
  softToneWrite(beepPin, freq);
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
  digitalWrite(droguePin, HIGH);
}

//fire main charge
void Jimbo::fireMain(){
  //turn pin attached to main charge to HIGH
  int mainT = clock() - totalT;
  wikiHow << "Main fired at time: " << (int)(((double)mainT/CLOCKS_PER_SEC)/60) << " minute(s) ";
  wikiHow << ((int)((double)mainT/CLOCKS_PER_SEC)%60) << " second(s)" << endl;
  wikiHow << "Main fired at a height of: " << curH-initH << "m;" << endl;
  mainFired = 1;
  digitalWrite(mainPin, HIGH);
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

        // Test for corrupted data
        if(altitude > 10000)
           altitude = prevAlt;
        else
           prevAlt = altitude;
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
  
  pinMode(drougePin, OUTPUT);
  pinMode(mainPin, OUTPUT);
  pinMode(beepPin, OUTPUT);
  
  softToneCreate(beepPin);
  

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
  
  for(int i = 0; i<4; i++){
      beep(440);
      usleep(500000);
      beep(0);
      usleep(500000);
  }
}

Jimbo::~Jimbo(){}
