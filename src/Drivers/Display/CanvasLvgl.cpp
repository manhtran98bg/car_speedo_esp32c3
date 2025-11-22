#include "CanvasLvgl.h"
#include "ICanvasManager.h"
#include "lvgl.h"
#include "Arduino.h"

static inline lv_color_t lv_color_from565(uint16_t c)
{
    uint8_t r = (c >> 11) & 0x1F; // 5 bit red
    uint8_t g = (c >> 5) & 0x3F;  // 6 bit green
    uint8_t b = c & 0x1F;         // 5 bit blue

    r = (r * 527 + 23) >> 6; // convert 5-bit → 8-bit
    g = (g * 259 + 33) >> 6; // convert 6-bit → 8-bit
    b = (b * 527 + 23) >> 6; // convert 5-bit → 8-bit
    return lv_color_make(r, g, b);
}

static inline bool _adjust_abs(int32_t &x, int32_t &w)
{
    if (w < 0)
    {
        x += w;
        w = -w;
    }
    return !w;
}

CanvasLvgl::CanvasLvgl(ICanvasManager *canvasManager, int id)
    : _canvasManager(canvasManager), _id(id)
{
    _clip_r = width();
    _clip_b = height();
}

bool CanvasLvgl::clipping(int32_t &x, int32_t &y, int32_t &w, int32_t &h)
{
    auto cl = _clip_l;
    if (x < cl)
    {
        w += x - cl;
        x = cl;
    }
    auto cr = _clip_r + 1 - x;
    if (w > cr)
        w = cr;
    if (w < 1)
        return false;

    auto ct = _clip_t;
    if (y < ct)
    {
        h += y - ct;
        y = ct;
    }
    auto cb = _clip_b + 1 - y;
    if (h > cb)
        h = cb;
    if (h < 1)
        return false;

    return true;
}

void CanvasLvgl::drawFillRectangle(int32_t x0, int32_t y0,
                               int32_t w, int32_t h,
                               uint16_t color)
{
    lv_obj_t *canvas = (lv_obj_t *)_canvasManager->getCanvas(_id);
    if (!canvas)
    {
        Serial.println("canvas is null");
        return;
    }
    lv_img_dsc_t *dsc = lv_canvas_get_img(canvas);
    if (!dsc)
    {
        Serial.println("canvas img desc null");
        return;
    }
    _adjust_abs(x0, w);
    _adjust_abs(y0, h);

    lv_color_t fillColor;
    fillColor.full = color;
    clipping(x0, y0, w, h);

    for (int j = y0; j <= y0 + h; j++)
    {
        for (int i = x0; i <= x0 + w; i++)
        {
            lv_img_buf_set_px_color(dsc, i, j, fillColor);
        }
    }
}

