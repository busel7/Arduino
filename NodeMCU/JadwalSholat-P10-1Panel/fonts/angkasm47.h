

/*
 *
 * angkasm47
 *
 * created with FontCreator
 * written by F. Maximilian Thiele
 *
 * http://www.apetech.de/fontCreator
 * me@apetech.de
 *
 * File Name           : angkasm47.h
 * Date                : 15.02.2019
 * Font size in bytes  : 325
 * Font width          : 4
 * Font height         : -7
 * Font first char     : 48
 * Font last char      : 59
 * Font used chars     : 11
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

#ifndef ANGKASM47_H
#define ANGKASM47_H

#define ANGKASM47_WIDTH 4
#define ANGKASM47_HEIGHT -7

const static uint8_t angkasm47[] PROGMEM = {
    0x01, 0x45, // size
    0x04, // width
    0x07, // height
    0x30, // first char
    0x0B, // char count
    
    // char widths
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 
    0x04, 
    
    // font data
    0xFE, 0x92, 0x8A, 0xFE, // 48
    0x82, 0x82, 0xFE, 0x80, // 49
    0xF2, 0x92, 0x92, 0x9E, // 50
    0x92, 0x92, 0x92, 0xFE, // 51
    0x1E, 0x10, 0x10, 0xFE, // 52
    0x9E, 0x92, 0x92, 0xF2, // 53
    0xFE, 0x92, 0x92, 0xF2, // 54
    0x02, 0x02, 0x02, 0xFE, // 55
    0xFE, 0x92, 0x92, 0xFE, // 56
    0x9E, 0x92, 0x92, 0xFE, // 57
    0x00, 0x6C, 0x6C, 0x00 // 58
    
};

#endif
