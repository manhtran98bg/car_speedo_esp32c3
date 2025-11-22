#pragma once

#include "ICanvas.h"

class ICanvasManager;

class CanvasLvgl : public ICanvas {
public:
    CanvasLvgl(ICanvasManager *canvasManager, int id);

    void drawFillRectangle(int32_t x0, int32_t y0,
                       int32_t x1, int32_t y1,
                       uint16_t color) override;

    void drawFillTriangle(int32_t x0, int32_t y0,
                      int32_t x1, int32_t y1,
                      int32_t x2, int32_t y2,
                      uint16_t color) override;
    void drawLine(int32_t x0, int32_t y0, 
                    int32_t x1, int32_t y1, uint16_t color) override;

    void drawFastHLine(int32_t x, int32_t y,
                       int32_t w, uint16_t color) override;

    void clear(uint16_t color) override;

    int32_t width()  const override;
    int32_t height() const override;

    void setBackgroundColor(uint16_t color) override;
    void setForegroundColor(uint16_t color) override;

    void push(int x, int y) override;

private:
    ICanvasManager *_canvasManager;
    int32_t _clip_l = 0, _clip_r = -1, _clip_t = 0, _clip_b = -1;
    int _id;
    uint16_t _fgColor565 = 0xFFFF;  //White 
    uint16_t _bgColor565 = 0x0000;  //Black
    bool clipping(int32_t &x, int32_t &y, int32_t &w, int32_t &h);
};