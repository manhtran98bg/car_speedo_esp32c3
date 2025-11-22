#pragma once
#include <stdint.h>

class ICanvas;

class ICanvasManager
{
public:
    struct CanvasEntry
    {
        void *obj = nullptr;
        void *buffer = nullptr;
        int w = 0;
        int h = 0;
        int format = 0;
    };

    virtual int createCanvas(int width, int height, int colorFormat ) = 0;
    virtual void deleteCanvas(int id) = 0;
    virtual void* getCanvas(int id) = 0;
    virtual int getWidth(int id) = 0;
    virtual int getHeight(int id) = 0;
    virtual ICanvas* getCanvasWrapper(int id) = 0;
    virtual ~ICanvasManager() {}
};