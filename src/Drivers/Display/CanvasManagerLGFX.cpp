#include "CanvasManagerLGFX.h"
#include "CanvasLGFX.h"
#include <stdlib.h>
#include "Arduino.h"

#include "ScreenDriver.h"

CanvasManagerLGFX::CanvasManagerLGFX()
{

    for (int i = 0; i < MAX_CANVAS; i++)
    {
        _entry[i].obj = nullptr;
        _entry[i].buffer = nullptr;
        _entry[i].w = 0;
        _entry[i].h = 0;
        _entry[i].format = (int)0;
        _canvasWrapper[i] = nullptr;
    }
}

CanvasManagerLGFX::~CanvasManagerLGFX()
{
    for (int i = 0; i < MAX_CANVAS; i++)
    {
        if (_entry[i].obj)
        {
            delete _entry[i].obj;
            _entry[i].obj = nullptr;
        }

        if (_entry[i].buffer)
        {
            free(_entry[i].buffer);
            _entry[i].buffer = nullptr;
        }

        if (_canvasWrapper[i])
        {
            delete _canvasWrapper[i];
            _canvasWrapper[i] = nullptr;
        }
    }
}

bool CanvasManagerLGFX::valid(int id)
{
    return id >= 0 && id < MAX_CANVAS && _entry[id].obj != nullptr;
}

int CanvasManagerLGFX::createCanvas(int width, int height, int colorFormat)
{
    for (int i = 0; i < MAX_CANVAS; i++)
    {
        if (_entry[i].obj == nullptr)
        {
            LGFX *panel = Screen.getPanel();
            LGFX_Sprite *spr = new LGFX_Sprite(panel);
            spr->setPsram(false);
            spr->setColorDepth(colorFormat);
            spr->createSprite(width, height);

            _entry[i].obj = (void *)spr;
            _entry[i].buffer = nullptr;
            _entry[i].w = width;
            _entry[i].h = height;
            _entry[i].format = colorFormat;
            _canvasWrapper[i] = new CanvasLGFX(this, i);
            return i;
        }
    }
    return -1;
}

void CanvasManagerLGFX::deleteCanvas(int id)
{
    if (!valid(id))
        return;

    if (_entry[id].obj)
    {
        delete _entry[id].obj;
        _entry[id].obj = nullptr;
    }

    if (_entry[id].buffer)
    {
        free(_entry[id].buffer);
        _entry[id].buffer = nullptr;
    }

    if (_canvasWrapper[id])
    {
        delete _canvasWrapper[id];
        _canvasWrapper[id] = nullptr;
    }
    _entry[id].w = 0;
    _entry[id].h = 0;
    _entry[id].format = 0;
}

void *CanvasManagerLGFX::getCanvas(int id)
{
    return valid(id) ? _entry[id].obj : nullptr;
}

int CanvasManagerLGFX::getWidth(int id)
{
    return valid(id) ? _entry[id].w : -1;
}

int CanvasManagerLGFX::getHeight(int id)
{
    return valid(id) ? _entry[id].h : -1;
}

ICanvas *CanvasManagerLGFX::getCanvasWrapper(int id)
{
    if (id < 0 || id >= MAX_CANVAS)
        return nullptr;
    return _canvasWrapper[id];
}