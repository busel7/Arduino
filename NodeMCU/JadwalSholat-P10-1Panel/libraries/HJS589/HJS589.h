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


#ifndef HJS589_H
#define HJS589_H
#include <Bitmap.h>
#include <SPI.h>

class HJS589 : public Bitmap
{
public:
    explicit HJS589(int widthPanels = 1, int heightPanels = 1);
    ~HJS589();

    bool doubleBuffer() const { return _doubleBuffer; }
    void setDoubleBuffer(bool doubleBuffer);
    void swapBuffers();
    void swapBuffersAndCopy();

    void loop();
    void refresh();

    void start();
    void disableTimer1();

    void enableTimer2();
    void disableTimer2();
    void setBrightness(uint16_t crh);
    static Color fromRGB(uint8_t r, uint8_t g, uint8_t b);

private:
    // Disable copy constructor and operator=().
    HJS589(const HJS589 &other) : Bitmap(other) {}
    HJS589 &operator=(const HJS589 &) { return *this; }
uint16_t cr;

    bool _doubleBuffer;
    volatile uint8_t phase;
    uint8_t *fb0;
    uint8_t *fb1;
    uint8_t *displayfb;
    unsigned long lastRefresh;
};

#endif
