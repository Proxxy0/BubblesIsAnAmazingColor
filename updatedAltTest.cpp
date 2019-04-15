#include "AnthonyTimothy.h"
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

using namespace std;

int main(){
  AnthonyTimothy tonyTim;
  
  for(int i = 0; i<1000; i++){
    tonyTim.gatherThings();
    cout << tonyTim.getAlt() << endl;
  }
}