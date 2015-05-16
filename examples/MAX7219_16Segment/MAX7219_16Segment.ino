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
* This sketch assumes you have connected your MAX7219/7221 to the SPI
* interface of the Arduino as explained in the README file. A 2-digit
* 16-segment common cathode display should be connected to the first 4 digits
* of the MAX7219/7221. Finally, a current limiting resistor should be 
* connected between ISET and V+, whose value can be determined from MAX7219's
* and the display's datasheets.
*
* 16 SEGMENT WIRING DIAGRAM:
* For those of you who appreciate images more than words, here it goes:
*
* /--\ /--\ A1 and A2                    / A1 to DP            / G1 to DP
* |\  ^  /|                   Even digit | A2 to A   Odd digit | G2 to A
* | \ | / | F, H, I, J and B      |      | B to B        |     | H to B
* \  \v/  /                       ^      | C to C        ^     | I to C
* <--> <--> G1 and G2             |      | D1 to D       |     | J to D
* /  /^\  \                    Com. K    | D2 to E    Com. K   | K to E
* | / | \ | E, M, L, K and C             | E to F              | L to F
* |/  v  \|                              \ F to G              \ M to G
* \--/ \--/ D1 and D2
*
* Even and Odd digit pins of MAX7219 connect to the common cathode of the
* display via diodes because MAX7219 connects digit outputs to the positive rail
* when they're off and so you'd short the chip out. On the MAX7221, they are set
* to high impedance so when using that particular chip, you can safely just tie
* all three of them together.
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
                                     {MAX7219_MODE_NC, 0, 4, 0, 7}};
const char alphabet[] PROGMEM = "0123456789ABCDEabcdeVWXYZvwxyz ";
/* we always wait a bit between updates of the display */
const byte delaytime = 250;

MAX7219 maxled;

void setup() {
  maxled.begin(topology, 2);
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
