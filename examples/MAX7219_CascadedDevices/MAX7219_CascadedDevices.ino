/*
* MAX7219 Cascaded Devices Example Sketch
* Written by Radu - Eosif Mihailescu
*
* This example sketch illustrates how to use some of the basic commands in the
* MAX7219 Library. The sketch will use two cascaded MAX7219/7221s to display
* various graphics on the connected displays.
* More information on the MAX7219/7221 chips can be found in the datasheet.
*
* HARDWARE SETUP:
* This sketch assumes you have connected your first MAX7219/7221 to the SPI
* interface of the Arduino as explained in the README file; the second one
* should also be connected as in the README (DIN to the first's DOUT etc.).
* 4 7-segment, common cathode displays are connected to the first 4 digits of
* the first MAX7219/7221, one bargraph display is connected to the last digit
* of the first chip and another, identical, one to the first digit of the 
* second chip; finally, a 5x7 dot matrix display is connected to the last 5 
* digits of the second chip. See the 7Segment and BarGraph example sketches
* for connection diagrams for the 7-segment and bargraph displays, the matrix
* connections are detailed below. Finally, one current limiting resistor
* should be connected between ISET and V+ (of each chip), whose value can be
* determined from MAX7219's and the displays' datasheets.
*
* SAFETY WARNING:
* Please double check power consumption figures *before* powering this up.
* Depending on the display types/models used and your particular situation,
* you may need to use a separate +5V power supply for the MAX7219/7221 and LED
* displays. No, I will not pay for your smoking USB port so please be a good
* engineer and do the math yourself before plugging this in.
* 
* 5x7 MATRIX WIRING DIAGRAM:
* For those of you who appreciate images more than words, here it goes:
* 
* DIG3 -*-----+--*-----+--*-----+--*-----+--*-----+--*-----+--\
*       |     |  |     |  |     |  |     |  |     |  |     |  |
*       \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-\ 
*             |        |        |        |        |        |        |
* DIG4 -*-----+--*-----+--*-----+--*-----+--*-----+--*-----+--*-----+
*       |     |  |     |  |     |  |     |  |     |  |     |  |     |
*       \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-* 
*             |        |        |        |        |        |        |
* DIG5 -*-----+--*-----+--*-----+--*-----+--*-----+--*-----+--*-----+
*       |     |  |     |  |     |  |     |  |     |  |     |  |     |
*       \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-* 
*             |        |        |        |        |        |        |
* DIG6 -*-----+--*-----+--*-----+--*-----+--*-----+--*-----+--*-----+
*       |     |  |     |  |     |  |     |  |     |  |     |  |     |
*       \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-* 
*             |        |        |        |        |        |        |
* DIG7 -*-----+--*-----+--*-----+--*-----+--*-----+--*-----+--*-----+
*       |     |  |     |  |     |  |     |  |     |  |     |  |     |
*       \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-*  \-|<|-* 
*             |        |        |        |        |        |        |
*          SEGA     SEGB     SEGC     SEGD     SEGE     SEGF     SEGG
*
* USING THE SKETCH:
* Compile, upload, enjoy :-)
*
*/

//Due to a bug in Arduino, this needs to be included here too/first
#include <SPI.h>

#include <MAX7219.h>

const MAX7219_Topology topology[] = {{MAX7219_MODE_7SEGMENT, 0, 0, 0, 3},
                                     {MAX7219_MODE_OFF, 0, 4, 0, 6},
                                     {MAX7219_MODE_BARGRAPH, 0, 7, 1, 0},
                                     {MAX7219_MODE_OFF, 1, 1, 1, 2},
                                     {MAX7219_MODE_MATRIX, 1, 3, 1, 7}};
#define THE_7SEGMENT 0
#define THE_BARGRAPH 2
#define THE_MATRIX 4
const char alphabet[] PROGMEM = "0123456789-EHLP ";
/* we always wait a bit between updates of the display */
const byte delaytime = 250;

MAX7219 maxled;

void setup() {
  maxled.begin(topology, 5);
  maxled.zeroDisplay(THE_7SEGMENT);
  maxled.zeroDisplay(THE_BARGRAPH);
  maxled.zeroDisplay(THE_MATRIX);
}

void loop() {
  char ssfb[4];
  byte bgfb[2];
  byte matrixfb[5];

  for(byte i = 0; i < 13; i++) {
    //7-segment animation frame
    for(byte j = 0; j < 4; j++)
      ssfb[j] = pgm_read_byte(&alphabet[i + j]);
    maxled.set7Segment(ssfb, THE_7SEGMENT);

    //BarGraph animation frame
    bgfb[0] = map(i, 0, 12, 0, 8);
    bgfb[1] = map(i, 0, 12, 8, 0);
    maxled.setBarGraph(bgfb, true, THE_BARGRAPH);

    //Matrix animation frame
    memset(matrixfb, 0x00, sizeof(matrixfb));
    matrixfb[map(i, 0, 12, 0, 4)] = 0xFF;
    for(byte j = 0; j < 5; j++)
      matrixfb[j] |= 1 << map(i, 0, 12, 0, 6);
    maxled.setMatrix(matrixfb, THE_MATRIX);

    delay(delaytime);
  }
}
