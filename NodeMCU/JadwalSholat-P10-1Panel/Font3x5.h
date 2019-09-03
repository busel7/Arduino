

/*
 *
 * Font3x5
 *
 * created with FontCreator
 * written by F. Maximilian Thiele
 *
 * http://www.apetech.de/fontCreator
 * me@apetech.de
 *
 * File Name           : Font3x5.h
 * Date                : 17.02.2012
 * Font size in bytes  : 1472
 * Font width          : 3
 * Font height         : 5
 * Font first char     : 32
 * Font last char      : 128
 * Font used chars     : 96
 *
 * The font data are defined as
 *
 * struct _FONT_ {
 *     uint16_t   font_Size_in_Bytes_over_all_included_Size_it_self;
 *     uint8_t    font_Width_in_Pixel_for_fixed_drawing;
 *     uint8_t    font_Height_in_Pixel_for_all_characters;
 *     unit8_t    font_First_Char;
 *     uint8_t    font_Char_Count;
 *
 *     uint8_t    font_Char_Widths[font_Last_Char - font_First_Char +1];
 *                  // for each character the separate width in pixels,
 *                  // characters < 128 have an implicit virtual right empty row
 *
 *     uint8_t    font_data[];
 *                  // bit field of all characters
 */

#include <inttypes.h>
#ifdef __AVR__
#include <avr/pgmspace.h>
#elif defined (ESP8266)
#include <pgmspace.h>
#else
#define PROGMEM
#endif

#ifndef FONT3X5_H
#define FONT3X5_H

#define FONT3X5_WIDTH 3
#define FONT3X5_HEIGHT 7

