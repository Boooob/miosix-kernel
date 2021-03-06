/***************************************************************************
 *   Copyright (C) 2020 by Emanuele Pisano                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   As a special exception, if other files instantiate templates or use   *
 *   macros or inline functions from this file, or you compile this file   *
 *   and link it with other works to produce a work based on this file,    *
 *   this file does not by itself cause the resulting work to be covered   *
 *   by the GNU General Public License. However the source code for this   *
 *   file must still be made available in accordance with the GNU General  *
 *   Public License. This exception does not invalidate any other reasons  *
 *   why a work based on this file might be covered by the GNU General     *
 *   Public License.                                                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>   *
 ***************************************************************************/


#include "lcd2004_i2c.h"
#include "stm32f2_f4_i2c.h"
#include "interfaces/gpio.h"
#include "kernel/kernel.h"
#include <interfaces/delays.h>

namespace miosix{
    
    Lcd2004::Lcd2004(GpioPin scl, GpioPin sda, unsigned char address, int columns, int rows) : 
        address(address), columns(columns), rows(rows) 
    {
        //set alternate function to I2C
        scl.alternateFunction(4);
        sda.alternateFunction(4);

        //set pins to alternate function, open drain, as required by the device
        scl.mode(Mode::ALTERNATE_OD);
        sda.mode(Mode::ALTERNATE_OD);
        
        init();
    }
    
    void Lcd2004::init()
    {
        //initialization, see the reference manual for details 
        Thread::sleep(50);
        backlight = BACKLIGHT_ON;
        send(backlight);
        Thread::sleep(1000);
        send4bits(0x03 << 4);
        Thread::sleep(5);
        send4bits(0x03 << 4);
        Thread::sleep(5);
        send4bits(0x03 << 4);
        delayUs(200);
   
        //set to 4-bit interface
        send4bits(0x02 << 4);
        
        // set function
        function = FUNCTION_4BITMODE | FUNCTION_1LINE | FUNCTION_5x8DOTS;
        
        if(rows>1){
            function |= FUNCTION_2LINE;
        }
        
	sendCommand(CMD_FUNCTIONSET | function);
        
        //set control
        control = CNTRL_DISPLAYON | CNTRL_CURSOROFF | CNTRL_BLINKOFF;
	sendCommand(CMD_DISPLAYCONTROL | control);
        
        //display clear
        clear();
        
        //set entry mode
	entry_mode = ENTRYMODE_LEFT | ENTRYMODE_SHIFTDECREMENT;
        sendCommand(CMD_ENTRYMODESET | entry_mode);
	
        home();
        
        
    }
    
    bool Lcd2004::clear()
    {
        bool success = sendCommand(CMD_CLEAR);
	Thread::sleep(2);
        return success;
    }
    
    bool Lcd2004::home(){
        bool success = sendCommand(CMD_RETURNHOME);
        Thread::sleep(2);
        return success;
    }
    
    bool Lcd2004::setPosition(int column, int row){
        int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
        //check if row is in the valid range; if not, set it to a valid value
        if ( row == 0 ) {
            row = 1;
        }
        else if ( row > rows ) {
		row = rows;
	}
        return sendCommand(CMD_SETDDRAMADDR | (row_offsets[row-1]+column-1));
    }
    
    bool Lcd2004::writeChar(char c){
        return send8bits(c, Rs);
    }
    
    bool Lcd2004::writeLine(const char* text, int row, int len){
        bool success = true;
        if((entry_mode & ENTRYMODE_LEFT) == 0x00) success = setPosition(columns, row);    //if the entry mode is set to right-to-left or adjusted right, the line is writed from the end
        else if((entry_mode & ENTRYMODE_SHIFTINCREMENT) != 0x00) success = setPosition(columns+1, row);
        else success = setPosition(1, row);                                          //else, the line is written from the start, as usual
        int i=0;
        if(len==-1){
            while(text[i]!='\n' && text[i]!='\0'){
            if(!send8bits(*(text+i), Rs)) success = false;
            i++;
            }
        }
        else{
            if(len<0) len=0;
            for(i=0; i<len ; i++){
                if(!send8bits(*(text+i), Rs)) success = false;
            }
        }
        return success;
    }
    
