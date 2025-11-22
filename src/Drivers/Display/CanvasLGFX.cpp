#include "CanvasLGFX.h"
#include "ICanvasManager.h"
#include "Arduino.h"

#include "ScreenDriver.h"

CanvasLGFX::CanvasLGFX(ICanvasManager *canvasManager, int id)
    : _canvasManager(canvasManager), _id(id)
{
}

void CanvasLGFX::drawFillRectangle(int32_t x0, int32_t y0,
                                   int32_t w, int32_t h,
                                   uint16_t color)
{
    LGFX_Sprite *canvas = (LGFX_Sprite *)_canvasManager->getCanvas(_id);
    if (!canvas)
    {
        Serial.println("canvas is null");
        return;
    }
    canvas->fillRect(x0, y0, w, h, color);
}

void CanvasLGFX::drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint16_t color)
{
    LGFX_Sprite *canvas = (LGFX_Sprite *)_canvasManager->getCanvas(_id);
    if (!canvas)
    {
        Serial.println("canvas is null");
        return;
    }
    canvas->drawLine(x0, y0, x1, y1, color);
}

void CanvasLGFX::drawFillTriangle(int32_t x0, int32_t y0,
                                  int32_t x1, int32_t y1,
                                  int32_t x2, int32_t y2,
                                  uint16_t color)
{
    LGFX_Sprite *canvas = (LGFX_Sprite *)_canvasManager->getCanvas(_id);
    if (!canvas)
    {
        Serial.println("canvas is null");
        return;
    }
    canvas->fillTriangle(x0, y0, x1, y1, x2, y2, color);
}

void CanvasLGFX::drawFastHLine(int32_t x, int32_t y, int32_t w,
                               uint16_t color)
{
    LGFX_Sprite *canvas = (LGFX_Sprite *)_canvasManager->getCanvas(_id);
    if (!canvas)
    {
        Serial.println("canvas is null");
        return;
    }
    canvas->drawFastHLine(x, y, w, color);
}

void CanvasLGFX::clear(uint16_t color)
{
    LGFX_Sprite *canvas = (LGFX_Sprite *)_canvasManager->getCanvas(_id);
    if (!canvas)
    {
        Serial.println("canvas is null");
        return;
    }
    canvas->fillScreen(color);
}

void CanvasLGFX::push(int x, int y)
{
    LGFX_Sprite *canvas = (LGFX_Sprite *)_canvasManager->getCanvas(_id);
    if (!canvas)
    {
        Serial.println("canvas is null");
        return;
    }
    canvas->pushSprite(x, y);
}

int32_t CanvasLGFX::width() const
{
    return _canvasManager->getWidth(_id);
}

int32_t CanvasLGFX::height() const
{
    return _canvasManager->getHeight(_id);
}

void CanvasLGFX::setBackgroundColor(uint16_t color)
{
    LGFX_Sprite *canvas = (LGFX_Sprite *)_canvasManager->getCanvas(_id);
    if (!canvas)
    {
        Serial.println("canvas is null");
        return;
    }
    canvas->setPaletteColor(0, color);
}

void CanvasLGFX::setForegroundColor(uint16_t color)
{
    LGFX_Sprite *canvas = (LGFX_Sprite *)_canvasManager->getCanvas(_id);
    if (!canvas)
    {
        Serial.println("canvas is null");
        return;
    }
    canvas->setPaletteColor(1, color);
}