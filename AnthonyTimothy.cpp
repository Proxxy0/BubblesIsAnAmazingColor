#include "AnthonyTimothy.h"

using namespace std;

void AnthonyTimothy::altMode(){
  // Select control register(0x26)
	// Active mode, OSR = 128, altimeter mode(0xB9)
  config[0] = 0x26;
  config[1] = 0xB9;
  write(file, config, 2);
}

void AnthonyTimothy::altDataReady(){
  // Select data configuration register(0x13)
  // Data ready event enabled for altitude, pressure, temperature(0x07)
  config[0] = 0x13;
  config[1] = 0x07;
  write(file, config, 2);
}

void AnthonyTimothy::altActiveMode(){
  // Select control register(0x26)
  // Active mode, OSR = 128, altimeter mode(0xB9)
  config[0] = 0x26;
  config[1] = 0xB9;
  write(file, config, 2);
  //sleep(1);
}

void AnthonyTimothy::altReadData(){
  // Read 6 bytes of data from address 0x00(00)
  // status, tHeight msb1, tHeight msb, tHeight lsb, temp msb, temp lsb
  write(file, reg, 1);
  if(read(file, data, 6) != 6)
  {
    printf("Error : Input/Output error \n");
  }
}

void AnthonyTimothy::altConvertData(){
  // Convert the data
  tHeight = ((data[1] * 65536) + (data[2] * 256 + (data[3] & 0xF0)) / 16);
  temp = ((data[4] * 256) + (data[5] & 0xF0)) / 16;
  altitude = tHeight / 16.0;
  cTemp = (temp / 16.0);
  fTemp = cTemp * 1.8 + 32;
}

void AnthonyTimothy::barMode(){
  // Select control register(0x26)
  // Active mode, OSR = 128, barometer mode(0x39)
  config[0] = 0x26;
  config[1] = 0x39;
  write(file, config, 2);
  //sleep(1);
}

void AnthonyTimothy::barReadData(){
  // Read 4 bytes of data from register(0x00)
  // status, pres msb1, pres msb, pres lsb
  reg[0] = 0x00;
  write(file, reg, 1);
  read(file, data, 4);
}

void AnthonyTimothy::barConvertData(){
  // Convert the data to 20-bits
  int pres = ((data[1] * 65536) + (data[2] * 256 + (data[3] & 0xF0))) / 16;
  float pressure = (pres / 4.0) / 1000.0;
}

void AnthonyTimothy::initialize(){
  // Create I2C bus
  int file;
  char *bus = "/dev/i2c-1";
  if((file = open(bus, O_RDWR)) < 0)
  {
    printf("Failed to open the bus. \n");
  }
  // Get I2C device, TSL2561 I2C address is 0x60(96)
	ioctl(file, I2C_SLAVE, 0x60);
}

float AnthonyTimothy::getPres(){
  return pressure;
}

float AnthonyTimothy::getAlt(){
  return altitude;
}

float AnthonyTimothy::getCTemp(){
  return cTemp;
}

float AnthonyTimothy::getFTemp(){
  return fTemp;
}

void AnthonyTimothy::gatherThings(){
  file = 0;
  for(int i=0; i<2; i++){
    config[i]=0;
  }
  for(int i=0; i<1; i++){
    reg[i]=0x00;
  }
  for(int i=0; i<6; i++){
    data[i]=0;
  }
  tHeight = 0;
  temp = 0;
  altitude = 0;
  cTemp = 0;
  fTemp = 0;
  pres = 0;
  pressure = 0;
  
  initialize();
  
  altMode();
  altDataReady();
  altActiveMode();
  altReadData();
  altConvertData();
  
  barMode();
  barReadData();
  barConvertData();
}
  
AnthonyTimothy::AnthonyTimothy(){}

AnthonyTimothy::~AnthonyTimothy(){}