    bool Lcd2004::displayOn(){
        control |= CNTRL_DISPLAYON;
        return sendCommand(CMD_DISPLAYCONTROL | control);
    }
    
    bool Lcd2004::displayOff(){
        control &= ~CNTRL_DISPLAYON;
        return sendCommand(CMD_DISPLAYCONTROL | control);
    }
    
    bool Lcd2004::backlightOn(){
        backlight = BACKLIGHT_ON;
        return send(0x00);
    }
    
    bool Lcd2004::backlightOff(){
        backlight = BACKLIGHT_OFF;
        return send(0x00);
    }
    
    bool Lcd2004::cursorOn(){
        control |= CNTRL_CURSORON;
        return sendCommand(CMD_DISPLAYCONTROL | control);
    }
    
    bool Lcd2004::cursorOff(){
        control &= ~CNTRL_CURSORON;
        return sendCommand(CMD_DISPLAYCONTROL | control);
    }
    
    bool Lcd2004::blinkOn(){
        control |= CNTRL_BLINKON;
        return sendCommand(CMD_DISPLAYCONTROL | control);
    }
    
    bool Lcd2004::blinkOff(){
        control &= ~CNTRL_BLINKON;
        return sendCommand(CMD_DISPLAYCONTROL | control);
    }
    
    bool Lcd2004::scrollRight(){
        return sendCommand(CMD_CURSORSHIFT | SHIFT_DISPLAYMOVE | SHIFT_MOVERIGHT);
    }
    
    bool Lcd2004::scrollLeft(){
        return sendCommand(CMD_CURSORSHIFT | SHIFT_DISPLAYMOVE | SHIFT_MOVELEFT);
    }
    
    bool Lcd2004::entryRight(){
        entry_mode &= ~ENTRYMODE_LEFT;
	return sendCommand(CMD_ENTRYMODESET | entry_mode);
    }
    
    bool Lcd2004::entryLeft(){
        entry_mode |= ENTRYMODE_LEFT;
	return sendCommand(CMD_ENTRYMODESET | entry_mode);
    }
    
    bool Lcd2004::alignRight(){
        entry_mode |= ENTRYMODE_SHIFTINCREMENT;
	return sendCommand(CMD_ENTRYMODESET | entry_mode);
    }
    
    bool Lcd2004::alignLeft(){
        entry_mode &= ~ENTRYMODE_SHIFTINCREMENT;
	return sendCommand(CMD_ENTRYMODESET | entry_mode);
    }
    
    bool Lcd2004::sendCommand(unsigned char command)
    {
        return send8bits(command, 0x00);
    }
    
    
    
    bool Lcd2004::send8bits(unsigned char data, unsigned char mode)
    {
        unsigned char first4bits = data & 0xf0;
        unsigned char last4bits = (data<<4) & 0xf0;
        bool success = true;
        if(!send4bits(first4bits|mode)) success = false;
        if(!send4bits(last4bits|mode)) success = false;
        return success;
    }
    
    bool Lcd2004::send4bits(unsigned char data)
    {
        bool success = true;
        if(!send(data)) success = false;
        if(!sendEnable(data)) success = false;
        return success;
    }
    
    bool Lcd2004::sendEnable(unsigned char data)
    {
        bool success = true;
        if(!send(data | En)) success = false;
        delayUs(1);
        if(!send(data & ~En)) success = false;
        delayUs(50);
        return success;
    }
    
    bool Lcd2004::send(unsigned char value)
    {
        unsigned char toSend = value | backlight;   //put the bit 5 to the correct value (1 if backlight on, 0 if off)
        return I2C1Driver::instance().send(address, &toSend, 1);
    }
    
}