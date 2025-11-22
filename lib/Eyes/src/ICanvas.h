#pragma once

#include "stdint.h"
class ICanvas
{
public:
    virtual void drawFillRectangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint16_t color) = 0;
    virtual void drawFillTriangle(int32_t x0, int32_t y0,
                              int32_t x1, int32_t y1,
                              int32_t x2, int32_t y2,
                              uint16_t color) = 0;
    virtual void drawFastHLine(int32_t x, int32_t y, int32_t w, uint16_t color) = 0;
    virtual void drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint16_t color) = 0;
    virtual void clear(uint16_t color) = 0;

    virtual void push(int x, int y) = 0;

    virtual int32_t width(void) const = 0;
    virtual int32_t height(void) const = 0;

    virtual void setBackgroundColor(uint16_t color) = 0;
    virtual void setForegroundColor(uint16_t color) = 0;

    virtual ~ICanvas() {}
};
