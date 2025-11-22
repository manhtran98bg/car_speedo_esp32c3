#include "ScreenDriver.h"

ScreenDriver Screen;

// Singleton 
static LGFX& GetTFT()
{
    static LGFX tft;
    return tft;
}

ScreenDriver::ScreenDriver()
{
    _panel = &GetTFT();
}

void ScreenDriver::begin()
{
    Serial.println("Screen init");
    _panel->init();
    _panel->setRotation(0);
    _panel->setSwapBytes(true);
    _panel->fillScreen(BLACK);

    if (BACKLIGHT_PIN != -1)
    {
        ledcSetup(LEDC_CHANNEL, LEDC_FREQ, LEDC_RES);
        ledcAttachPin(BACKLIGHT_PIN, LEDC_CHANNEL);
    }
    on();
}

void ScreenDriver::on() const
{
    if (BACKLIGHT_PIN != -1)
        ledcWrite(LEDC_CHANNEL, 255);
}

void ScreenDriver::off() const
{
    if (BACKLIGHT_PIN != -1)
        ledcWrite(LEDC_CHANNEL, 0);
}

void ScreenDriver::setBrightness(uint8_t b)
{
    if (BACKLIGHT_PIN != -1)
        ledcWrite(LEDC_CHANNEL, b);
}

//===== Sprite API ======================================================

int ScreenDriver::createSprite(int w, int h, int depth)
{
    for (int i = 0; i < MAX_SPRITES; i++)
    {
        if (_sprites[i] == nullptr)
        {
            LGFX_Sprite* spr = new LGFX_Sprite(_panel);
            spr->setPsram(false);
            spr->setColorDepth(depth);
            spr->createSprite(w, h);
            _sprites[i] = spr;
            return i;
        }
    }
    return -1;  //out of slot
}

LGFX_Sprite* ScreenDriver::getSprite(int id)
{
    if (id < 0 || id >= MAX_SPRITES) return nullptr;
    return _sprites[id];
}

void ScreenDriver::deleteSprite(int id)
{
    if (id < 0 || id >= MAX_SPRITES) return;
    if (_sprites[id])
    {
        delete _sprites[id];
        _sprites[id] = nullptr;
    }
}

//===== Draw functions ==================================================

void ScreenDriver::drawRect(uint16_t *data, int16_t x, int16_t y, int16_t w, int16_t h)
{
    _panel->pushImageDMA(x, y, w, h, data);
}

void ScreenDriver::drawRegion(uint16_t *data, int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
    int w = x2 - x1 + 1;
    int h = y2 - y1 + 1;
    if (w > 0 && h > 0)
        _panel->pushImageDMA(x1, y1, w, h, data);
}

//===== Fade Task =======================================================

struct FadeParam {
    ScreenDriver* self;
    int duration;
    bool fade_in;
};

void ScreenDriver::_fade(int duration_ms, bool fade_in)
{
    FadeParam* p = new FadeParam();
    p->self = this;
    p->duration = duration_ms;
    p->fade_in = fade_in;

    xTaskCreatePinnedToCore(
        fadeTask, "fadeTask",
        2048, p, 1, NULL, 0
    );
}

void ScreenDriver::fadeTask(void* param)
{
    FadeParam* p = (FadeParam*)param;
    ScreenDriver* self = p->self;
    int duration = p->duration;
    bool fade_in = p->fade_in;
    delete p;

    if (BACKLIGHT_PIN == -1)
    {
        vTaskDelete(NULL);
        return;
    }

    float delay_ms = duration / 255.0f;

    for (int i = 0; i <= 255; i++)
    {
        int duty = fade_in ? i : (255 - i);
        self->setBrightness(duty);

        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }

    vTaskDelete(NULL);
}
