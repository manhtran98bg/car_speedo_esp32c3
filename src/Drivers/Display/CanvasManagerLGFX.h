#pragma once

#include "ICanvasManager.h"

// forward declare
class ICanvas;

class CanvasManagerLGFX : public ICanvasManager
{
public:
    static constexpr int MAX_CANVAS = 2;

    CanvasManagerLGFX();
    ~CanvasManagerLGFX() override;

    int  createCanvas(int width, int height, int colorFormat ) override;
    void deleteCanvas(int id) override;

    void* getCanvas(int id) override;  
    int   getWidth(int id) override;
    int   getHeight(int id) override;

    ICanvas* getCanvasWrapper(int id) override;

private:
    bool valid(int id);
    CanvasEntry _entry[MAX_CANVAS];
    ICanvas*    _canvasWrapper[MAX_CANVAS];
};
