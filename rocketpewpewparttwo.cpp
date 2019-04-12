
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
#include "Jimbo.h"

using namespace std;


//##############################################################################
//main function
//##############################################################################
int main(){
  Jimbo tonyTim;
  cout << "initialization complete!" << endl;

  //run for 10 seconds
  while(tonyTim.getTotTime() < 3){
    cout << tonyTim.getTotTime() << endl;
    tonyTim.updateAll();
  }
  tonyTim.endFlight();
  if(tonyTim.landed){
     return 0;
  }
  return 0;
}
