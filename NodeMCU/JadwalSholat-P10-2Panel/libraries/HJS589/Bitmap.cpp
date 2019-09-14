

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




#include <Bitmap.h>
#include <WString.h>
#include <string.h>
#include <stdlib.h>
#include <pgmspace.h>
//#include <sys/pgmspace.h>


Bitmap::Bitmap(int width, int height)
    : _width(width)
    , _height(height)
    , _stride((width + 7) / 8)
    , fb(0)
    , _font(0)
    , _textColor(White)
{
    // Allocate memory for the framebuffer and r it (1 = pixel off).
    unsigned int size = _stride * _height;
    fb = (uint8_t *)malloc(size);
    if (fb)
        memset((void *)fb, 0xFF, size);
}


Bitmap::~Bitmap()
{
    if (fb)
        free((void *)fb);
}



void Bitmap::clear(Color color)
{
    unsigned int size = _stride * _height;
    if (color == Black)
        memset((void *)fb, 0xFF, size);
    else
        memset((void *)fb, 0x00, size);
}




Bitmap::Color Bitmap::pixel(int x, int y) const
{
    if (((unsigned int)x) >= ((unsigned int)_width) ||
            ((unsigned int)y) >= ((unsigned int)_height))
        return Black;
    volatile uint8_t *ptr = fb + y * _stride + (x >> 3);
    if (*ptr & ((uint8_t)0x80) >> (x & 0x07))
        return Black;
    else
        return White;
}


void Bitmap::setPixel(int x, int y, Color color)
{
    if (((unsigned int)x) >= ((unsigned int)_width) ||
            ((unsigned int)y) >= ((unsigned int)_height))
        return;     // Pixel is off-screen.
    volatile uint8_t *ptr = fb + y * _stride + (x >> 3);
    if (color)
        *ptr &= ~(((uint8_t)0x80) >> (x & 0x07));
    else
        *ptr |= (((uint8_t)0x80) >> (x & 0x07));
}








void Bitmap::drawLine(int x1, int y1, int x2, int y2, Color color)
{
    // Midpoint line scan-conversion algorithm from "Computer Graphics:
    // Principles and Practice", Second Edition, Foley, van Dam, et al.
    int dx = x2 - x1;
    int dy = y2 - y1;
    int xstep, ystep;
    int d, incrE, incrNE;
    if (dx < 0) {
        xstep = -1;
        dx = -dx;
    } else {
        xstep = 1;
    }
    if (dy < 0) {
        ystep = -1;
        dy = -dy;
    } else {
        ystep = 1;
    }
    if (dx >= dy) {
        d = 2 * dy - dx;
        incrE = 2 * dy;
        incrNE = 2 * (dy - dx);
        setPixel(x1, y1, color);
        while (x1 != x2) {
            if (d <= 0) {
                d += incrE;
            } else {
                d += incrNE;
                y1 += ystep;
            }
            x1 += xstep;
            setPixel(x1, y1, color);
        }
    } else {
        d = 2 * dx - dy;
        incrE = 2 * dx;
        incrNE = 2 * (dx - dy);
        setPixel(x1, y1, color);
        while (y1 != y2) {
            if (d <= 0) {
                d += incrE;
            } else {
                d += incrNE;
                x1 += xstep;
            }
            y1 += ystep;
            setPixel(x1, y1, color);
        }
    }
}


void Bitmap::drawRect(int x1, int y1, int x2, int y2, Color borderColor, Color fillColor)
{
    int temp;
    if (x1 > x2) {
        temp = x1;
        x1 = x2;
        x2 = temp;
    }
    if (y1 > y2) {
        temp = y1;
        y1 = y2;
        y2 = temp;
    }
    if (fillColor == borderColor) {
        fill(x1, y1, x2 - x1 + 1, y2 - y1 + 1, fillColor);
    } else {
        drawLine(x1, y1, x2, y1, borderColor);
        if (y1 < y2)
            drawLine(x2, y1 + 1, x2, y2, borderColor);
        if (x1 < x2)
            drawLine(x2 - 1, y2, x1, y2, borderColor);
        if (y1 < (y2 - 1))
            drawLine(x1, y2 - 1, x1, y1 + 1, borderColor);
        if (fillColor != NoFill)
            fill(x1 + 1, y1 + 1, x2 - x1 - 1, y2 - y1 - 1, fillColor);
    }
}


