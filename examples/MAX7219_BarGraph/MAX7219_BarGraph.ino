/*
* MAX7219 BarGraph Example Sketch
* Written by Radu - Eosif Mihailescu
*
* This example sketch illustrates how to use some of the basic commands in the
* MAX7219 Library. The sketch will use the MAX7219/7221 to display some
* levels on a bar graph display.
* More information on the MAX7219/7221 chips can be found in the datasheet.
*
* HARDWARE SETUP:
* This sketch assumes you have connected your MAX7219/7221 to the SPI
* interface of the Arduino as explained in the README file. Four bar graph 
* displays should be connected to the first 4 digits of the MAX7219/7221 in
* common cathode mode (all cathodes of a single display connect together and 
* to the corresponding DIG input of the MAX7219/7221). All anodes are then 
* connected together across all displays and to the corresponding SEG output 
* of the MAX7219/7221. Finally, a current limiting resistor should be 
* connected between ISET and V+, whose value can be determined from MAX7219's
* and the display's datasheets.
* 
* BARGRAPH WIRING DIAGRAM:
* For those of you who appreciate images more than words, here it goes:
* 
*  SEGDP ----|>|---\  SEGDP ----|>|---\  SEGDP ----|>|---\  SEGDP ----|>|---\ 
*  SEGA  ----|>|---|  SEGA  ----|>|---|  SEGA  ----|>|---|  SEGA  ----|>|---|
*  SEGB  ----|>|---|  SEGB  ----|>|---|  SEGB  ----|>|---|  SEGB  ----|>|---| 
*  SEGC  ----|>|---|  SEGC  ----|>|---|  SEGC  ----|>|---|  SEGC  ----|>|---|
*  SEGD  ----|>|---|  SEGD  ----|>|---|  SEGD  ----|>|---|  SEGD  ----|>|---| 
*  SEGE  ----|>|---|  SEGE  ----|>|---|  SEGE  ----|>|---|  SEGE  ----|>|---| 
*  SEGF  ----|>|---|  SEGF  ----|>|---|  SEGF  ----|>|---|  SEGF  ----|>|---| 
*  SEGG  ----|>|---|  SEGG  ----|>|---|  SEGG  ----|>|---|  SEGG  ----|>|---| 
*                  |                  |                  |                  |
*               DIG0               DIG1               DIG2               DIG3
*
* USING THE SKETCH:
* Compile, upload, enjoy :-)
*
*/

//Due to a bug in Arduino, this needs to be included here too/first
#include <SPI.h>

#include <MAX7219.h>

const MAX7219_Topology topology[2] = {{MAX7219_MODE_BARGRAPH, 0, 0, 0, 3},
                                      {MAX7219_MODE_NC, 0, 4, 0, 7}};
/* we always wait a bit between updates of the display */
const byte delaytime = 125;

MAX7219 maxled;

void setup() {
  maxled.begin(topology, 2);
  maxled.zeroDisplay();
}

/* This will scroll a wave across the display, in both bar/dot mode */
void scrollWave(bool mode){
  byte framebuffer[4] = {0, 0, 0, 0}, peak, oldpeak = 0;
  signed char slope, oldslope = 1;

  //Fade in
  framebuffer[3] = 1;
  maxled.setBarGraph(framebuffer, mode);
  delay(delaytime);
  framebuffer[3] = 2;
  framebuffer[2] = 1;
  maxled.setBarGraph(framebuffer, mode);
  delay(delaytime);
  
  //Animate
  for(byte i = 0; i < 13; i++) {
    peak = oldpeak;
    slope = oldslope;
    for(byte j = 0; j < 4; j++) {
      framebuffer[j] = peak;
      if(j == 1) {
        oldpeak = peak;
        oldslope = slope;
      };
      if(peak == 8) slope = -1;
      if(peak == 0) slope = 1;
      peak += slope;
    }
    maxled.setBarGraph(framebuffer, mode);
    delay(delaytime);
  }
  
  //Fade out
  for(byte i = 0; i < 4; i++) {
    //j must first become negative for the exit-condition to become true and the
    //for loop to be exited, thus we need it to be a signed type.
    //This is a wonderful example of how the lack of bounds checking in the 
    //embedded world can ruin your day if you're not paying attention.
    for(signed char j = 3; j >= 3 - i; j--) framebuffer[j] = 0;
    for(byte j = 0; j < 3 - i; j++) framebuffer[j] = 3 - j - i;
    maxled.setBarGraph(framebuffer, mode);
    delay(delaytime);
  }
}

void loop() {
  scrollWave(true);
  scrollWave(false);
}
