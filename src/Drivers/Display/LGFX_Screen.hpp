#pragma once

#define LGFX_USE_V1

#include <LovyanGFX.hpp>

#define LCD_DC    7
#define LCD_SDA   6
#define LCD_SCLK  5
#define LCD_CS    8
#define LCD_RST   9
#define LCD_BL    -1

#define LCD_WIDTH 240
#define LCD_HEIGHT 240

constexpr uint16_t BLACK       = lgfx::v1::color565(0,   0,   0);
constexpr uint16_t WHITE       = lgfx::v1::color565(255, 255, 255);
constexpr uint16_t RED         = lgfx::v1::color565(255, 0,   0);
constexpr uint16_t GREEN       = lgfx::v1::color565(0,   255, 0);
constexpr uint16_t BLUE        = lgfx::v1::color565(0,   0,   255);

constexpr uint16_t YELLOW      = lgfx::v1::color565(255, 255, 0);
constexpr uint16_t CYAN        = lgfx::v1::color565(0,   255, 255);
constexpr uint16_t MAGENTA     = lgfx::v1::color565(255, 0,   255);

constexpr uint16_t GRAY        = lgfx::v1::color565(128, 128, 128);
constexpr uint16_t LIGHTGRAY   = lgfx::v1::color565(192, 192, 192);
constexpr uint16_t DARKGRAY    = lgfx::v1::color565(64,  64,  64);

constexpr uint16_t ORANGE      = lgfx::v1::color565(255, 165, 0);
constexpr uint16_t BROWN       = lgfx::v1::color565(165, 42,  42);
constexpr uint16_t PURPLE      = lgfx::v1::color565(128, 0,   128);
constexpr uint16_t PINK        = lgfx::v1::color565(255, 192, 203);
constexpr uint16_t LIME        = lgfx::v1::color565(50,  205, 50);


class LGFX : public lgfx::LGFX_Device
{

  lgfx::Panel_GC9A01 _panel_instance;
  lgfx::Bus_SPI _bus_instance;

public:
  LGFX(void)
  {
    {                                    
      auto cfg = _bus_instance.config(); 
      cfg.spi_host = SPI2_HOST; 
      cfg.spi_mode = 0;                  
      cfg.freq_write = 80000000;       
      cfg.freq_read = 16000000;         
      cfg.spi_3wire = true;              
      cfg.use_lock = true;               
      cfg.dma_channel = SPI_DMA_CH_AUTO; 

      cfg.pin_sclk = LCD_SCLK;  
      cfg.pin_mosi = LCD_SDA; 
      cfg.pin_miso = -1; 
      cfg.pin_dc = LCD_DC;   
      _bus_instance.config(cfg);              
      _panel_instance.setBus(&_bus_instance); 
    }

    {                                      
      auto cfg = _panel_instance.config(); 

      cfg.pin_cs = LCD_CS;   
      cfg.pin_rst = LCD_RST;  
      cfg.pin_busy = -1;

      cfg.panel_width = LCD_WIDTH;    
      cfg.panel_height = LCD_HEIGHT;   
      cfg.offset_x = 0;         
      cfg.offset_y = 0;      
      cfg.offset_rotation = 0;  
      cfg.dummy_read_pixel = 8; 
      cfg.dummy_read_bits = 1;  
      cfg.readable = true;     
      cfg.invert = true;      
      cfg.rgb_order = false;    
      cfg.dlen_16bit = false;   
      cfg.bus_shared = false;  
      _panel_instance.config(cfg);
    }
    setPanel(&_panel_instance);
  }
};
