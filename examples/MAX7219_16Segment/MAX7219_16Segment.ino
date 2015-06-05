/*
* MAX7219 16-segment Example Sketch
* Written by Radu - Eosif Mihailescu
*
* This example sketch illustrates how to use some of the basic commands in the
* MAX7219 Library. The sketch will use the MAX7219/7221 to display some
* numbers and characters on a 16-segment display.
* More information on the MAX7219/7221 chips can be found in the datasheet.
*
* HARDWARE SETUP:
* This sketch assumes you have connected two daisy-chained MAX7219/7221s to the
* SPI interface of the Arduino as explained in the README file. A 2-digit
* 16-segment common cathode display should be connected to the first two digits
* of both MAX7219/7221s. In order to allow a common-cathode display digit be
* driven by two MAX7219s, two diodes are necessary (1N4148 will do) -- connect
* the anodes of the diodes toghether with the the common cathode of the display
* digit; then connect each diode's cathode to the MAX7219 DIGn outputs. Note
* that this means you need a total of 4 diodes for a 2-digit 16-segment display.
* Also note that the MAX7221 doesn't need this setup and in that case the
* display digit common cathode can simply be tied to both DIGn outputs of both
* chips involved. Finally, a current limiting resistor should be connected
* between ISET and V+ on each chip, whose value can be determined from
* MAX7219's and the display's datasheets.
*
* 16 SEGMENT WIRING DIAGRAM:
* For those of you who appreciate images more than words, here it goes:
*
* /--\ /--\ A1 and A2                  / A1 to DP           / G1 to DP
* |\  ^  /|                   1st chip | A2 to A   2nd chip | G2 to A
* | \ | / | F, H, I, J and B      |    | B to B        |    | H to B
* \  \v/  /                       ^    | C to C        ^    | I to C
* <--> <--> G1 and G2             |    | D1 to D       |    | J to D
* /  /^\  \                    Com. K  | D2 to E    Com. K  | K to E
* | / | \ | E, M, L, K and C           | E to F             | L to F
* |/  v  \|                            \ F to G             \ M to G
* \--/ \--/ D1 and D2
*
* Each 16-segment display digit spans two MAX7219/7221s and is connected to the
* same digit on both chips. The common cathode of the display digit connects to
* the same DIGn output on both chips, via diodes for MAX7219 and straight
* through for MAX7221.
*
* USING THE SKETCH:
* Compile, upload, enjoy :-)
*
*/

#include <stdio.h> /* for itoa(); */

//Due to a bug in Arduino, this needs to be included here too/first
#include <SPI.h>

#include <MAX7219.h>

#define _MAX7219_DEMO_DIGITS 2

const MAX7219_Topology topology[] = {{MAX7219_MODE_16SEGMENT, 0, 0, 0, 3},
                                     {MAX7219_MODE_OFF, 0, 4, 0, 7},
                                     {MAX7219_MODE_1614HALF, 1, 0, 1, 3},
                                     {MAX7219_MODE_OFF, 1, 4, 1, 7}};
const char alphabet[] PROGMEM = "0123456789ABCDEabcdeVWXYZvwxyz ";
/* we always wait a bit between updates of the display */
const byte delaytime = 250;

MAX7219 maxled;

void setup() {
  maxled.begin(topology, sizeof(topology) / sizeof(MAX7219_Topology));
  maxled.zeroDisplay();
}

/* This will scroll a 16-segment font sample on the display. */
void scrollDigits() {
  char framebuffer[_MAX7219_DEMO_DIGITS];

  for(byte i = 0; i <= strlen(alphabet) - _MAX7219_DEMO_DIGITS; i++) {
    for(byte j = 0; j < _MAX7219_DEMO_DIGITS; j++)
      framebuffer[j] = pgm_read_byte(&alphabet[i + j]);
    maxled.set16Segment(framebuffer);
    delay(delaytime);
  }
  maxled.clearDisplay();
  delay(delaytime);
}

void loop() { 
  scrollDigits();
}
