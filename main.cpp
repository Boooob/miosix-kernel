
#include <cstdio>
#include "miosix.h"
#include "miosix/arch/common/drivers/lcd2004_i2c.h"
#include "miosix/interfaces/gpio.h"

using namespace std;
using namespace miosix;

int main()
{
    //iprintf("Hello world, write your application here\n");
    /**
    for(;;)
    {
        ledOn();
        Thread::sleep(500);
        ledOff();
        Thread::sleep(500);
        ledOn();
        Thread::sleep(1000);
        ledOff();
        Thread::sleep(1000);
        
        
    }
     * */
    GpioPin scl(GPIOB_BASE, 6);
    GpioPin sda(GPIOB_BASE, 7);
    
    //set alternate function to I2C
    scl.alternateFunction(4);
    sda.alternateFunction(4);
    
    scl.mode(Mode::ALTERNATE_OD);
    sda.mode(Mode::ALTERNATE_OD);
    
    scl.speed(Speed::_100MHz);
    sda.speed(Speed::_100MHz);
    
    Lcd2004 lcd(0x4e, 20, 4); //0x4e
    
    lcd.turnOn();
    if(lcd.write("ciao papi!", 10, 1)) ledOn();
    /**
    for(unsigned char address = 0x40; address < 0x50; address++){
        Lcd2004 lcd(address, 20, 4); //0x4e
    
        if(lcd.write("ciao papi!", 10, 1)){
            for(unsigned char i=0; i<(address-64); i++){
                ledOn();
                Thread::sleep(500);
                ledOff();
                Thread::sleep(500);
            }
        }
    }
     * */
    
    Thread::sleep(5000);
    for(;;)
    {
        ledOn();
        Thread::sleep(500);
        ledOff();
        Thread::sleep(500);
        ledOn();
        Thread::sleep(1000);
        ledOff();
        Thread::sleep(1000);
        
        
    }
}