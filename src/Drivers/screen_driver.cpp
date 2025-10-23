#include "screen_driver.h"

ScreenDriver Screen;

ScreenDriver::ScreenDriver()
    : Arduino_Canvas(LCD_WIDTH, LCD_HEIGHT, nullptr)
{
    _bus = new Arduino_ESP32SPI(LCD_DC, LCD_CS, LCD_SCLK, LCD_SDA, -1, FSPI);
    _tft = new Arduino_GC9A01(_bus, LCD_RST, 0, true, LCD_WIDTH, LCD_HEIGHT);
    this->setDriver(_tft);
}

void ScreenDriver::begin()
{
    Serial.println("Initializing Display...");

    // ledcSetup(LEDC_CHANNEL, LEDC_FREQ, LEDC_TIMER_RES);
    // ledcAttachPin(LEDC_PIN, LEDC_CHANNEL);
    // off();
    Arduino_Canvas::begin();   
    fillScreen(BLACK);        
    setRotation(TFT_ROTATION);
    // delay(100);
    // on();
}

void ScreenDriver::setDriver(Arduino_G *gfx) {
    this->_output = gfx;
}
void ScreenDriver::on() const
{
    ledcWrite(LEDC_CHANNEL, LEDC_DUTY_MAX);
}

void ScreenDriver::off() const
{
    ledcWrite(LEDC_CHANNEL, LEDC_DUTY_MIN);
}

void ScreenDriver::fadeIn()
{
    xTaskCreatePinnedToCore(
        fadeInTask,
        "fadeIn_task",
        4096,
        this,
        tskIDLE_PRIORITY + 1,
        NULL,
        0);
}

void ScreenDriver::fadeInTask(void *param)
{
    for (int i = LEDC_DUTY_MIN; i < LEDC_DUTY_MAX; i++)
    {
        ledcWrite(LEDC_CHANNEL, i);
        vTaskDelay(pdMS_TO_TICKS(5));
    }
    vTaskDelete(NULL);
}

void ScreenDriver::drawRegion(uint16_t *data, int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
    uint32_t w = (x2 - x1 + 1);
    uint32_t h = (y2 - y1 + 1);
    if (w <= 0 || h <= 0)
        return;
    Arduino_Canvas::flushDirectNoCanvasBuffer(x1, y1, data, w, h);
}
void ScreenDriver::drawRect(uint16_t *data, int16_t x, int16_t y, int16_t w, int16_t h)
{
    if (w <= 0 || h <= 0)
        return;
    Arduino_Canvas::flushDirectNoCanvasBuffer(x, y, data, w, h);
}
