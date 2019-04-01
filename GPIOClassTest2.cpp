#include "GPIOClass.h"

using namespace std;

int main (void)
{

    string inputstate;
    int i = 0;
    GPIOClass* gpio6 = new GPIOClass("6"); //create new GPIO object to be attached to  GPIO6
    GPIOClass* gpio5 = new GPIOClass("5"); //create new GPIO object to be attached to  GPIO5

    cout << " GPIO pins exported" << endl;

    gpio5->setdir_gpio("out"); //GPIO5 set to output
    gpio6->setdir_gpio("out"); // GPIO6 set to input

    cout << " Set GPIO pin directions" << endl;

    while(i < 20)
    {
        usleep(500000);
        usleep(20000);
        cout << "input pin state is definitely \"Pressed\". Turning LED ON" <<endl;
        gpio6->setval_gpio("1"); // turn LED ON
        usleep(2000000);
        gpio6->setval_gpio("0");
        gpio5->setval_gpio("1");
        usleep(2000000);
        gpio5->setval_gpio("0");
        i++;
    }

    cout << "Releasing heap memory and exiting....." << endl;
    delete gpio6;
    delete gpio5;
    gpio6 = NULL;
    gpio5 = NULL;

    return 0;
}