void Bitmap::drawCircle(int centerX, int centerY, int radius, Color borderColor, Color fillColor)
{
    // Midpoint circle scan-conversion algorithm using second-order
    // differences from "Computer Graphics: Principles and Practice",
    // Second Edition, Foley, van Dam, et al.
    if (radius < 0)
        radius = -radius;
    int x = 0;
    int y = radius;
    int d = 1 - radius;
    int deltaE = 3;
    int deltaSE = 5 - 2 * radius;
    drawCirclePoints(centerX, centerY, radius, x, y, borderColor, fillColor);
    while (y > x) {
        if (d < 0) {
            d += deltaE;
            deltaE += 2;
            deltaSE += 2;
        } else {
            d += deltaSE;
            deltaE += 2;
            deltaSE += 4;
            --y;
        }
        ++x;
        drawCirclePoints(centerX, centerY, radius, x, y, borderColor, fillColor);
    }
}


void Bitmap::drawBitmap(int x, int y, const Bitmap &bitmap, Color color)
{
    int w = bitmap.width();
    int s = bitmap.stride();
    int h = bitmap.height();
    Color invColor = !color;
    for (uint8_t by = 0; by < h; ++by) {
        volatile const uint8_t *line = bitmap.data() + by * s;
        uint8_t mask = 0x80;
        uint8_t value = *line++;
        for (uint8_t bx = 0; bx < w; ++bx) {
            if (value & mask)
                setPixel(x + bx, y + by, invColor);
            else
                setPixel(x + bx, y + by, color);
            mask >>= 1;
            if (!mask) {
                mask = 0x80;
                value = *line++;
            }
        }
    }
}


void Bitmap::drawBitmap(int x, int y, Bitmap::ProgMem bitmap, Color color)
{
    uint8_t w = pgm_read_byte(bitmap);
    uint8_t s = (w + 7) >> 3;
    uint8_t h = pgm_read_byte(bitmap + (uint8_t)1);
    Color invColor = !color;
    for (uint8_t by = 0; by < h; ++by) {
        const uint8_t *line = ((const uint8_t *)bitmap) + 2 + by * s;
        uint8_t mask = 0x80;
        uint8_t value = pgm_read_byte(line);
        for (uint8_t bx = 0; bx < w; ++bx) {
            if (value & mask)
                setPixel(x + bx, y + by, color);
            else
                setPixel(x + bx, y + by, invColor);
            mask >>= 1;
            if (!mask) {
                mask = 0x80;
                ++line;
                value = pgm_read_byte(line);
            }
        }
    }
}

#define fontIsFixed(font)   (pgm_read_byte((font)) == 0 && \
                             pgm_read_byte((font) + 1) == 0)
#define fontWidth(font)     (pgm_read_byte((font) + 2))
#define fontHeight(font)    (pgm_read_byte((font) + 3))
#define fontFirstChar(font) (pgm_read_byte((font) + 4))
#define fontCharCount(font) (pgm_read_byte((font) + 5))


void Bitmap::drawText(int x, int y, const char *str, int len)
{
 
    uint8_t height = fontHeight(_font);
        len = strlen(str);
    while (len--) {
        x += drawChar(x, y, *str++);
        
            fill(x, y, 1, height, !_textColor);
            ++x;
       
        if (x >= _width)
            return;
    }
}


void Bitmap::drawText(int x, int y, const String &str, int start, int len)
{
    if (!_font)
        return;
    uint8_t height = fontHeight(_font);
    if (len < 0)
        len = str.length() - start;
    while (len-- > 0) {
        x += drawChar(x, y, str[start++]);
        if (len > 0) {
            fill(x, y, 1, height, !_textColor);
            ++x;
        }
        if (x >= _width)
            break;
    }
}


