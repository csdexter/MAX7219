/* Arduino MAX7219/7221 Library
 * See the README file for author and licensing information. In case it's
 * missing from your distribution, use the one here as the authoritative
 * version: https://github.com/csdexter/MAX7219/blob/master/README
 *
 * This library is for use with Maxim's MAX7219 and MAX7221 LED driver chips.
 * See the example sketches to learn how to use the library in your code.
 *
 * This is the main code file for the library.
 * See the header file for better function documentation.
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

#include <SPI.h>

void MAX7219::begin(const MAX7219_Topology *topology, const byte length) {
    static const MAX7219_Topology defaultTopo = MAX7219_DEFAULT_TOPOLOGY;
    
    if(topology) _topology = topology;
    else _topology = defaultTopo;
    
    _chips = 0;
    for(int i = 0; i < length; i++) 
        for(int j = 0; j < _topology[i].length; j++)
            if(_topology[i].list[j].ID > _chips) 
                _chips = _topology[i].list[j].ID;
    _chips++;
    
    SPI.begin();
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);
    //1MHz suffices for doing 25fps to 625 chained chips driving 8x8 matrices,
    //2MHz (as you would get on the Uno/Mega) is twice that and if you find
    //yourself needing more, you shouldn't be using Arduino anyway
    SPI.setClockDivider(SPI_CLOCK_DIV8);
    
    for(int i = 0; i < _chips; i++) {
        //Since the MAX7219 does not have a RESET, we must enforce consistency
        noDisplayTest(i);
        setScanLimit(0x07, i);
        setIntensity(0x08, i);
        writeRegister(MAX7219_REG_DECODEMODE, 
                      MAX7219_FLG_DIGIT0_RAW | MAX7219_FLG_DIGIT1_RAW | 
                      MAX7219_FLG_DIGIT2_RAW | MAX7219_FLG_DIGIT3_RAW | 
                      MAX7219_FLG_DIGIT4_RAW | MAX7219_FLG_DIGIT5_RAW | 
                      MAX7219_FLG_DIGIT6_RAW | MAX7219_FLG_DIGIT7_RAW, i)
        noShutdown(i);
    }
    
    for(int i = 0; i < _elements; i++) {
        clearDisplay(i);
        if(_topology[i].ID == MAX7219_MODE_NC) 
            setScanLimit(0x07 - _topology[i].list[0].length, 
                         _topology[i].list[0].ID);
        if(_topology[i].ID == MAX7219_MODE_7SEGMENT) {
            for(int j = 0; j < _topology[i].length; j++) {
                byte decodemask = 0;

                for(int k = 0; k < _topology[i].list[j].length; k++)
                    decodemask |= MAX7219_FLG_DIGIT0_CODEB << 
                                  _topology[i].list[j].data[k];
                writeRegister(MAX7219_REG_DECODEMODE, decodemask, j);
            }
        }
    }
}

void MAX7219::end(void) {
    for(int i = 0; i < _elements; i++) clearDisplay(i);
    for(int i = 0; i < getChipCount(); i++) shutdown(i);
}

void MAX7219::clearDisplay(byte topo) {
    byte *buf;

    if(_topology[topo].ID == MAX7219_MODE_OFF ||
       _topology[topo].ID == MAX7219_MODE_NC) return;
    
    buf = (byte *)calloc(getDigitCount(topo), sizeof(byte));
    setDigits(buf, topo);
    free(buf);
}

void MAX7219::zeroDisplay(byte topo) {
    byte *buf;
    word digits;

    if(_topology[topo].ID == MAX7219_MODE_OFF ||
       _topology[topo].ID == MAX7219_MODE_NC) return;
  
    digits = getDigitCount(topo);
    buf = (byte *)malloc(digits * sizeof(byte));
    switch(_topology[topo].ID) {
        case MAX7219_MODE_7SEGMENT:
            memset((void *)buf, 0x00, digits * sizeof(byte));
            break;
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

void MAX7219::set7Segment(char *number, byte topo) {
    byte *buf, chr;
    word digits;

    if(_topology[topo].ID != MAX7219_MODE_7SEGMENT) return;
  
    digits = getDigitCount(topo);
    buf = (byte *)calloc(digits, sizeof(byte));
    for(int i = 0; i < digits; i++) {
        chr = number[i];
        //Set DP if so instructed
        if((byte)chr & MAX7219_FLG_SEGDP) {
            buf[i] |= MAX7219_FLG_SEGDP;
            (byte)chr &= ~MAX7219_FLG_SEGDP; 
        }
        //Cheaper than using atoi() or a lookup table
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
                buf[i] |= (byte)chr - (byte)'0';
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
    setDigits(buf, topo);    
    free(buf);
}

void MAX7219::setBarGraph(byte *values, boolean dot, byte topo){
    byte *buf;
    word digits;

    if(_topology[topo].ID != MAX7219_MODE_BARGRAPH) return;
  
    digits = getDigitCount(topo);
    buf = (byte *)malloc(digits * sizeof(byte));
    for(int i = 0; i < digits; i++) {
        if(!values[i]) buf[i] = values[i];
        else 
            if(dot) buf[i] = 1 << (values[i] - 1);
            else {
                buf[i] = 0x00;
                for(int j = 0; j < values[i] - 1; j++) buf[i] |= 1 << j;
            }
    }
    setDigits(buf, topo);    
    free(buf);  
}

void MAX7219::setMatrix(byte *values, byte topo) {
    if(_topology[topo].ID != MAX7219_MODE_MATRIX) return;
  
    setDigits(values, topo);
}

void MAX7219::writeRegister(byte addr, byte value, byte chip) {
    digitalWrite(_pinLOAD, LOW);
    
    //Datasheet calls for 25ns between LOAD/#CS going low and the start of the
    //transfer, Arduino can only go as low as 3us
    delayMicroseconds(5);
    SPI.transfer(addr);
    SPI.transfer(value);
    for(int i = 0; i < chip; i++) {
        SPI.transfer(MAX7219_REG_NOOP);
        SPI.transfer(0x00);
    }
    
    digitalWrite(_pinLOAD, HIGH);
}

void MAX7219::writeRegisters(word *registers, byte size, byte chip) {
    digitalWrite(_pinLOAD, LOW);
    
    //Datasheet calls for 25ns between LOAD/#CS going low and the start of the
    //transfer, Arduino can only go as low as 3us
    delayMicroseconds(5);

    for(int i = size; i > 0; i--) {
        SPI.transfer(highByte(registers[i - 1]);
        SPI.transfer(lowByte(registers[i - 1]);
    }
    for(int i = 0; i < chip; i++) {
        SPI.transfer(MAX7219_REG_NOOP);
        SPI.transfer(0x00);
    }
    
    digitalWrite(_pinLOAD, HIGH);
}

void MAX7219::setDigits(byte *values, byte topo) {
    word *buf = (word *)malloc(_topology[topo].length * sizeof(word));
    word digits, transfers;
    
    digits = getDigitCount(topo);
    transfers = digits / _topology[topo].length;
    if(digits % _topology[topo].length) transfers++;
    
    for (int i = 0; i < transfers; i++) {
        for(int j = 0; j < _topology[topo].length; j++)
            if(i < _topology[topo].list[j].length)
                buf[j] = word(MAX7219_REG_DIGIT0 + 
                              _topology[topo].list[j].data[i],
                              values[(transfers - 1) * j + i]);
            else
                buf[j] = word(MAX7219_REG_NOOP, 0x00);
        writeRegisters(buf, _topology[topo].length, 
                       _topology[topo].list[0].ID);
    }
    
    free(buf); 
}

void MAX7219::getDigitCount(byte topo) {
    word result = 0;
    
    for(int i = 0; i < _topology[topo].length; i++)
        result += _topology[topo].list[i].length;
        
    return result;
}
