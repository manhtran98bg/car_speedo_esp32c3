#pragma once
#include <Arduino.h>
// #include <Arduino_GFX_Library.h>
// #include <Arduino_DriveBus_Library.h>
// #include "user_config.h"

#include <LovyanGFX.hpp>
#include "LGFX_Screen.hpp"


class ScreenDriver : public LGFX_Sprite
{
public:
    ScreenDriver();
    void begin();

    void on() const;
    void off() const;
    void fadeIn();
    void drawRect(uint16_t *data, int16_t x, int16_t y, int16_t w, int16_t h);
    void drawRegion(uint16_t *data, int16_t x1, int16_t y1, int16_t x2, int16_t y2);
    
private:
    // LEDC constants
    static constexpr int LEDC_TIMER_RES = 8;
    static constexpr int LEDC_DUTY_MIN = 0;
    static constexpr int LEDC_DUTY_MAX = 255;
    static constexpr int LEDC_CHANNEL = 0;
    static constexpr int LEDC_PIN = -1;
    static constexpr int LEDC_FREQ = 5000;

    LGFX * _panel;
    static void fadeInTask(void *param);
    
};

extern ScreenDriver Screen;