int Bitmap::drawChar(int x, int y, char ch)
{
    uint8_t height = fontHeight(_font);
    if (ch == ' ') {
        // Font may not have space, or it is zero-width.  Calculate
        // the real size and fill the space.
        int spaceWidth = charWidth('n');
        fill(x, y, spaceWidth, height, !_textColor);
        return spaceWidth;
    }
    uint8_t first = fontFirstChar(_font);
    uint8_t count = fontCharCount(_font);
    uint8_t index = (uint8_t)ch;
    if (index < first || index >= (first + count))
        return 0;
    index -= first;
    uint8_t heightBytes = (height + 7) >> 3;
    uint8_t width;
    const uint8_t *image;
    if (fontIsFixed(_font)) {
        // Fixed-width font.
        width = fontWidth(_font);
        image = (( uint8_t*)_font) + 6 + index * heightBytes * width;
    } else {
        // Variable-width font.
        width = pgm_read_byte(_font + 6 + index);
        image = (( uint8_t*)_font) + 6 + count;
        for (uint8_t temp = 0; temp < index; ++temp) {
            // Scan through all previous characters to find the starting
            // location for this one.
            image += pgm_read_byte(_font + 6 + temp) * heightBytes;
        }
    }
    if ((x + width) <= 0 || (y + height) <= 0)
        return width;   // Character is off the top or left of the screen.
    Color invColor = !_textColor;
    for (uint8_t cx = 0; cx < width; ++cx) {
        for (uint8_t cy = 0; cy < heightBytes; ++cy) {
            uint8_t value = pgm_read_byte(image + cy * width + cx);
            int posn;
            if (heightBytes > 1 && cy == (heightBytes - 1))
                posn = height - 8;
            else
                posn = cy * 8;
            for (uint8_t bit = 0; bit < 8; ++bit) {
                if ((posn + bit) >= (cy * 8) && (posn + bit) <= height) {
                    if (value & 0x01)
                        setPixel(x + cx, y + posn + bit, _textColor);
                    else
                        setPixel(x + cx, y + posn + bit, invColor);
                }
                value >>= 1;
            }
        }
    }
    return width;
}



int Bitmap::charWidth(char ch) const
{
    uint8_t index = (uint8_t)ch;
    if (!_font)
        return 0;
    uint8_t first = fontFirstChar(_font);
    uint8_t count = fontCharCount(_font);
    if (index == ' ')
        index = 'n';    // In case the font does not contain space.
    if (index < first || index >= (first + count))
        return 0;
    if (fontIsFixed(_font))
        return fontWidth(_font);
    else
        return pgm_read_byte(_font + 6 + (index - first));
}


int Bitmap::textWidth(const char *str, int len) const
{
    int width = 0;
    len = strlen(str);
    while (len--) {
        width += charWidth(*str++)+1;
        
    }
    return width;
}


int Bitmap::textWidth(const String &str, int start, int len) const
{
    int width = 0;
    if (len < 0)
        len = str.length() - start;
    while (len-- > 0) {
        width += charWidth(str[start++]);
        if (len > 0)
            ++width;
    }
    return width;
}


int Bitmap::textHeight() const
{
    if (_font)
        return fontHeight(_font);
    else
        return 0;
}


void Bitmap::copy(int x, int y, int width, int height, Bitmap *dest, int destX, int destY)
{
    if (dest == this) {
        // Copying to within the same bitmap, so copy in a direction
        // that will prevent problems with overlap.
        blit(x, y, x + width - 1, y + height - 1, destX, destY);
    } else {
        // Copying to a different bitmap.
        while (height > 0) {
            for (int tempx = 0; tempx < width; ++tempx)
                dest->setPixel(destX + tempx, destY, pixel(x + tempx, y));
            ++y;
            ++destY;
            --height;
        }
    }
}


void Bitmap::fill(int x, int y, int width, int height, Color color)
{
    while (height > 0) {
        for (int temp = 0; temp < width; ++temp)
            setPixel(x + temp, y, color);
        ++y;
        --height;
    }
}


void Bitmap::fill(int x, int y, int width, int height, Bitmap::ProgMem pattern, Color color)
{
    uint8_t w = pgm_read_byte(pattern);
    uint8_t s = (w + 7) >> 3;
    uint8_t h = pgm_read_byte(pattern + 1);
    if (!w || !h)
        return;
    Color invColor = !color;
    for (int tempy = 0; tempy < height; ++tempy) {
        const uint8_t *startLine = ((const uint8_t *)pattern) + 2 + (tempy % h) * s;
        const uint8_t *line = startLine;
        uint8_t mask = 0x80;
        uint8_t value = pgm_read_byte(line++);
        int bit = 0;
        for (int tempx = 0; tempx < width; ++tempx) {
            if (value & mask)
                setPixel(x + tempx, y + tempy, color);
            else
                setPixel(x + tempx, y + tempy, invColor);
            if (++bit >= w) {
                mask = 0x80;
                line = startLine;
                value = pgm_read_byte(line++);
                bit = 0;
            } else {
                mask >>= 1;
                if (!mask) {
                    mask = 0x80;
                    value = pgm_read_byte(line++);
                }
            }
        }
    }
}


