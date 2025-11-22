#pragma once

#include "ICanvas.h"

class ICanvasManager;

class CanvasLGFX : public ICanvas {
public:
    CanvasLGFX(ICanvasManager *canvasManager, int id);

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
    int _id;
};