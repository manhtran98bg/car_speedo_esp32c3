#ifndef _USER_CONFIG_H_
#define _USER_CONFIG_H_

#include "lvgl.h"
#include <stdio.h>
#include "driver/i2s.h"
// TFT config
#define TFT_HOR_RES 240
#define TFT_VER_RES 240
#define TFT_ROTATION 0

#define LCD_DC 18
#define LCD_SDA 10
#define LCD_SCLK 3
#define LCD_CS 2
#define LCD_RST 21

#define LCD_WIDTH 240
#define LCD_HEIGHT 240

#define LCD_BL 42

// IIC
#define IIC_SDA 8
#define IIC_SCL 9

// TOUCH
#define TP_INT 11
#define TP_RST 0

// SD
#define SD_CS 38
#define SD_MOSI 39
#define SD_MISO 40
#define SD_SCLK 41

#define FPS 30
#define MJPEG_BUFFER_SIZE (240 * 240 * 2 / 8)
#define AUDIOASSIGNCORE 1
#define DECODEASSIGNCORE 0
#define DRAWASSIGNCORE 1

// Audio

#define I2S_DIN 7
#define I2S_WS 4
#define I2S_BCLK 5
#define I2S_PORT I2S_NUM_0
#define SAMPLE_RATE 44100

// Color palette (constexpr để tránh multiple definition)

constexpr lv_color_t PALETTE_BLACK = LV_COLOR_MAKE(0, 0, 0);
constexpr lv_color_t PALETTE_WHITE = LV_COLOR_MAKE(255, 255, 255);
constexpr lv_color_t PALETTE_GREY = LV_COLOR_MAKE(90, 90, 90);
constexpr lv_color_t PALETTE_DARK_GREY = LV_COLOR_MAKE(60, 60, 60);
constexpr lv_color_t PALETTE_AMBER = LV_COLOR_MAKE(250, 140, 0);
constexpr lv_color_t PALETTE_RED = LV_COLOR_MAKE(255, 0, 0);

enum UiMode
{
    UI_MODE_ODO,
    UI_MODE_GIF
};

// Draw buffer sizes
#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8))
#define DRAW_BUF_SIZE1 (TFT_HOR_RES * TFT_VER_RES / 10)

extern SemaphoreHandle_t displayMutex;
extern volatile UiMode currentMode;
#endif // _USER_CONFIG_H_
