#pragma once
#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include "user_config.h"

class ScreenDriver 
#ifdef ARDUINO_CANVAS
    : public Arduino_Canvas
#endif
{
public:
    ScreenDriver();
    void begin();
    
    void on() const;
    void off() const;
    void fadeIn();
    void drawRect(uint16_t *data, int16_t x, int16_t y, int16_t w, int16_t h);
    void drawRegion(uint16_t *data, int16_t x1, int16_t y1, int16_t x2, int16_t y2);
    Arduino_GFX *getScreen();
private:
    // LEDC constants
    static constexpr int LEDC_TIMER_RES = 8;
    static constexpr int LEDC_DUTY_MIN = 0;
    static constexpr int LEDC_DUTY_MAX = 255;
    static constexpr int LEDC_CHANNEL = 0;
    static constexpr int LEDC_PIN = LCD_BL;
    static constexpr int LEDC_FREQ = 5000;

    Arduino_DataBus *_bus;
    Arduino_GFX *_tft;
    static void fadeInTask(void *param);
    
};

extern ScreenDriver Screen;
