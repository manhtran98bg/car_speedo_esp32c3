#pragma once

#include "ICanvasManager.h"

// forward declare
class ICanvas;

class CanvasManagerLvgl : public ICanvasManager
{
public:
    static constexpr int MAX_CANVAS = 2;

    CanvasManagerLvgl();
    ~CanvasManagerLvgl() override;

    int  createCanvas(int width, int height, int colorFormat ) override;
    void deleteCanvas(int id) override;

    void* getCanvas(int id) override;  
    int   getWidth(int id) override;
    int   getHeight(int id) override;

    ICanvas* getCanvasWrapper(int id) override;

private:
    bool valid(int id);
    uint32_t getBufferSize(int w, int h, int cf);
    CanvasEntry _entry[MAX_CANVAS];
    ICanvas*    _canvasWrapper[MAX_CANVAS];
};