void CanvasLvgl::drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint16_t color)
{
    bool steep = abs(y1 - y0) > abs(x1 - x0);

    int32_t xstart = _clip_l;
    int32_t ystart = _clip_t;
    int32_t xend = _clip_r;
    int32_t yend = _clip_b;

    if (steep)
    {
        std::swap(xstart, ystart);
        std::swap(xend, yend);
        std::swap(x0, y0);
        std::swap(x1, y1);
    }
    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    if (x0 > xend || x1 < xstart)
        return;
    xend = std::min(x1, xend);

    int32_t dy = abs(y1 - y0);
    int32_t ystep = (y1 > y0) ? 1 : -1;
    int32_t dx = x1 - x0;
    int32_t err = dx >> 1;

    while (x0 < xstart || y0 < ystart || y0 > yend)
    {
        err -= dy;
        if (err < 0)
        {
            err += dx;
            y0 += ystep;
        }
        if (++x0 > xend)
            return;
    }
    int32_t xs = x0;
    int32_t dlen = 0;
    if (ystep < 0)
        std::swap(ystart, yend);
    yend += ystep;
    if (steep)
    {
        do
        {
            ++dlen;
            if ((err -= dy) < 0)
            {
                drawFillRectangle(y0, xs, 1, dlen, color);
                err += dx;
                xs = x0 + 1;
                dlen = 0;
                y0 += ystep;
                if (y0 == yend)
                    break;
            }
        } while (++x0 <= xend);
        if (dlen)
            drawFillRectangle(y0, xs, 1, dlen, color);
    }
    else
    {
        do
        {
            ++dlen;
            if ((err -= dy) < 0)
            {
                drawFillRectangle(xs, y0, dlen, 1, color);
                err += dx;
                xs = x0 + 1;
                dlen = 0;
                y0 += ystep;
                if (y0 == yend)
                    break;
            }
        } while (++x0 <= xend);
        if (dlen)
            drawFillRectangle(xs, y0, dlen, 1, color);
    }
}
void CanvasLvgl::drawFillTriangle(int32_t x0, int32_t y0,
                              int32_t x1, int32_t y1,
                              int32_t x2, int32_t y2,
                              uint16_t color)
{

    lv_obj_t *canvas = (lv_obj_t *)_canvasManager->getCanvas(_id);
    if (!canvas)
    {
        Serial.println("canvas is null");
        return;
    }

    lv_img_dsc_t *dsc = lv_canvas_get_img(canvas);
    if (!dsc)
    {
        Serial.println("canvas img desc null");
        return;
    }

    int32_t a, b;

    // Sort coordinates by Y order (y2 >= y1 >= y0)
    if (y0 > y1)
    {
        std::swap(y0, y1);
        std::swap(x0, x1);
    }
    if (y1 > y2)
    {
        std::swap(y2, y1);
        std::swap(x2, x1);
    }
    if (y0 > y1)
    {
        std::swap(y0, y1);
        std::swap(x0, x1);
    }

    if (y0 == y2)
    { // Handle awkward all-on-same-line case as its own thing
        a = b = x0;
        if (x1 < a)
            a = x1;
        else if (x1 > b)
            b = x1;
        if (x2 < a)
            a = x2;
        else if (x2 > b)
            b = x2;
        drawFastHLine(a, y0, b - a + 1, color);
        return;
    }
    if ((x1 - x0) * (y2 - y0) == (x2 - x0) * (y1 - y0))
    {
        drawLine(x0, y0, x2, y2, color);
        return;
    }

    int32_t dy1 = y1 - y0;
    int32_t dy2 = y2 - y0;
    bool change = ((x1 - x0) * dy2 > (x2 - x0) * dy1);
    int32_t dx1 = abs(x1 - x0);
    int32_t dx2 = abs(x2 - x0);
    int32_t xstep1 = x1 < x0 ? -1 : 1;
    int32_t xstep2 = x2 < x0 ? -1 : 1;
    a = b = x0;
    if (change)
    {
        std::swap(dx1, dx2);
        std::swap(dy1, dy2);
        std::swap(xstep1, xstep2);
    }
    int32_t err1 = (std::max(dx1, dy1) >> 1) + (xstep1 < 0
                                                    ? std::min(dx1, dy1)
                                                    : dx1);
    int32_t err2 = (std::max(dx2, dy2) >> 1) + (xstep2 > 0
                                                    ? std::min(dx2, dy2)
                                                    : dx2);
    if (y0 != y1)
    {
        do
        {
            err1 -= dx1;
            while (err1 < 0)
            {
                err1 += dy1;
                a += xstep1;
            }
            err2 -= dx2;
            while (err2 < 0)
            {
                err2 += dy2;
                b += xstep2;
            }
            drawFastHLine(a, y0, b - a + 1, color);
        } while (++y0 < y1);
    }

    if (change)
    {
        b = x1;
        xstep2 = x2 < x1 ? -1 : 1;
        dx2 = abs(x2 - x1);
        dy2 = y2 - y1;
        err2 = (std::max(dx2, dy2) >> 1) + (xstep2 > 0
                                                ? std::min(dx2, dy2)
                                                : dx2);
    }
    else
    {
        a = x1;
        dx1 = abs(x2 - x1);
        dy1 = y2 - y1;
        xstep1 = x2 < x1 ? -1 : 1;
        err1 = (std::max(dx1, dy1) >> 1) + (xstep1 < 0
                                                ? std::min(dx1, dy1)
                                                : dx1);
    }
    do
    {
        err1 -= dx1;
        while (err1 < 0)
        {
            err1 += dy1;
            if ((a += xstep1) == x2)
                break;
        }
        err2 -= dx2;
        while (err2 < 0)
        {
            err2 += dy2;
            if ((b += xstep2) == x2)
                break;
        }
        drawFastHLine(a, y0, b - a + 1, color);
    } while (++y0 <= y2);
}

void CanvasLvgl::drawFastHLine(int32_t x, int32_t y, int32_t w,
                               uint16_t color)
{
    lv_obj_t *canvas = (lv_obj_t *)_canvasManager->getCanvas(_id);
    if (!canvas)
    {
        Serial.println("canvas is null");
        return;
    }

    lv_img_dsc_t *dsc = lv_canvas_get_img(canvas);
    if (!dsc)
    {
        Serial.println("canvas img desc null");
        return;
    }

    if (y < _clip_t || y > _clip_b)
        return;
    auto cl = _clip_l;
    if (x < cl)
    {
        w += x - cl;
        x = cl;
    }
    auto cr = _clip_r + 1 - x;
    if (w > cr)
        w = cr;
    if (w < 1)
        return;
    drawFillRectangle(x, y, w, 1, color);
}

void CanvasLvgl::clear(uint16_t color)
{
    int w = width();
    int h = height();
    if (w < 0 || h < 0)
    {
        Serial.println("clear failed ");
        return;
    }
    drawFillRectangle(0, 0, 120, 120, 0);
}
void CanvasLvgl::push(int x, int y)
{
    lv_obj_t *canvas = (lv_obj_t *)_canvasManager->getCanvas(_id);
    if (!canvas)
        return;
    lv_obj_invalidate(canvas);
    lv_obj_set_pos(canvas, x, y);
}
int32_t CanvasLvgl::width() const
{
    return _canvasManager->getWidth(_id);
}

int32_t CanvasLvgl::height() const
{
    return _canvasManager->getHeight(_id);
}

void CanvasLvgl::setBackgroundColor(uint16_t color)
{

    lv_obj_t *canvas = (lv_obj_t *)_canvasManager->getCanvas(_id);
    if (!canvas)
        return;
    lv_canvas_set_palette(canvas, 0, lv_color_from565(color));
    _bgColor565 = color;
    lv_color_t c;
    c.full = 0;
    lv_canvas_fill_bg(canvas, c, LV_OPA_COVER);
}
void CanvasLvgl::setForegroundColor(uint16_t color)
{
    lv_obj_t *canvas = (lv_obj_t *)_canvasManager->getCanvas(_id);
    if (!canvas)
        return;
    lv_canvas_set_palette(canvas, 1, lv_color_from565(color));
    _fgColor565 = color;
}