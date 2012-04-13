/*
  LedControl.h - A library for controling Led Digits or 
  Led arrays with a MAX7219/MAX7221
  Copyright (c) 2007 Eberhard Fahle

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef LedControl_h
#define LedControl_h

#include <WConstants.h>

/*
 * Here are the segments to be switched on for characters and digits on
 * 7-Segment Displays
 */
const static byte charTable[128] = {
    B01111110,B00110000,B01101101,B01111001,B00110011,B01011011,B01011111,B01110000,
    B01111111,B01111011,B01110111,B00011111,B00001101,B00111101,B01001111,B01000111,
    B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
    B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
    B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
    B00000000,B00000000,B00000000,B00000000,B10000000,B00000001,B10000000,B00000000,
    B01111110,B00110000,B01101101,B01111001,B00110011,B01011011,B01011111,B01110000,
    B01111111,B01111011,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
    B00000000,B01110111,B00011111,B00001101,B00111101,B01001111,B01000111,B00000000,
    B00110111,B00000000,B00000000,B00000000,B00001110,B00000000,B00000000,B00000000,
    B01100111,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
    B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00001000,
    B00000000,B01110111,B00011111,B00001101,B00111101,B01001111,B01000111,B00000000,
    B00110111,B00000000,B00000000,B00000000,B00001110,B00000000,B00000000,B00000000,
    B01100111,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
    B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000
};

class LedControl {
    private :
    /* We keep track of the led-status for 8 devices in this array */
    byte status[64];
    /* This is the array we send the data with 2 commandbytes for 8 devices */
    byte spidata[16];
    /* Data is shifted out of this pin*/
    int SPI_MOSI;
    /* The clock is signaled on this pin */
    int SPI_CLK;
    /* This one is driven LOW for chip selectzion */
    int SPI_CS;
    /* The maximum number of devices to be used */
    int maxDevices;
    /* Send out a single command to the device */
    void spiTransfer(int addr, byte opcode, byte data);
    
    public:
    /* 
     * Create a new controler 
     * Params :
     * int dataPin	The pin on the Arduino where data gets shifted out
     * int clockPin	The pin for the clock
     * int csPin	The pin for selecting the device when data is to be sent
     * int numDevices	The maximum number of devices that can be controled
     */
    LedControl(int dataPin, int clkPin, int csPin, int numDevices);

    /*
     * Gets the maximum number of devices attached to
     * this LedControl.
     * Returns :
     * int the number of devices attached to this LedControl
     */
    int getDeviceCount();

    /* 
     * Set the shutdown (power saving) mode for the device
     * Params :
     * int addr		The address of the display to control
     * boolean b	If true the device goes into power-down mode. If false
     *			device goes into normal operation
     */
    void shutdown(int addr, bool b);

    /* 
     * Set the number of digits (or rows) to be displayed.
     * See datasheet for sideeffects of the scanlimit on the brightness
     * of the display.
     * Params :
     * int addr		The address of the display to control
     * int limit The number of digits to be displayed
     * Only values between 0 (only 1 digit) and 7 (all digits) are valid.
     */
    void setScanLimit(int addr, int limit);

    /* 
     * Set the brightness of the display.
     * Params:
     * int addr		The address of the display to control
     * int intensity the brightness of the display. 
     * Only values between 0(darkest) and 15(brightest) are valid.
     */
    void setIntensity(int addr, int intensity);

    /* 
     * Switch all Leds on the display off. 
     * Params:
     * int addr		The address of the display to control
     */
    void clearDisplay(int addr);

    /* 
     * Set the status for a specific Led.
     * Params :
     * int addr		The address of the display to control
     * int row		the row in which the led is located 
     *			Only values between 0 and 7 are valid.
     * int col		the column in which the led is located
     *			Only values between 0 and 7 are valid.
     * boolean state	If true the led is switched on, if false it is switched off
     */
    void setLed(int addr, int row, int col, boolean state);

    /* 
     * Set the 8 Led's in a row to a new state
     * Params:
     * int addr		The address of the display to control
     * int row		The row on which the led's are to be set
     *			Only values between 0 and 7 are valid.
     * byte value	A bit set to 1 in this value will light up the
     *			corresponding led.
     */
    void setRow(int addr, int row, byte value);

    /* 
     * Set the 8 Led's in a column to a new state
     * Params:
     * int addr		The address of the display to control
     * int col		The column on which the led's are to be set
     *			Only values between 0 and 7 are valid.
     * byte value	A bit set to 1 in this value will light up the
     *			corresponding led.
     */
    void setColumn(int addr, int col, byte value);

    /* 
     * Display a hexadecimal digit on a 7-Segment Display
     * Params:
     * int addr		the address of the display to control
     * int digit	the position of the digit on the display
     * byte value	the value to be displayed. 
     *			Only values between 0x00 and 0x0F are valid.
     * boolean dp	If true also switches on the decimal point.
     */
    void setDigit(int addr, int digit, byte value, boolean dp);

    /* 
     * Display a character on a 7-Segment display.
     * The char that can be acutally displayed are obviously quite limited.
     * Here is the whole set :	
     *	'0','1','2','3','4','5','6','7','8','9','0',
     *  'A','b','c','d','E','F','H','L','P',
     *  '.','-','_',' ' 
     * Params:
     * int addr		the address of the display to control
     * int digit	the position of the character on the display
     * char value	the character to be displayed. (See the limited set above!) 
     * boolean dp	If true also switches on the decimal point.
     */
    void setChar(int addr, int digit, char value, boolean dp);
};

#endif	//LedControl.h