const static uint8_t Font3x5[] PROGMEM = {
    0x05, 0x07, //C0, // size
    0x03, // width
    0x07, // height
    0x20, // first char
    0x60, // char count
    
    // char widths
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, // ,!,",#,$,%,&,',(,)
    0x03, 0x03, 0x03, 0x02, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, //*,+,,-,.,/,0,1,2,3
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x03, 0x03, 0x03, //4,5,6,7,8,9,:,;,<,=
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x04, //>,?,@,A,B,C,D,E,F,G
    0x04, 0x03, 0x03, 0x04, 0x03, 0x05, 0x04, 0x03, 0x04, 0x04, //H,I,J,K,L,M,N,O,P,Q
    0x04, 0x03, 0x03, 0x03, 0x05, 0x05, 0x05, 0x03, 0x03, 0x03, //R,S,T,U,V,W,X,Y,Z,[
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, //,,,,,,,,,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, //,,,,,,,,,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x05, 0x03, 0x03, //,,,,,,,,,
    0x03, 0x03, 0x01, 0x03, 0x03, 0x03,							//,,,,,,,,,
    
    // font data
        0x00, 0x00, 0x00,                        // Code for char  
        0x00, 0x2E, 0x00,                        // Code for char !
        0x06, 0x00, 0x06,                        // Code for char "
        0x00, 0x00, 0x00,                        // Code for char #
        0x00, 0x00, 0x00,                        // Code for char $
        0x00, 0x00, 0x00,                        // Code for char %
        0x00, 0x00, 0x00,                        // Code for char &
        0x00, 0x06, 0x00,                        // Code for char '
        0x00, 0x00, 0x00,                        // Code for char (
        0x00, 0x00, 0x00,                        // Code for char )
        0x00, 0x00, 0x00,                        // Code for char *
        0x00, 0x00, 0x00,                        // Code for char +
        0x00, 0x00, 0x00,                        // Code for char ,
        0x08, 0x08,                         // Code for char -
        0x00, 0x00, 0x00,                        // Code for char .

		0x30, 0x08, 0x06,                        // Code for char /
        0x3E, 0x22, 0x3E,                        // Code for char 0
        0x24, 0x3E, 0x20,                        // Code for char 1
        0x3A, 0x2A, 0x2E,                        // Code for char 2
        0x2A, 0x2A, 0x3E,                        // Code for char 3
        0x0E, 0x08, 0x3E,                        // Code for char 4
        0x2E, 0x2A, 0x3A,                        // Code for char 5
        0x3E, 0x2A, 0x3A,                        // Code for char 6
        0x02, 0x02, 0x3E,                        // Code for char 7
        0x3E, 0x2A, 0x3E,                        // Code for char 8
        0x2E, 0x2A, 0x3E,                        // Code for char 9
        0x14,                                    // Code for char :
    
        0x00, 0x00, 0x00,                        // Code for char ;
        0x00, 0x00, 0x00,                        // Code for char <
        0x00, 0x00, 0x00,                        // Code for char =
        0x00, 0x00, 0x00,                        // Code for char >
        0x00, 0x00, 0x00,                        // Code for char ?
        0x00, 0x00, 0x00,                        // Code for char @

        0x3C, 0x0A, 0x3C,                        // Code for char A
        0x3E, 0x2A, 0x14,                        // Code for char B
        0x1C, 0x22, 0x14,                        // Code for char C
        0x3E, 0x22, 0x1C,                        // Code for char D
        0x3E, 0x2A, 0x2A,                        // Code for char E
        0x3E, 0x0A, 0x0A,                        // Code for char F
        0x1C, 0x22, 0x2A, 0x1A,                  // Code for char G
        0x3E, 0x08, 0x08, 0x3E,                  // Code for char H
        0x22, 0x3E, 0x22,                        // Code for char I
        0x10, 0x22, 0x1E,                        // Code for char J
        0x3E, 0x08, 0x14, 0x22,					 // Code for char K
        0x3E, 0x20, 0x20,                        // Code for char L
        0x3E, 0x04, 0x18, 0x04, 0x3E,            // Code for char M
        0x3E, 0x04, 0x08, 0x3E,                  // Code for char N
        0x3E, 0x22, 0x3E,                        // Code for char O
        0x3E, 0x0A, 0x0A, 0x04,                        // Code for char P
        0x1C, 0x22, 0x12, 0x2C,                  // Code for char Q
        0x3E, 0x0A, 0x1A, 0x24,                  // Code for char R
        0x2E, 0x2A, 0x3A,                        // Code for char S
        0x02, 0x3E, 0x02,                        // Code for char T
        0x3E, 0x20, 0x3E,                        // Code for char U
        0x0E, 0x10, 0x20, 0x10, 0x0E,            // Code for char V
        0x1E, 0x20, 0x18, 0x20, 0x1E,            // Code for char W
        0x22, 0x14, 0x08, 0x14, 0x22,            // Code for char X
        0x0E, 0x30, 0x0E,                        // Code for char Y
        0x32, 0x2A, 0x26,                        // Code for char Z
        0x3E, 0x22, 0x00,                        // Code for char [
        0x06, 0x08, 0x30,                        // Code for char BackSlash
        0x00, 0x22, 0x3E,                        // Code for char ]
        0x04, 0x02, 0x04,                        // Code for char ^
        0x20, 0x20, 0x20,                        // Code for char _
        0x04, 0x02, 0x02,                        // Code for char `
        0x3A, 0x2A, 0x3E,                        // Code for char a
        0x3E, 0x28, 0x38,                        // Code for char b
        0x3C, 0x24, 0x24,                        // Code for char c
        0x38, 0x28, 0x3E,                        // Code for char d
        0x3E, 0x2A, 0x2E,                        // Code for char e
        0x08, 0x3C, 0x0A,                        // Code for char f
        0x2E, 0x2A, 0x3E,                        // Code for char g
        0x3E, 0x08, 0x38,                        // Code for char h
        0x28, 0x3A, 0x20,                        // Code for char i
        0x20, 0x28, 0x3A,                        // Code for char j
        0x3E, 0x18, 0x24,                        // Code for char k
        0x1E, 0x20, 0x20,                        // Code for char l
        0x3E, 0x02, 0x3E,                        // Code for char m
        0x3E, 0x02, 0x3E,                        // Code for char n
        0x3E, 0x22, 0x3E,                        // Code for char o
        0x3E, 0x0A, 0x0E,                        // Code for char p
        0x00, 0x00, 0x00,                        // Code for char q
        0x3E, 0x04, 0x02,                        // Code for char r
        0x2E, 0x2A, 0x3A,                        // Code for char s
        0x04, 0x1E, 0x24,                        // Code for char t
        0x1E, 0x20, 0x1E,                        // Code for char u
        0x00, 0x00, 0x00,                        // Code for char v
        0x00, 0x00, 0x00,                        // Code for char w
        0x00, 0x00, 0x00,                        // Code for char x
        0x00, 0x00, 0x00,                        // Code for char y
        0x00, 0x00, 0x00,                        // Code for char z
		0x20, 0xF8, 0x88, // 123
		0xF8, // 124
		0x88, 0xF8, 0x20, // 125
		0x60, 0x20, 0x30, // 126
		0x00, 0x00, 0x00 // 127
    
};

#endif
