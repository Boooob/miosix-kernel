#include "miosix.h"
#include "miosix/arch/common/drivers/lcd2004_i2c.h"
#include "miosix/interfaces/gpio.h"

using namespace std;
using namespace miosix;

int main()
{
    GpioPin scl(GPIOB_BASE, 6);
    GpioPin sda(GPIOB_BASE, 7);
    
    Lcd2004 lcd(scl, sda, 0x4e, 20, 4);
    
    lcd.writeLine("AOS Project 2020", 1);
    lcd.writeLine("Miosix OS LCD driver", 2);
    lcd.writeLine("Emanuele Pisano", 3);
    lcd.writeLine("Federico Terraneo", 4);
    
    for(;;)
    {
        ledOn();
        Thread::sleep(1000);
        ledOff();
        Thread::sleep(1000);   
    }
}