void Bitmap::scroll(int x, int y, int width, int height, int dx, int dy, Color fillColor)
{
    // Bail out if no scrolling at all.
    if (!dx && !dy)
        return;

    // Clamp the scroll region to the extents of the bitmap.
    if (x < 0) {
        width += x;
        x = 0;
    }
    if (y < 0) {
        height += y;
        y = 0;
    }
    if ((x + width) > _width)
        width = _width - x;
    if ((y + height) > _height)
        height = _height - y;
    if (width <= 0 || height <= 0)
        return;

    // Scroll the region in the specified direction.
    if (dy < 0) {
        if (dx < 0)
            blit(x - dx, y - dy, x + width - 1 + dx, y + height - 1 + dy, x, y);
        else
            blit(x, y - dy, x + width - 1 - dx, y + height - 1 + dy, x + dx, y);
    } else {
        if (dx < 0)
            blit(x - dx, y, x + width - 1 + dx, y + height - 1 - dy, x, y + dy);
        else
            blit(x, y, x + width - 1 - dx, y + height - 1 - dy, x + dx, y + dy);
    }

    // Fill the pixels that were uncovered by the scroll.
    if (dy < 0) {
        fill(x, y + height + dy, width, -dy, fillColor);
        if (dx < 0)
            fill(x + width + dx, y, -dx, height + dy, fillColor);
        else if (dx > 0)
            fill(x, y, dx, height + dy, fillColor);
    } else if (dy > 0) {
        fill(x, y, width, -dy, fillColor);
        if (dx < 0)
            fill(x + width + dx, y + dy, -dx, height - dy, fillColor);
        else if (dx > 0)
            fill(x, y + dy, dx, height - dy, fillColor);
    } else if (dx < 0) {
        fill(x + width + dx, y, -dx, height, fillColor);
    } else if (dx > 0) {
        fill(x, y, dx, height, fillColor);
    }
}

/**
 * \brief Inverts the \a width x \a height pixels starting at top-left
 * corner (\a x, \a y).
 *
 * \sa fill()
 */
void Bitmap::invert(int x, int y, int width, int height)
{
    while (height > 0) {
        for (int tempx = x + width - 1; tempx >= x; --tempx)
            setPixel(tempx, y, !pixel(tempx, y));
        --height;
        ++y;
    }
}








//..............................................................









//..............................................................














void Bitmap::blit(int x1, int y1, int x2, int y2, int x3, int y3)
{
    if (y3 < y1 || (y1 == y3 && x3 <= x1)) {
        for (int tempy = y1; tempy <= y2; ++tempy) {
            int y = y1 - tempy + y3;
            int x = x3 - x1;
            for (int tempx = x1; tempx <= x2; ++tempx)
                setPixel(x + tempx, y, pixel(tempx, tempy));
        }
    } else {
        for (int tempy = y2; tempy >= y1; --tempy) {
            int y = y1 - tempy + y3;
            int x = x3 - x1;
            for (int tempx = x2; tempx >= x1; --tempx)
                setPixel(x + tempx, y, pixel(tempx, tempy));
        }
    }
}

void Bitmap::drawCirclePoints(int centerX, int centerY, int radius, int x, int y, Color borderColor, Color fillColor)
{
    if (x != y) {
        setPixel(centerX + x, centerY + y, borderColor);
        setPixel(centerX + y, centerY + x, borderColor);
        setPixel(centerX + y, centerY - x, borderColor);
        setPixel(centerX + x, centerY - y, borderColor);
        setPixel(centerX - x, centerY - y, borderColor);
        setPixel(centerX - y, centerY - x, borderColor);
        setPixel(centerX - y, centerY + x, borderColor);
        setPixel(centerX - x, centerY + y, borderColor);
        if (fillColor != NoFill) {
            if (radius > 1) {
                drawLine(centerX - x + 1, centerY + y, centerX + x - 1, centerY + y, fillColor);
                drawLine(centerX - y + 1, centerY + x, centerX + y - 1, centerY + x, fillColor);
                drawLine(centerX - x + 1, centerY - y, centerX + x - 1, centerY - y, fillColor);
                drawLine(centerX - y + 1, centerY - x, centerX + y - 1, centerY - x, fillColor);
            } else if (radius == 1) {
                setPixel(centerX, centerY, fillColor);
            }
        }
    } else {
        setPixel(centerX + x, centerY + y, borderColor);
        setPixel(centerX + y, centerY - x, borderColor);
        setPixel(centerX - x, centerY - y, borderColor);
        setPixel(centerX - y, centerY + x, borderColor);
        if (fillColor != NoFill) {
            if (radius > 1) {
                drawLine(centerX - x + 1, centerY + y, centerX + x - 1, centerY + y, fillColor);
                drawLine(centerX - x + 1, centerY - y, centerX + x - 1, centerY - y, fillColor);
            } else if (radius == 1) {
                setPixel(centerX, centerY, fillColor);
            }
        }
    }
}
