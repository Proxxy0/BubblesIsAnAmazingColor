#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

class AnthonyTimothy{
  private:
    int file;
    char *bus = "/dev/i2c-1";
    char config[2]={0};
    char reg[1]={0x00};
    char data[6] = {0};
    
    int tHeight;
    int temp;
    
    float altitude;
    float cTemp;
    float fTemp;
    
    int pres;
    float pressure;
    
    
    altMode();
    altDataReady();
    altActiveMode();
    altReadData();
    altConvertData();
    
    barMode();
    barReadData();
    barConvertData();
    
    initialize();
    
  public:
    getPres();
    getAlt();
    getCTemp();
    getFTemp();
    
    gatherThings();
    
    AnthonyTimothy();
    ~AnthonyTimothy();
};