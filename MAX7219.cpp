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
 * This is the main code file for the library.
 * See the header file for better function documentation.
 *
 * #define MAX7219_DEBUG for debugging output via serial.
 * ---------------------------------------------------------------------------
 * The header of the original file follows:
 *
 * LedControl.cpp - A library for controling Leds with a MAX7219/MAX7221
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

#include "MAX7219.h"
#include "MAX7219-private.h"

#include <SPI.h>


void MAX7219::begin(const MAX7219_Topology *topology, const byte length) {
    MAX7219_Topology *defaultTopo;

    if(topology) {
        _topology = topology;
        _elements = length;
    } else {
        //Yes, we are leaking memory here. Yet again, in embedded software
        //things usually get allocated at start and never die off as there's
        //no exit();
        defaultTopo = (MAX7219_Topology *)malloc(sizeof(MAX7219_Topology) *
                                                 MAX7219_DEFAULT_LENGTH);
        MAX7219_DEFAULT_TOPOLOGY(defaultTopo);
        _topology = defaultTopo;
        _elements = MAX7219_DEFAULT_LENGTH;
    };

    _chips = 0;
    for(int i = 0; i < length; i++)
        if(_topology[i].chipTo > _chips) 
                _chips = _topology[i].chipTo;
    _chips++;
#if defined(MAX7219_DEBUG)
    Serial.print("Topology has ");
    Serial.print(_elements);
    Serial.print(" elements which span ");
    Serial.print(_chips);
    Serial.println(" chips in total.");
#endif
    SPI.begin();
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);
    //1MHz suffices for doing 25fps to 625 chained chips driving 8x8 matrices,
    //2MHz (as you would get on the Uno/Mega) is twice that and if you find
    //yourself needing more, you shouldn't be using Arduino anyway
    SPI.setClockDivider(SPI_CLOCK_DIV8);

    //Since the MAX7219 does not have a RESET, we must enforce consistency
    noDisplayTest(MAX7219_CHIP_ALL);
    setScanLimit(0x07, MAX7219_CHIP_ALL);
    setIntensity(0x08, MAX7219_CHIP_ALL);
    writeRegister(MAX7219_REG_DECODEMODE,
                  MAX7219_FLG_DIGIT0_RAW | MAX7219_FLG_DIGIT1_RAW |
                  MAX7219_FLG_DIGIT2_RAW | MAX7219_FLG_DIGIT3_RAW |
                  MAX7219_FLG_DIGIT4_RAW | MAX7219_FLG_DIGIT5_RAW |
                  MAX7219_FLG_DIGIT6_RAW | MAX7219_FLG_DIGIT7_RAW,
                  MAX7219_CHIP_ALL);
    noShutdown(MAX7219_CHIP_ALL);

    for(int i = 0; i < _elements; i++) {
        if(_topology[i].elementType == MAX7219_MODE_NC)
            setScanLimit(_topology[i].digitFrom - 1, _topology[i].chipFrom);
        if(_topology[i].elementType == MAX7219_MODE_7SEGMENT)
            for(int j = 0; j < _topology[i].chipTo - _topology[i].chipFrom +
                1; j++) {
                byte decodemask = 0;

                for(int k = (j == _topology[i].chipFrom ?
                             _topology[i].digitFrom : 0);
                     k <= (j == _topology[i].chipTo ? 
                          _topology[i].digitTo : 7); k++)
                    decodemask |= MAX7219_FLG_DIGIT0_CODEB << k;
                writeRegister(MAX7219_REG_DECODEMODE, decodemask, j);
            }
        clearDisplay(i);
    }
}

void MAX7219::end(void) {
    for(int i = 0; i < _elements; i++) clearDisplay(i);
    for(int i = 0; i < getChipCount(); i++) shutdown(i);
}

