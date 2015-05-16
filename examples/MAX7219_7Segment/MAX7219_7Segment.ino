/*
* MAX7219 7-segment Example Sketch
* Written by Eberhard Fahle
* Updated for the current state of the library by Radu - Eosif Mihailescu
*
* This example sketch illustrates how to use some of the basic commands in the
* MAX7219 Library. The sketch will use the MAX7219/7221 to display some
* numbers and characters on a 7-segment display.
* More information on the MAX7219/7221 chips can be found in the datasheet.
*
* HARDWARE SETUP:
* This sketch assumes you have connected your MAX7219/7221 to the SPI
* interface of the Arduino as explained in the README file. A 4-digit
* 7-segment common cathode display should be connected to the first 4 digits
* of the MAX7219/7221. Finally, a current limiting resistor should be 
* connected between ISET and V+, whose value can be determined from MAX7219's
* and the display's datasheets.
*
* USING THE SKETCH:
* Compile, upload, enjoy :-)
*
*/

#include <stdio.h> /* for itoa(); */

//Due to a bug in Arduino, this needs to be included here too/first
#include <SPI.h>

#include <MAX7219.h>

#define _MAX7219_DEMO_DIGITS 4

const MAX7219_Topology topology[] = {{MAX7219_MODE_7SEGMENT, 0, 0, 0, 3},
                                     {MAX7219_MODE_NC, 0, 4, 0, 7}};
const char alphabet[] PROGMEM = "0123456789-EHLP ";
/* we always wait a bit between updates of the display */
const byte delaytime = 250;

MAX7219 maxled;

void setup() {
  maxled.begin(topology, 2);
  maxled.zeroDisplay();
}

/* This will scroll the whole 7-segment font on the display. */
void scrollDigits() {
  char framebuffer[4];

  for(byte i = 0; i <= strlen(alphabet) - _MAX7219_DEMO_DIGITS; i++) {
    for(byte j = 0; j < _MAX7219_DEMO_DIGITS; j++)
      framebuffer[j] = pgm_read_byte(&alphabet[i + j]);
    maxled.set7Segment(framebuffer);
    delay(delaytime);
  }
  maxled.clearDisplay();
  delay(delaytime);
}

/* This will count down from 100 to zero */
void countDown() {
  char buf[] = "    ";

  for(byte i = 99; i > 0; i--) {
    itoa(i, buf, 10);
    //Show a space (blank digit) instead of a zero
    buf[strlen(buf)] = ' ';
    maxled.set7Segment(buf);
    delay(delaytime);
  }
  maxled.clearDisplay();
  delay(delaytime);
  maxled.set7Segment("----");
  delay(delaytime);
}

void loop() { 
  scrollDigits();
  countDown();
}
