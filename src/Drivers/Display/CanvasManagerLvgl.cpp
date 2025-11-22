#include "CanvasManagerLvgl.h"
#include "CanvasLvgl.h"
#include "lvgl.h"
#include <stdlib.h>
#include "Arduino.h"

CanvasManagerLvgl::CanvasManagerLvgl()
{

    for (int i = 0; i < MAX_CANVAS; i++)
    {
        _entry[i].obj = nullptr;
        _entry[i].buffer = nullptr;
        _entry[i].w = 0;
        _entry[i].h = 0;
        _entry[i].format = (int)LV_IMG_CF_UNKNOWN;
        _canvasWrapper[i] = nullptr;
    }
}

CanvasManagerLvgl::~CanvasManagerLvgl()
{
    for (int i = 0; i < MAX_CANVAS; i++)
    {
        if (_entry[i].obj)
            lv_obj_del((lv_obj_t *)_entry[i].obj);

        if (_entry[i].buffer)
            free(_entry[i].buffer);

        if (_canvasWrapper[i])
        {
            delete _canvasWrapper[i];
            _canvasWrapper[i] = nullptr;
        }
    }
}

bool CanvasManagerLvgl::valid(int id)
{
    return id >= 0 && id < MAX_CANVAS && _entry[id].obj != nullptr;
}
uint32_t CanvasManagerLvgl::getBufferSize(int w, int h, int cf)
{
    switch ((lv_img_cf_t)cf)
    {
    case LV_IMG_CF_TRUE_COLOR:
        return LV_CANVAS_BUF_SIZE_TRUE_COLOR(w, h);
    case LV_IMG_CF_TRUE_COLOR_ALPHA:
        return LV_CANVAS_BUF_SIZE_TRUE_COLOR_ALPHA(w, h);
    case LV_IMG_CF_INDEXED_1BIT:
        return LV_CANVAS_BUF_SIZE_INDEXED_1BIT(w, h);
    case LV_IMG_CF_INDEXED_2BIT:
        return LV_CANVAS_BUF_SIZE_INDEXED_2BIT(w, h);
    case LV_IMG_CF_INDEXED_4BIT:
        return LV_CANVAS_BUF_SIZE_INDEXED_4BIT(w, h);
    case LV_IMG_CF_INDEXED_8BIT:
        return LV_CANVAS_BUF_SIZE_INDEXED_8BIT(w, h);
    default:
        return w * h * sizeof(lv_color_t);
    }
}
int CanvasManagerLvgl::createCanvas(int width, int height, int colorFormat)
{
    for (int i = 0; i < MAX_CANVAS; i++)
    {
        if (_entry[i].obj == nullptr)
        {
            size_t size = getBufferSize(width, height, (lv_img_cf_t)colorFormat);
            void *buf = malloc(size);
            if (!buf)
                return -1;

            lv_obj_t *canvas = lv_canvas_create(lv_scr_act());
            if (!canvas)
            {
                Serial.println("lv_canvas_create failed, returned NULL");
                free(buf);
                return -1;
            }

            lv_canvas_set_buffer(canvas, buf, width, height, colorFormat);
            _entry[i].obj = canvas;
            _entry[i].buffer = buf;
            _entry[i].w = width;
            _entry[i].h = height;
            _entry[i].format = colorFormat;
            _canvasWrapper[i] = new CanvasLvgl(this, i);
            return i;
        }
    }
    return -1;
}

void CanvasManagerLvgl::deleteCanvas(int id)
{
    if (!valid(id))
        return;

    lv_obj_del((lv_obj_t *)_entry[id].obj);
    if (_entry[id].buffer)
    {
        free(_entry[id].buffer);
        _entry[id].buffer = nullptr;
    }

    _entry[id].obj = nullptr;
    _entry[id].buffer = nullptr;
    _entry[id].w = 0;
    _entry[id].h = 0;
    _entry[id].format = (int)LV_IMG_CF_UNKNOWN;
    if (_canvasWrapper[id])
    {
        delete _canvasWrapper[id];
        _canvasWrapper[id] = nullptr;
    }
}

void *CanvasManagerLvgl::getCanvas(int id)
{
    return valid(id) ? _entry[id].obj : nullptr;
}

int CanvasManagerLvgl::getWidth(int id)
{
    return valid(id) ? _entry[id].w : -1;
}

int CanvasManagerLvgl::getHeight(int id)
{
    return valid(id) ? _entry[id].h : -1;
}

ICanvas *CanvasManagerLvgl::getCanvasWrapper(int id)
{
    if (id < 0 || id >= MAX_CANVAS)
        return nullptr;
    return _canvasWrapper[id];
}