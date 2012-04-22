/*
* MAX7219 Matrix Example Sketch
* Written by Radu - Eosif Mihailescu
*
* This example sketch illustrates how to use some of the basic commands in the
* MAX7219 Library. The sketch will use the MAX7219/7221 to display some
* graphics on an 8x8 dot-matrix display.
* More information on the MAX7219/7221 chips can be found in the datasheet.
*
* HARDWARE SETUP:
* This sketch assumes you have connected your MAX7219/7221 to the SPI
* interface of the Arduino as explained in the README file. An 8x8 dot-matrix
* display should be connected to the MAX7219/7221: each common cathode rail of
* the matrix to the corresponding DIG input of the MAX7219/7221 and every
* common anode rail to the corresponding SEG output of the MAX7219/7221.
* Physically orient the matrix such that the topmost row of pixels is the one
* connected to the DIG0 input and the leftmost column of pixels is the one
* connected to the SEGDP output. Finally, a current limiting resistor should
* be connected between ISET and V+, whose value can be determined from 
* MAX7219's and the matrix' datasheets.
* 
* MATRIX WIRING DIAGRAM:
* For those of you who appreciate images more than words, here it goes:
*
* DIG0 -*--------*--------*--------*--------*--------*--------*--------\
*       |        |        |        |        |        |        |        |
*       \-|<|-\  \-|<|-\  \-|<|-\  \-|<|-\  \-|<|-\  \-|<|-\  \-|<|-\  \-|<|-\
*             |        |        |        |        |        |        |        |
* DIG1 -*-----+--*-----+--*-----+--*-----+--*-----+--*-----+--*-----+--*-----+
*       |     |  |     |  |     |  |     |  |     |  |     |  |     |  |     |
*       \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-* 
*             |        |        |        |        |        |        |        |
* DIG3 -*-----+--*-----+--*-----+--*-----+--*-----+--*-----+--*-----+--*-----+
*       |     |  |     |  |     |  |     |  |     |  |     |  |     |  |     |
*       \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-* 
*             |        |        |        |        |        |        |        |
* DIG4 -*-----+--*-----+--*-----+--*-----+--*-----+--*-----+--*-----+--*-----+
*       |     |  |     |  |     |  |     |  |     |  |     |  |     |  |     |
*       \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-* 
*             |        |        |        |        |        |        |        |
* DIG5 -*-----+--*-----+--*-----+--*-----+--*-----+--*-----+--*-----+--*-----+
*       |     |  |     |  |     |  |     |  |     |  |     |  |     |  |     |
*       \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-* 
*             |        |        |        |        |        |        |        |
* DIG6 -*-----+--*-----+--*-----+--*-----+--*-----+--*-----+--*-----+--*-----+
*       |     |  |     |  |     |  |     |  |     |  |     |  |     |  |     |
*       \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-* 
*             |        |        |        |        |        |        |        |
* DIG7 -*-----+--*-----+--*-----+--*-----+--*-----+--*-----+--*-----+--*-----+
*       |     |  |     |  |     |  |     |  |     |  |     |  |     |  |     |
*       \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-* 
*             |        |        |        |        |        |        |        |
* DIG7 -*-----+--*-----+--*-----+--*-----+--*-----+--*-----+--*-----+--*-----+
*       |     |  |     |  |     |  |     |  |     |  |     |  |     |  |     |
*       \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-* 
*             |        |        |        |        |        |        |        |
*          SEGDP    SEGA     SEGB     SEGC     SEGD     SEGE     SEGF     SEGG
*
* USING THE SKETCH:
* Compile, upload, enjoy :-)
*
*/

//Due to a bug in Arduino, this needs to be included here too/first
#include <SPI.h>

#include <MAX7219.h>

const MAX7219_Topology topology = {MAX7219_MODE_MATRIX, 0, 0, 0, 7};
/* we always wait a bit between updates of the display */
const byte delaytime = 250;
#define MOVIE_LENGTH 7
/* the short animation we will display */
const byte movie[MOVIE_LENGTH][8] = {{B00111100,
                                      B00111100,
                                      B00011000,
                                      B00011000,
                                      B00011000,
                                      B00011000,
                                      B00111100,
                                      B00111100},
                                     {B00000000,
                                      B01100110,
                                      B11111111,
                                      B11111111,
                                      B01111110,
                                      B00111100,
                                      B00011000,
                                      B00000000},
                                     {B01100000,
                                      B10010000,
                                      B11110000,
                                      B10010000,
                                      B10011011,
                                      B00001100,
                                      B00001000,
                                      B00001000},
                                     {B00010000,
                                      B00010000,
                                      B01110000,
                                      B10010000,
                                      B01010000,
                                      B00001001,
                                      B00001011,
                                      B00000101},
                                     {B00000000,
                                      B00011000,
                                      B00000000,
                                      B00111000,
                                      B00011000,
                                      B00011000,
                                      B01111110,
                                      B01111110},
                                     {B00000000,
                                      B10100000,
                                      B11010000,
                                      B10010000,
                                      B00000110,
                                      B00001001,
                                      B00001001,
                                      B00000110},
                                     {B00111100,
                                      B00111100,
                                      B00111100,
                                      B00011000,
                                      B00000000,
                                      B00011000,
                                      B00111100,
                                      B00011000}};
/* the first element is an operation code and the second is the number of
   animation ticks to perform it for:
  - opcode 1: display frame
  - opcode 2: flash frame */
const byte script[MOVIE_LENGTH][2] = {{1, 4}, {2, 4}, {1, 2}, {1, 2}, {1, 2},
                                      {1, 2}, {1, 8}};

MAX7219 maxled;
byte framebuffer[8];

void setup() {
  maxled.begin(&topology);
  maxled.zeroDisplay();
}

void animateArduino() {
  for(byte i = 0; i < MOVIE_LENGTH; i++) {
    switch(script[i][0]) {
      case 1:
        maxled.setMatrix(movie[i]);
        delay(delaytime * script[i][1]);
      break;
      case 2:
        for(byte j = 0; j < script[i][1]; j++) {
          maxled.setMatrix(movie[i]);
          delay(delaytime / 2);
          maxled.clearDisplay();
          delay(delaytime / 2);
        }
      break;  
    }
  }
}

void rows() {
  for(byte i = 0; i < 8; i++) {
    framebuffer[i] = 0xFF;
    for(byte j = 0; j < 4; j++) {
      maxled.setMatrix(framebuffer);
      delay(delaytime / 2);
      maxled.clearDisplay();
      delay(delaytime / 2);      
    }
    framebuffer[i] = 0x00;
  }
}

void columns(){
  for(byte i = 0; i < 8; i++) {
    memset(framebuffer, 1 << i, 8);
    for(byte j = 0; j < 4; j++) {
      maxled.setMatrix(framebuffer);
      delay(delaytime / 2);
      maxled.clearDisplay();
      delay(delaytime / 2);      
    }
  }
}

void single(){
  memset(framebuffer, 0x00, 8);
  for(byte i = 0; i < 8; i++) {
    framebuffer[0] = 1 << i;
    framebuffer[i] = 0x01 | 1 << i;
    maxled.setMatrix(framebuffer);
    delay(delaytime);
    framebuffer[0] = 0x00;
    framebuffer[i] = 0x00;
  }
}

void loop() { 
  animateArduino();
  rows();
  columns();
  single();
}
