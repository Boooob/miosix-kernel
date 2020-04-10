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


#ifndef LCD2004_I2C_H
#define LCD2004_I2C_H

#include "stm32f2_f4_i2c.h"
#include "interfaces/gpio.h"

// commands
#define CMD_CLEAR 0x01
#define CMD_RETURNHOME 0x02
#define CMD_ENTRYMODESET 0x04
#define CMD_DISPLAYCONTROL 0x08
#define CMD_CURSORSHIFT 0x10
#define CMD_FUNCTIONSET 0x20
#define CMD_SETCGRAMADDR 0x40
#define CMD_SETDDRAMADDR 0x80

// flags for display entry mode
#define ENTRYMODE_RIGHT 0x00
#define ENTRYMODE_LEFT 0x02
#define ENTRYMODE_SHIFTINCREMENT 0x01
#define ENTRYMODE_SHIFTDECREMENT 0x00

// flags for display on/off control
#define CNTRL_DISPLAYON 0x04
#define CNTRL_DISPLAYOFF 0x00
#define CNTRL_CURSORON 0x02
#define CNTRL_CURSOROFF 0x00
#define CNTRL_BLINKON 0x01
#define CNTRL_BLINKOFF 0x00

// flags for display/cursor shift
#define SHIFT_DISPLAYMOVE 0x08
#define SHIFT_CURSORMOVE 0x00
#define SHIFT_MOVERIGHT 0x04
#define SHIFT_MOVELEFT 0x00

// flags for function set
#define FUNCTION_8BITMODE 0x10
#define FUNCTION_4BITMODE 0x00
#define FUNCTION_2LINE 0x08
#define FUNCTION_1LINE 0x00
#define FUNCTION_5x10DOTS 0x04
#define FUNCTION_5x8DOTS 0x00

// flags for backlight control
#define BACKLIGHT_ON 0x08
#define BACKLIGHT_OFF 0x00

#define En 0b00000100  // Enable bit
#define Rw 0b00000010  // Read/Write bit
#define Rs 0b00000001  // Register select bit

namespace miosix{
    
    /**
     Driver for Liquid Crystal Display 2004, connected to the board through the I2C bus.
     */
    class Lcd2004{
        
    public:
        
        /**
         * Constructor. Requires two GpioPin objects already created by the caller. 
         * Refer to the board manual to retrieve the correct list of pins that 
         * provide I2C function
         * @param scl the gpio pin that will work as Serial Clock for the I2C 
         *  protocol. Refer to the board manual to retrieve a correct pin that 
         *  can provide this function
         * @param sda the gpio pin that will work as Serial Data for the I2C 
         *  protocol. Refer to the board manual to retrieve a correct pin that 
         *  can provide this function
         * @param address device address. Refer to the display manual to retrieve this.
         * @param columns   number of columns of the display
         * @param rows number of rows of the display
         */
        Lcd2004 (GpioPin scl, GpioPin sda, unsigned char address, int columns, int rows);
        
        /**
         * Clear the display
         * @return true on success, false on failure
         */
        bool clear();
        
        /**
         * Set cursor position to zero 
         * @return true on success, false on failure
         */
        bool home();
        
        /**
         * Set the cursor position. Columns and rows are numbered starting from 1
         * @param column the number of the column, from 1 to (columns number)
         * @param row the number of the row, from 1 to (rows number)
         * @return true on success, false on failure
         */
        bool setPosition(int column, int row);
        
        /**
         * Write a single character in the current cursor position
         * @param c the character to write
         * @return true on success, false on failure
         */
        bool writeChar(char c);
        
        
        /**
         * Write a line
         * @param text text to write
         * @param row the number of the row to write in, from 1 to 4. If out of 
         * bounds, it will be set to the closest bound
         * @param len the number of character to write. If not specified, it will 
         *        be considered the length of the string
         * @return true on success, false on failure
         */
        bool writeLine(const char* text, int row, int len=-1);
        
        /**
         * Turn on the display
         * @return true on success, false on failure
         */
        bool displayOn();
        
        /**
         * Turn off the display
         * @return true on success, false on failure
         */
        bool displayOff();
        
        /**
         * Turn on the backlight
         * @return true on success, false on failure
         */
        bool backlightOn();
        
        /**
         * Turn off the backlight
         * @return true on success, false on failure
         */
        bool backlightOff();
        
        /**
         * Turn on the cursor
         * @return true on success, false on failure
         */
        bool cursorOn();
        
        /**
         * Turn off the cursor
         * @return true on success, false on failure
         */
        bool cursorOff();
        
        /**
         * Turn on the blinking cursor
         * @return true on success, false on failure
         */
        bool blinkOn();
        
        /**
         * Turn off the blinking cursor
         * @return true on success, false on failure
         */
        bool blinkOff();
        
        /**
         * Scroll the display right. 
         * Keep note that row addresses may not be ordered like the rows, 
         * so a character at the end of a line could shift in a row different 
         * from the following one.
         * @return true on success, false on failure
         */
        bool scrollRight();
        
        /**
         * Scroll the display left. 
         * Keep note that row addresses may not be ordered like the rows, 
         * so a character at the start of a line could shift in a row different 
         * from the previous one.
         * @return true on success, false on failure
         */
        bool scrollLeft();
        
        /**
         * Set the entry mode to right-to-left
         * @return true on success, false on failure
         */
        bool entryRight();
        
        /**
         * Set the entry mode to left-to-right
         * @return true on success, false on failure
         */
        bool entryLeft();
       
        /**
         * Align all the text on the display right from the cursor.
         * This command shifts all the rows, so text already written will
         * shift accordingly 
         * @return true on success, false on failure
         */
        bool alignRight();
        
        /**
         * Align all the text left from the cursor.
         * This command shifts all the rows, so text already written will
         * shift accordingly
         * @return true on success, false on failure
         */
        bool alignLeft();
        
    private:
        
        /**
         * Initializes the peripheral
         */
        void init();
        
        /**
         * Send a command
         * @param command the command to send
         * @return true on success, false on failure
         */
        bool sendCommand(unsigned char command);
        
        /**
         * Send 8 bit of data
         * @param data data to send
         * @param mode mode in which the receiver will interpret data
         * @return true on success, false on failure
         */
        bool send8bits(unsigned char data, unsigned char mode);
        
        /**
         * Send 4 data bit
         * @param data data to send
         * @return true on success, false on failure
         */
        bool send4bits(unsigned char data);
        
        /**
         * Send enable signal
         * @param data data to enable
         * @return true on success, false on failure
         */
        bool sendEnable(unsigned char data);
        
        /**
         * Send generic data to the device
         * @param value the data to send
         * @param mode  the mode the data will be interpreted by the device
         * @return true on success, false on failure
         */
        bool send(unsigned char value);
       
        

        unsigned char address;
        int columns;
        int rows;
        unsigned char function;
        unsigned char control;
        unsigned char entry_mode;
        unsigned char backlight;
        
    };
}

#endif /* LCD2004_I2C_H */

