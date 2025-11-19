#include "screen_driver.h"

ScreenDriver Screen;

static LGFX *GetTFTInstance()
{
    static LGFX *panel = new LGFX();
    return panel;
}

ScreenDriver::ScreenDriver()
    : LGFX_Sprite(GetTFTInstance())
{
    _panel = GetTFTInstance();
}

void ScreenDriver::begin()
{
    Serial.println("Initializing Display...");
    off();
    _panel->init();
    _panel->setRotation(0);
	_panel->setColorDepth(16);
	_panel->fillScreen(_panel->color565(0, 0, 0));
    LGFX_Sprite::setColorDepth(1);
    LGFX_Sprite::setPsram(false);
    LGFX_Sprite::createSprite(240, 240);
    on();
}

void ScreenDriver::on() const
{
    // ledcWrite(LEDC_CHANNEL, LEDC_DUTY_MAX);
}

void ScreenDriver::off() const
{
    // ledcWrite(LEDC_CHANNEL, LEDC_DUTY_MIN);
}

void ScreenDriver::fadeIn()
{
    // xTaskCreatePinnedToCore(
    //     fadeInTask,
    //     "fadeIn_task",
    //     4096,
    //     this,
    //     tskIDLE_PRIORITY + 1,
    //     NULL,
    //     APP_CPU_NUM);
}

void ScreenDriver::fadeInTask(void *param)
{
    // for (int i = LEDC_DUTY_MIN; i < LEDC_DUTY_MAX; i++)
    // {
    //     ledcWrite(LEDC_CHANNEL, i);
    //     vTaskDelay(pdMS_TO_TICKS(5));
    // }
    // vTaskDelete(NULL);
}

void ScreenDriver::drawRegion(uint16_t *data, int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
    uint32_t w = (x2 - x1 + 1);
    uint32_t h = (y2 - y1 + 1);
    if (w <= 0 || h <= 0)
        return;
    // Arduino_Canvas::flushDirectNoCanvasBuffer(x1, y1, data, w, h);
}
void ScreenDriver::drawRect(uint16_t *data, int16_t x, int16_t y, int16_t w, int16_t h)
{
    if (w <= 0 || h <= 0)
        return;
    // Arduino_Canvas::flushDirectNoCanvasBuffer(x, y, data, w, h);
}