void MAX7219::clearDisplay(byte topo) {
    byte *buf, digits;

    if(_topology[topo].elementType == MAX7219_MODE_OFF ||
       _topology[topo].elementType == MAX7219_MODE_NC) return;

    digits = getDigitCount(topo);
    buf = (byte *)calloc(digits, sizeof(byte));
    if(_topology[topo].elementType == MAX7219_MODE_7SEGMENT)
      memset((void *)buf, 0x0F, digits * sizeof(byte)); 
    setDigits(buf, topo);
    free(buf);
}

void MAX7219::zeroDisplay(byte topo) {
    byte *buf;
    word digits;

    if(_topology[topo].elementType == MAX7219_MODE_OFF ||
       _topology[topo].elementType == MAX7219_MODE_NC) return;

    digits = getDigitCount(topo);
    buf = (byte *)malloc(digits * sizeof(byte));
    switch(_topology[topo].elementType) {
        case MAX7219_MODE_7SEGMENT:
            memset((void *)buf, 0x00, digits * sizeof(byte));
            break;
        case MAX7219_MODE_16SEGMENT:
          // use pgm_read to fetch the zero character here
            buf[0] = highByte();
            but[1] = lowByte();
            memset((void *)&buf[2], 0x00, (digits - 2) * sizeof(byte));
        case MAX7219_MODE_MATRIX:
            buf[0] = 0x01;
            memset((void *)&buf[1], 0x00, (digits - 1) * sizeof(byte));
            break;
        case MAX7219_MODE_BARGRAPH:
            memset((void *)buf, 0x01, digits * sizeof(byte));
            break;
    }
    setDigits(buf, topo);
    free(buf);
}

void MAX7219::set7Segment(const char *number, byte topo, bool mirror) {
    byte *buf, chr;
    word digits;

    if(_topology[topo].elementType != MAX7219_MODE_7SEGMENT) return;

    digits = getDigitCount(topo);
    buf = (byte *)calloc(digits, sizeof(byte));
    for(word i = 0; i < digits; i++) {
        chr = number[i];
        //Set DP if so instructed
        if((byte)chr & MAX7219_FLG_SEGDP) {
            buf[i] |= MAX7219_FLG_SEGDP;
            chr &= ~MAX7219_FLG_SEGDP; 
        }
        //Cheaper than using atoi() or a PROGMEM lookup table
        switch(chr) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                buf[i] |= chr - (byte)'0';
                break;
            case '-':
                buf[i] |= 0x0A;
                break;
            case 'E':
            case 'e':
                buf[i] |= 0x0B;
                break;
            case 'H':
            case 'h':
                buf[i] |= 0x0C;
                break;
            case 'L':
            case 'l':
                buf[i] |= 0x0D;
                break;
            case 'P':
            case 'p':
                buf[i] |= 0x0E;
                break;
            case ' ':
                buf[i] |= 0x0F;
                break;
        }
    }
    if (mirror) {
      for (word i = 0; i < digits; i++) {
        chr = buf[i];
        buf[i] = buf[digits - 1 - i];
        buf[digits - 1 - i] = chr;
      }
    }
    setDigits(buf, topo);
    free(buf);
}

void MAX7219::set16Segment(const char *text, byte topo) {
    byte *buf;
    word digits;

    if(_topology[topo].elementType != MAX7219_MODE_16SEGMENT) return;

    digits = getDigitCount(topo);
    buf = (word *)calloc(digits, sizeof(byte));
    for(word i = 0; i < digits / 2; i++) {
      // use pgm_read to index font with text and fill buf with words accordingly
    }
    setDigits(buf, topo);
    free(buf);
}

void MAX7219::setBarGraph(const byte *values, boolean dot, byte topo){
    byte *buf;
    word digits;

    if(_topology[topo].elementType != MAX7219_MODE_BARGRAPH) return;

    digits = getDigitCount(topo);
    buf = (byte *)malloc(digits * sizeof(byte));
    for(byte i = 0; i < digits; i++) {
        if(!values[i]) buf[i] = values[i];
        else 
            if(dot) buf[i] = 1 << (values[i] - 1);
            else {
                buf[i] = 0x00;
                for(int j = 0; j < values[i]; j++) buf[i] |= 1 << j;
            }
    }
    setDigits(buf, topo);
    free(buf);  
}

