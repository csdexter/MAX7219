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
 * This file contains definitions that are only used by the MAX7219 class and
 * shouldn't be needed by most users.
 */

#ifndef _MAX7219_PRIVATE_H_INCLUDED
#define _MAX7219_PRIVATE_H_INCLUDED

#define _MAX7219_7SEGMENT_SPACE 0x0F
#define _MAX7219_16SEGMENT_FONT_START ' '
#define _MAX7219_16SEGMENT_SPACE 0
#define _MAX7219_16SEGMENT_ZERO 16

// Font for 16-segment displays (MAX7219 doesn't have a built-in character
// generator for those). One word per character, one bit per segment, DP is not
// used and you need two MAX7219 digits to drive one 16-segment display digit.
// Mapping goes as follows (digit/segment on MAX7219 => segment on display):
// D0/DP=>A1, D0/A=>A2, D0/B=>B, D0/C=>C, D0/D=>D1, D0/E=>D2, D0/F=>E, D0/G=>F
// D1/DP=>G1, D1/A=>G2, D1/B=>H, D1/C=>I, D1/D=>J, D1/E=>K, D1/F=>L, D1/G=>M
// Font begins with ASCII 0x20, also known as space.
const word MAX7219_16Seg_Font[] PROGMEM = {
    /* ' ' to '(' */
    0x0000, 0x3000, 0x2010, 0x03D2, 0xDDD2, 0x95DB, 0x8EB4, 0x0010, 0x000C,
    /* ')' to '1' */
    0x0021, 0x00FF, 0x00D2, 0x0008, 0x00C0, 0x0000, 0x0009, 0xFF09, 0x3008,
    /* '2' to ':' */
    0xEED0, 0xFD40, 0x31D0, 0xDDC0, 0x9FC0, 0xF000, 0xFFC0, 0xF5C0, 0x0012,
    /* ';' to 'C' */
    0x0011, 0x008C, 0x0CC0, 0x0061, 0xD042, 0xEF48, 0xF3C0, 0xFC52, 0xCF00,
    /* 'D' to 'L' */
    0xFC12, 0xCF80, 0xC380, 0xDF40, 0x33C0, 0xCC12, 0x3E00, 0x038C, 0x0F00,
    /* 'M' to 'U' */
    0x3328, 0x3324, 0xFF00, 0xE3C0, 0xFF04, 0xE3C4, 0xDDC0, 0xC012, 0x3F00,
    /* 'V' to '^' */
    0x0309, 0x3305, 0x002D, 0x21C2, 0xCC09, 0x4412, 0x0024, 0x8812, 0x0005,
    /* '_' to 'g' */
    0x0C00, 0x0020, 0x0AA2, 0x0B82, 0x0A80, 0x0A92, 0x0E81, 0x40D2, 0x8992,
    /* 'h' to 'p' */
    0x0382, 0x0002, 0x0A12, 0x001E, 0x0012, 0x12C2, 0x0282, 0x0A82, 0x8390,
    /* 'q' to 'y' */
    0x8192, 0x0280, 0x8982, 0x04D2, 0x0A02, 0x0201, 0x1205, 0x001B, 0x002A,
    /* 'z' to '~' */
    0x0882, 0x4492, 0x0300, 0x8852, 0xA150
};

#endif
