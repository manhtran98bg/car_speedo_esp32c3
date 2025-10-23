#ifndef _USER_CONFIG_H_
#define _USER_CONFIG_H_

#include "lvgl.h"
#include <stdio.h>
#include "driver/i2s.h"
// TFT config
#define TFT_HOR_RES 240
#define TFT_VER_RES 240
#define TFT_ROTATION 0

#define LCD_DC 7
#define LCD_SDA 6
#define LCD_SCLK 5
#define LCD_CS 8
#define LCD_RST 9

#define LCD_WIDTH 240
#define LCD_HEIGHT 240

#define LCD_BL 10

// IIC
#define IIC_SDA 8
#define IIC_SCL 9

// TOUCH
#define TP_INT 11
#define TP_RST 0

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

#endif // _USER_CONFIG_H_