void MAX7219::setMatrix(const byte *values, byte topo) {
    if(_topology[topo].elementType != MAX7219_MODE_MATRIX) return;

    setDigits(values, topo);
}

void MAX7219::writeRegister(byte addr, byte value, byte chip) {
    word cmd, *buf;

    cmd = word(addr, value);
    if(chip == MAX7219_CHIP_ALL) {
      buf = (word *)malloc(_chips * sizeof(word));

      for(byte i = 0; i < _chips; i++) buf[i] = cmd;
      writeRegisters(buf, _chips, 0);

      free(buf);
    } else writeRegisters(&cmd, 1, chip);
}

void MAX7219::writeRegisters(const word *registers, byte size, byte chip) {
    digitalWrite(_pinLOAD, LOW);

    //Datasheet calls for 25ns between LOAD/#CS going low and the start of the
    //transfer, Arduino can only go as low as 3us
    delayMicroseconds(5);

    for(byte i = 0; i < _chips - (chip + size); i++) injectNoop();

    for(byte i = size; i > 0; i--) {
        SPI.transfer(highByte(registers[i - 1]));
        SPI.transfer(lowByte(registers[i - 1]));
    }
    for(byte i = 0; i < chip; i++) injectNoop();

    digitalWrite(_pinLOAD, HIGH);
#if defined(MAX7219_DEBUG)
    Serial.print("Wrote (register, value) pairs {");
    for(word i = 0; i < size; i++) {
        Serial.print("(0x");
        Serial.print(highByte(registers[i]), HEX);
        Serial.print(", 0x");
        Serial.print(lowByte(registers[i]), HEX);
        Serial.print(" [");
        Serial.print(lowByte(registers[i]), BIN);
        Serial.print("])");
        if(i < size - 1) Serial.print(", ");      
    }
    Serial.print("} starting at chip ");
    Serial.println(chip);
#endif
}

void MAX7219::setDigits(const byte *values, byte topo) {
    word *buf;
    word transfers;
    byte chips;

    chips = _topology[topo].chipTo - _topology[topo].chipFrom + 1;
    buf = (word *)malloc(chips * sizeof(word));
    transfers = (_topology[topo].chipFrom == _topology[topo].chipTo ? 
                 getDigitCount(topo) :
                 (_topology[topo].chipFrom == _topology[topo].chipTo - 1 ?
                 max(7 - _topology[topo].digitFrom,
                     _topology[topo].digitTo) + 1 : 8));
#if defined(MAX7219_DEBUG)
    Serial.print("Chips: ");
    Serial.print(chips);
    Serial.print(", transfers: ");
    Serial.print(transfers);
    Serial.print(", element: ");
    Serial.println(topo);
#endif

    for (word i = 0; i < transfers; i++) {
        for(byte j = 0; j < chips; j++)
            if(i < (j == _topology[topo].chipFrom ? 
                    7 - _topology[topo].digitFrom + 1 : 
                    (j == _topology[topo].chipTo ? 
                     _topology[topo].digitTo + 1 : 8)))
                buf[j] = word(MAX7219_REG_DIGIT0 + 
                              (j ? i : _topology[topo].digitFrom + i),
                              values[(transfers - 1) * j + i]);
            else
                buf[j] = word(MAX7219_REG_NOOP, 0x00);
        writeRegisters(buf, chips, _topology[topo].chipFrom);
    }

    free(buf); 
}

word MAX7219::getDigitCount(byte topo) {
    return (_topology[topo].chipFrom == _topology[topo].chipTo ? 
            _topology[topo].digitTo - _topology[topo].digitFrom + 1 :
            (7 - _topology[topo].digitFrom + 1 + 
             (_topology[topo].chipTo - _topology[topo].chipFrom - 1) * 8 + 
             _topology[topo].digitTo + 1));
}

void MAX7219::injectNoop(void)  {
    SPI.transfer(MAX7219_REG_NOOP);
    SPI.transfer(0x00);
};
