#include "GPIOClass.h"

using namespace std;

int main (void)
{

    string inputstate;
    int i = 0;
    GPIOClass* gpio4 = new GPIOClass("4"); //create new GPIO object to be attached to  GPIO4
    GPIOClass* gpio5 = new GPIOClass("5"); //create new GPIO object to be attached to  GPIO17

    cout << " GPIO pins exported" << endl;

    gpio5->setdir_gpio("out"); //GPIO5 set to output
    gpio4->setdir_gpio("out"); // GPIO4 set to input

    cout << " Set GPIO pin directions" << endl;

    while(i < 20)
    {
        usleep(500000);
        usleep(20000);
        cout << "input pin state is definitely \"Pressed\". Turning LED ON" <<endl;
        gpio4->setval_gpio("1"); // turn LED ON
        usleep(2000000);
        gpio4->setval_gpio("0");
        gpio5->setval_gpio("1");
        usleep(2000000);
        gpio5->setval_gpio("0");
        i++;
    }

    cout << "Releasing heap memory and exiting....." << endl;
    delete gpio4;
    delete gpio5;
    gpio4 = NULL;
    gpio5 = NULL;

    return 0;
}
