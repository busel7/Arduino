/*--------------------------------------------------------------------------------------

 DMD.cpp - Function and support library for the Freetronics DMD, a 512 LED matrix display
           panel arranged in a 32 x 16 layout.

 Copyright (C) 2011 Marc Alexander (info <at> freetronics <dot> com)

 Note that the DMD library uses the SPI port for the fastest, low overhead writing to the
 display. Keep an eye on conflicts if there are any other devices running from the same
 SPI port, and that the chip select on those devices is correctly set to be inactive
 when the DMD is being written to.

 ---

 This program is free software: you can redistribute it and/or modify it under the terms
 of the version 3 GNU General Public License as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program.
 If not, see <http://www.gnu.org/licenses/>.

--------------------------------------------------------------------------------------*/

#include <HJS589.h>
#include <string.h>
#include <stdlib.h>
#include "Bitmap.h"




#define DMD3_NUM_COLUMNS         32     
#define DMD3_NUM_ROWS            16      

// Refresh times.
#define DMD3_REFRESH_MS          1
#define DMD3_REFRESH_US          1000


HJS589::HJS589(int widthPanels, int heightPanels)
    : Bitmap(widthPanels * DMD3_NUM_COLUMNS, heightPanels * DMD3_NUM_ROWS)
    , _doubleBuffer(0)
    , phase(0)
    , fb0(0)
    , fb1(0)
    , displayfb(0)
    , lastRefresh(millis())
{
 
    fb0 = displayfb = fb;
}

/**
 * \brief Destroys this dot matrix display handler.
 */
HJS589::~HJS589()
{
    if (fb0)
        free((void *)fb0);
    if (fb1)
        free((void *)fb1);
    fb = 0; // Don't free the buffer again in the base class.
}


void HJS589::setDoubleBuffer(bool doubleBuffer)
{
    if (doubleBuffer != _doubleBuffer) {
        _doubleBuffer = doubleBuffer;
        if (doubleBuffer) {
            // Allocate a new back buffer.
            unsigned int size = _stride * _height;
            fb1 = (uint8_t *)malloc(size);

            // Clear the new back buffer and then switch to it, leaving
            // the current contents of fb0 on the screen.
            if (fb1) {
                memset((void *)fb1, 0xFF, size);
                cli();
                fb = fb1;
                displayfb = fb0;
                sei();
            } else {
                // Failed to allocate the memory, so revert to single-buffered.
                _doubleBuffer = false;
            }
        } else if (fb1) {
            // Disabling double-buffering, so forcibly switch to fb0.
            cli();
            fb = fb0;
            displayfb = fb0;
            sei();

            // Free the unnecessary buffer.
            free((void *)fb1);
            fb1 = 0;
        }
    }
}


void HJS589::swapBuffers()
{
    if (_doubleBuffer) {
        // Turn off interrupts while swapping buffers so that we don't
        // accidentally try to refresh() in the middle of this code.
        cli();
        if (fb == fb0) {
            fb = fb1;
            displayfb = fb0;
        } else {
            fb = fb0;
            displayfb = fb1;
        }
        sei();
    }
}


void HJS589::swapBuffersAndCopy()
{
    swapBuffers();
    if (_doubleBuffer)
        memcpy((void *)fb, (void *)displayfb, _stride * _height);
}


void HJS589::loop()
{
    unsigned long currentTime = millis();
    if ((currentTime - lastRefresh) >= DMD3_REFRESH_MS) {
        lastRefresh = currentTime;
        refresh();
    }
}




static const uint8_t flipBits[256] PROGMEM = {
    0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0,
    0x30, 0xB0, 0x70, 0xF0, 0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8,
    0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8, 0x04, 0x84, 0x44, 0xC4,
    0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
    0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC,
    0x3C, 0xBC, 0x7C, 0xFC, 0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2,
    0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2, 0x0A, 0x8A, 0x4A, 0xCA,
    0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
    0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6,
    0x36, 0xB6, 0x76, 0xF6, 0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE,
    0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE, 0x01, 0x81, 0x41, 0xC1,
    0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
    0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9,
    0x39, 0xB9, 0x79, 0xF9, 0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5,
    0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5, 0x0D, 0x8D, 0x4D, 0xCD,
    0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
    0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3,
    0x33, 0xB3, 0x73, 0xF3, 0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB,
    0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB, 0x07, 0x87, 0x47, 0xC7,
    0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
    0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF,
    0x3F, 0xBF, 0x7F, 0xFF
};


void HJS589::refresh(){
 
    int stride4 = _stride * 4;
    volatile uint8_t *data0;
    volatile uint8_t *data1;
    volatile uint8_t *data2;
    volatile uint8_t *data3;
    bool flipRow = ((_height & 0x10) == 0);
    for (byte y = 0; y < _height; y += 16) {
        if (!flipRow) {
            // The panels in this row are the right way up.
            data0 = displayfb + _stride * (y + phase);
            data1 = data0 + stride4;
            data2 = data1 + stride4;
            data3 = data2 + stride4;
            for (int x = _stride; x > 0; --x) {
                SPI.write(*data3++);
                SPI.write(*data2++);
                SPI.write(*data1++);
                SPI.write(*data0++);
            }
            flipRow = true;
        } else {
          
            data0 = displayfb
     + _stride * (y + 16 - phase) - 1;
            data1 = data0 - stride4;
            data2 = data1 - stride4;
            data3 = data2 - stride4;
            for (int x = _stride; x > 0; --x) {
                SPI.transfer(pgm_read_byte(&(flipBits[*data3--])));
                SPI.transfer(pgm_read_byte(&(flipBits[*data2--])));
                SPI.transfer(pgm_read_byte(&(flipBits[*data1--])));
                SPI.transfer(pgm_read_byte(&(flipBits[*data0--])));
            }
            flipRow = false;
        }
    }
pinMode(15,INPUT);
digitalWrite(0,1);
digitalWrite(0,0);
digitalWrite(16,bitRead(phase,0));
digitalWrite(12,bitRead(phase,1));
pinMode(15,OUTPUT);
analogWrite(15,cr);
phase=++phase&0x03;
}


void HJS589::start(){
analogWriteFreq(16384);
pinMode(SCK, SPECIAL);  
pinMode(MOSI, SPECIAL);
SPI1C = 0;
SPI1U = SPIUMOSI | SPIUDUPLEX | SPIUSSE;
SPI1U1 = (7 << SPILMOSI) | (7 << SPILMISO);
SPI1C1 = 0;
SPI1C &= ~(SPICWBO | SPICRBO);
SPI1U &= ~(SPIUSME);SPI1P &= ~(1<<29);
SPI.setFrequency(10000000); 
byte jsh=0x11; 
while(jsh--){if(jsh==16||jsh==15||jsh==12||jsh==0){
digitalWrite(jsh, 0);pinMode(jsh,OUTPUT);}else{continue;}}
timer0_detachInterrupt();
timer0_isr_init();
}



void HJS589::setBrightness(uint16_t b){
if(b==1){b=2;} 
cr=b;
}



HJS589::Color HJS589::fromRGB(uint8_t r, uint8_t g, uint8_t b)
{
    if (r || g || b)
        return White;
    else
        return Black;
}
