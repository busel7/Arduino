/*
 *
 * ElektronMartArabic3x6
 *
 *
 * File Name           : ElektronMartArabic3x6.h
 * Date                : 6 Sept 2019
 * Font width          : 6
 * Font height         : 16
 * Font first char     : 32
 * Font last char      : 127
 * Font used chars     : 94
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

#ifndef ELEKTRONMARTARABIC3x6_H
#define ELEKTRONMARTARABIC3x6_H

#define ELEKTRONMARTARABIC3x6_WIDTH 5
#define ELEKTRONMARTARABIC3x6_HEIGHT 6

const static uint8_t ElektronMartArabic3x6[] PROGMEM = {
    0x03, 0xD0, // size // size of zero indicates fixed width font, actual length is width * height
    0x05, // width
    0x06, // height
    0x30, // first char
    0x0A, // char count
    
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 
    0x05, 0x05, 
    
	// font data			   |
	0x10, 0x38, 0x7c, 0x38, 0x10,	//0
	0x00, 0x3e, 0x7c, 0x00, 0x00,	//1
	0x7e, 0x7e, 0x06, 0x06, 0x06,	//2
	0x7e, 0x7c, 0x06, 0x04, 0x06,	//3
	0x3e, 0x7e, 0x4a, 0x4a, 0x42,	//4
	0x38, 0x7c, 0x66, 0x7c, 0x38,	//5
	0x06, 0x06, 0x06, 0x7e, 0x7c,	//6
	0x3e, 0x7e, 0x60, 0x7e, 0x3c,	//7
	0x3c, 0x7e, 0x06, 0x7e, 0x7c,	//8
	0x0e, 0x0e, 0x0a, 0x7e, 0x7e	//9    
    
};

#endif
