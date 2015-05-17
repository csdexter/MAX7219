/* Arduino MAX7219/7221 Library
 * See the README file for author and licensing information. In case it's
 * missing from your distribution, use the one here as the authoritative
 * version: https://github.com/csdexter/MAX7219/blob/master/README
 *
 * This library is for use with Maxim's MAX7219 and MAX7221 LED driver chips.
 * Austria Micro Systems' AS1100/1106/1107 is a pin-for-pin compatible and is
 * also supported, including its extra functionality in register 0xE.
 * See the example sketches to learn how to use the library in your code.
 *
 * This is the main include file for the library.
 * ---------------------------------------------------------------------------
 * The header of the original file follows:
 *
 * LedControl.h - A library for controling Leds with a MAX7219/MAX7221
 * Copyright (c) 2007 Eberhard Fahle
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _MAX7219_H_INCLUDED
#define _MAX7219_H_INCLUDED

#if defined(ARDUINO) && ARDUINO >= 100
# include <Arduino.h>
#else
# include <WProgram.h>
#endif

//Assign the SPI pin numbers
//DIN and CLK always connected to MOSI and SCK
#define MAX7219_PIN_LOAD SS

//Define MAX7219 Register codes
#define MAX7219_REG_NOOP 0x00
#define MAX7219_REG_DIGIT0 0x01
#define MAX7219_REG_DIGIT1 0x02
#define MAX7219_REG_DIGIT2 0x03
#define MAX7219_REG_DIGIT3 0x04
#define MAX7219_REG_DIGIT4 0x05
#define MAX7219_REG_DIGIT5 0x06
#define MAX7219_REG_DIGIT6 0x07
#define MAX7219_REG_DIGIT7 0x08
#define MAX7219_REG_DECODEMODE 0x09
#define MAX7219_REG_INTENSITY 0x0A
#define MAX7219_REG_SCANLIMIT 0x0B
#define MAX7219_REG_SHUTDOWN 0x0C
//MAX7219 prefix for consistency, even if AS1100 specific
#define MAX7219_REG_RESETCLOCK 0x0E
//MAX7219 prefix for consistency, even if AS1106/1107 specific
#define MAX7219_REG_FEATURE 0x0E
#define MAX7219_REG_DISPLAYTEST 0x0F

//Define MAX7219 Flags
#define MAX7219_FLG_SEGDP 0x80
#define MAX7219_FLG_SEGA 0x40
#define MAX7219_FLG_SEGB 0x20
#define MAX7219_FLG_SEGC 0x10
#define MAX7219_FLG_SEGD 0x08
#define MAX7219_FLG_SEGE 0x04
#define MAX7219_FLG_SEGF 0x02
#define MAX7219_FLG_SEGG 0x01
#define MAX7219_FLG_DIGIT0_RAW 0x00
#define MAX7219_FLG_DIGIT0_CODEB 0x01
#define MAX7219_FLG_DIGIT0_DECODE 0x01
#define MAX7219_FLG_DIGIT1_RAW 0x00
#define MAX7219_FLG_DIGIT1_CODEB 0x02
#define MAX7219_FLG_DIGIT1_DECODE 0x02
#define MAX7219_FLG_DIGIT2_RAW 0x00
#define MAX7219_FLG_DIGIT2_CODEB 0x04
#define MAX7219_FLG_DIGIT2_DECODE 0x04
#define MAX7219_FLG_DIGIT3_RAW 0x00
#define MAX7219_FLG_DIGIT3_CODEB 0x08
#define MAX7219_FLG_DIGIT3_DECODE 0x08
#define MAX7219_FLG_DIGIT4_RAW 0x00
#define MAX7219_FLG_DIGIT4_CODEB 0x10
#define MAX7219_FLG_DIGIT4_DECODE 0x10
#define MAX7219_FLG_DIGIT5_RAW 0x00
#define MAX7219_FLG_DIGIT5_CODEB 0x20
#define MAX7219_FLG_DIGIT5_DECODE 0x20
#define MAX7219_FLG_DIGIT6_RAW 0x00
#define MAX7219_FLG_DIGIT6_CODEB 0x40
#define MAX7219_FLG_DIGIT6_DECODE 0x40
#define MAX7219_FLG_DIGIT7_RAW 0x00
#define MAX7219_FLG_DIGIT7_CODEB 0x80
#define MAX7219_FLG_DIGIT7_DECODE 0x80
#define MAX7219_FLG_SHUTDOWN 0x01
//MAX7219 prefix for consistency, even if AS1106/1107 specific
#define MAX7219_FLG_SAVEFEATURE 0x80
#define MAX7219_FLG_DISPLAYTEST 0x01
//MAX7219 prefix for consistency, even if AS1100/1106/1107 specific
#define MAX7219_FLG_EXTERNAL_CLOCK 0x01
//MAX7219 prefix for consistency, even if AS1100/1106/1107 specific
#define MAX7219_FLG_RESET 0x02
//MAX7219 prefix for consistency, even if AS1106/1107 specific
#define MAX7219_FLG_DECODE_CODEB 0x00
//MAX7219 prefix for consistency, even if AS1106/1107 specific
#define MAX7219_FLG_DECODE_HEX 0x04
//MAX7219 prefix for consistency, even if AS1106 specific
#define MAX7219_FLG_ENABLE_SPI 0x08
//MAX7219 prefix for consistency, even if AS1106/1107 specific
#define MAX7219_FLG_ENABLE_BLINK 0x10
//MAX7219 prefix for consistency, even if AS1106/1107 specific
#define MAX7219_FLG_BLINK_FREQ1 0x00
//MAX7219 prefix for consistency, even if AS1106/1107 specific
#define MAX7219_FLG_BLINK_FREQ2 0x20
//MAX7219 prefix for consistency, even if AS1106/1107 specific
#define MAX7219_FLG_BLINK_SYNC 0x40
//MAX7219 prefix for consistency, even if AS1106/1107 specific
#define MAX7219_FLG_BLINK_START_OFF 0x00
//MAX7219 prefix for consistency, even if AS1106/1107 specific
#define MAX7219_FLG_BLINK_START_ON 0x80

//Define per-digit operation modes
#define MAX7219_MODE_7SEGMENT 0x01
#define MAX7219_MODE_MATRIX 0x02
#define MAX7219_MODE_BARGRAPH 0x03
#define MAX7219_MODE_16SEGMENT 0x04
//Don't touch this digit
#define MAX7219_MODE_OFF 0xFD
//Don't scan this digit
#define MAX7219_MODE_NC 0xFE

//Define broadcast flag
#define MAX7219_CHIP_ALL 0xFF

typedef struct {
    byte elementType;
    byte chipFrom, digitFrom;
    byte chipTo, digitTo;
} MAX7219_Topology;

#define MAX7219_DEFAULT_TOPOLOGY(x) x->elementType = MAX7219_MODE_7SEGMENT, \
                                    x->chipFrom = 0, x->digitFrom = 0, \
                                    x->chipTo = 0, x->digitTo = 7
#define MAX7219_DEFAULT_LENGTH 1

class MAX7219 
{
    public:
        /*
        * Description:
        *   This is the constructor, it creates a new MAX7219 driver chain.
        *   You will need one instance of this class for each string of chips
        *   which share a LOAD/#CS pin.
        * Parameters:
        *   pinLOAD - digital pin to which LOAD/#CS is wired to, defaults to
        *             SPI SS
        */
        MAX7219(byte pinLOAD = MAX7219_PIN_LOAD) { _pinLOAD = pinLOAD; };

        /*
        * Description:
        *   This is the destructor, it simply calls end().
        */
        ~MAX7219() { end(); };

        /*
        * Description:
        *   Sets the topology of the display.
        * Parameters:
        *   topology - topology to use, ignore for defaults
        *   length   - number of topology elements described
        */
        void begin(const MAX7219_Topology *topology = NULL, 
                   const byte length = 1);

        /*
        * Description:
        *    Clears the SRAM and sends a shutdown command to the MAX7219(s).
        */
        void end(void);

        /*
        * Description:
        *   Gets the total number of devices attached to this driver, as
        *   extrapolated from the current topology.
        */
        byte getChipCount(void) { return _chips; };

        /*
        * Description:
        *   Sets the selected chip to shutdown/powered mode
        * Parameters:
        * 	chip - the index of the chip to control
        *       saveFR - [AS1106/1107] save (do not reset) the feature register
        */
        void shutdown(byte chip = 0, boolean saveFR = false) {
            writeRegister(MAX7219_REG_SHUTDOWN,
                          (saveFR ? MAX7219_FLG_SAVEFEATURE : 0x00), chip);
        };
        void noShutdown(byte chip = 0, boolean saveFR = false) {
            writeRegister(MAX7219_REG_SHUTDOWN,
                          (saveFR ? MAX7219_FLG_SAVEFEATURE : 0x00) |
                          MAX7219_FLG_SHUTDOWN, chip);
        };

        /*
        * Description:
        *   Sets the selected chip to display test/normal mode
        * Parameters:
        * 	chip - the index of the chip to control
        */
        void displayTest(byte chip = 0) {
            writeRegister(MAX7219_REG_DISPLAYTEST, MAX7219_FLG_DISPLAYTEST, 
                          chip);
        };
        void noDisplayTest(byte chip = 0) {
            writeRegister(MAX7219_REG_DISPLAYTEST, 0x00, chip);
        };

        /*
        * Description:
        *   Set the number of digits, bargraph columns or matrix rows to be
        *   scanned by the selected chip. See datasheet for side effects of
        *   the scan limit on the brightness of the display.
        * Parameters:
        *   limit - number of digits to be scanned-1 (0..7)
        *   chip  - the index of the chip to control
        */
        void setScanLimit(byte limit, byte chip = 0) {
            writeRegister(MAX7219_REG_SCANLIMIT, limit, chip);
        };

        /*
        * Description:
        *   Set the brightness of the display.
        * Parameters:
        *   intensity - the brightness of the display (0..15)
        *   addr      - the index of the chip to control
        */
        void setIntensity(byte intensity, byte chip = 0) {
            writeRegister(MAX7219_REG_INTENSITY, intensity, chip);
        };

        /*
        * Description:
        *   [AS1100/1106/1107] Control the feature register.
        */
        void setFeatureRegister(byte flags, byte chip = 0) {
            writeRegister(MAX7219_REG_FEATURE, flags, chip);
        };

        /*
        * Description:
        *   Switch all LEDs belonging to the given topology element off. 
        * Parameters:
        *   topo - the index of the topology element to switch off
        */
        void clearDisplay(byte topo = 0);

        /*
        * Description:
        *   Reset topology element to a meaningful zero. This involves
        *   displaying an actual 0 on 7-segment displays, turning only the
        *   first bottom line on on bargraphs and displaying just a single
        *   pixel in the bottom left corner on matrices.
        */
        void zeroDisplay(byte topo = 0);

        /*
        * Description:
        *   Displays the given number on the given topology element, previously
        *   configured as a 7-segment display.
        * Parameters:
        *   number - [0-9-EeHhLlPp ]. Set bit 7 on any character whose
        *            corresponding digit should have DP on.
        *   topo   - topology element to update (must be 7-segment)
        *   mirror - format output in reverse, i.e. '0123' is displayed as
        *            '3210'. This is meant for the sad cases when you haven't
        *            read the README before sending out your gerbers to the fab.
        */
        void set7Segment(const char *number, byte topo = 0,
                         bool mirror = false);

        /*
        * Description:
        *   Displays the givent text on the given topology element using the
        *   specified font that starts at character fontStart. Intended for use
        *   with 14- and 16-segment displays.
        * Parameters:
        *   text - <any character that font provides>. Depending on the topology
        *          element type, characters such as "." (period) or "'"
        *          (apostrophe) may display as stand-alone glyphs or may be
        *          rendered by DPs in adjacent digits. If that is the case, by
        *          display industry convention, a period displays on the
        *          previous character and an apostrophe on the next.
        *   topo - topology element to update.
        *   font - a pointer to an array of words containing the font to be
        *          used, assumed to reside in FLASH.
        *   fcif - the first character described by the font, used as a base
        *          offset against all characters in text.
        */
        void setFromFont(const char *text, byte topo, const word *font,
                         char fcif);

        /*
        * Description:
        *   Displays the given text on the given topology element, previously
        *   configured as a 16-segment display.
        * Parameters:
        *   text - [!-~ ]
        *   topo - topology element to update (must be 16-segment)
        */
        void set16Segment(const char *text, byte topo = 0);

        /*
        * Description:
        *   Displays the given bar/dot values on the given topology element,
        *   previously configured as a bargraph display.
        * Parameters:
        *   values - [0, 8]
        *   dot    - use dot instead of bar mode
        *   topo   - topology element to update (must be bargraph)
        */
        void setBarGraph(const byte *values, boolean dot = false, 
                         byte topo = 0);

        /*
        * Description:
        *   Displays the given pixel values on the given topology element,
        *   previously configured as a matrix.
        * Parameters:
        *   values - [0, 0xFF]
        *   topo   - topology element to update (must be matrix)
        */
        void setMatrix(const byte *values, byte topo = 0);

    private:
        const MAX7219_Topology *_topology;
        byte _pinLOAD, _elements, _chips;
        boolean _isAS1100;

        /*
        * Description:
        *   Write to one of the chip registers, on a single chip, via SPI.
        */
        void writeRegister(byte addr, byte value, byte chip = 0);

        /*
        * Description:
        *   Write to one of the chip registers, on multiple chips, via SPI.
        * Parameters:
        *   registers - data to be written
        *   size      - length of data to be written
        *   chip      - chip index to start writing at
        */
        void writeRegisters(const word *registers, byte size, byte chip = 0);
        /*
        * Descriptions:
        *   Sets consecutive digits in a topology element to the given raw
        *   values.
        */
        void setDigits(const byte *values, byte topo = 0);
        /*
        * Description:
        *   Counts the number of digits spanned by a topology element.
        */
        word getDigitCount(byte topo = 0);
        /*
        * Description:
        *   Inserts a NOOP in the data stream.
        */
        void injectNoop(void);
};

#endif
