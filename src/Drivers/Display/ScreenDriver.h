#pragma once
#include "LGFX_Screen.hpp"
#include <Arduino.h>

class ScreenDriver
{
public:
    ScreenDriver();
    void begin();

    void on() const;
    void off() const;

    void setBrightness(uint8_t b);
    void fadeIn(int duration_ms);
    void fadeOut(int duration_ms);

    int createSprite(int w, int h, int depth = 16);
    void deleteSprite(int id);
    LGFX_Sprite* getSprite(int id);


    void drawRect(uint16_t *data, int16_t x, int16_t y, int16_t w, int16_t h);
    void drawRegion(uint16_t *data, int16_t x1, int16_t y1, int16_t x2, int16_t y2);

    LGFX *getPanel() { return _panel; }

private:
    static constexpr int MAX_SPRITES = 2;
    LGFX_Sprite* _sprites[MAX_SPRITES] = { nullptr };
    
    static constexpr int BACKLIGHT_PIN = LCD_BL;
    static constexpr int LEDC_CHANNEL = 0;
    static constexpr int LEDC_FREQ = 5000;
    static constexpr int LEDC_RES = 8;

    LGFX *_panel;

    void _fade(int duration_ms, bool fade_in);
    static void fadeTask(void *param);
};

extern ScreenDriver Screen;
