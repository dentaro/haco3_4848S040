#pragma once

#include <iostream>
#define LGFX_USE_V1
#include <LovyanGFX.hpp>

#include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp>
#include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp>
#include <driver/i2c.h>

class LGFX : public lgfx::LGFX_Device
{
public:

  lgfx::Bus_RGB      _bus_instance;
  lgfx::Panel_ST7701 _panel_instance;
  lgfx::Touch_GT911  _touch_instance;
  lgfx::Light_PWM   _light_instance;

  LGFX(void)
  {
    {
      auto cfg = _panel_instance.config();

      cfg.memory_width  = 480;
      cfg.memory_height = 480;
      cfg.panel_width  = 480;
      cfg.panel_height = 480;

      cfg.offset_x = 0;
      cfg.offset_y = 0;

      cfg.offset_rotation  =     0;  // 回転方向の値のオフセット 0~7 (4~7は上下反転)
      // cfg.dummy_read_pixel =     8;  // ピクセル読出し前のダミーリードのビット数
      // cfg.dummy_read_bits  =     1;  // ピクセル以外のデータ読出し前のダミーリードのビット数
      // cfg.readable         = false;  // データ読出しが可能な場合 trueに設定
      // cfg.invert           =  true;  // パネルの明暗が反転してしまう場合 trueに設定
      // cfg.rgb_order        =  true;  // パネルの赤と青が入れ替わってしまう場合 trueに設定
      // cfg.dlen_16bit       = true;  // データ長を16bit単位で送信するパネルの場合 trueに設定
      // cfg.bus_shared       = false;  // SDカードとバスを共有している場合 trueに設定(drawJpgFile等でバス制御を行います)

      _panel_instance.config(cfg);
    }

    {
      auto cfg = _panel_instance.config_detail();
      
      cfg.pin_cs = -1;//39;//40;//1;
      cfg.pin_sclk = -1;//48;//36;//12;
      cfg.pin_mosi = -1;//47;//35;//11;


      // _panel_instance.setInvert(false);
      _panel_instance.config_detail(cfg);
      
    }

    {

      
      
      auto cfg = _bus_instance.config();
      
      cfg.panel = &_panel_instance;


//-------------------------------------
  //CMY
  // cfg.pin_d0  = GPIO_NUM_4;  // B0
  // cfg.pin_d1  = GPIO_NUM_5;  // B1
  // cfg.pin_d2  = GPIO_NUM_6; // B2
  // cfg.pin_d3  = GPIO_NUM_7;  // B3
  // cfg.pin_d4  = GPIO_NUM_15;  // B4
  // cfg.pin_d5  = GPIO_NUM_8;  // G0
  // cfg.pin_d6  = GPIO_NUM_20;  // G1
  // cfg.pin_d7  = GPIO_NUM_3;  // G2
  // cfg.pin_d8  = GPIO_NUM_46; // G3
  // cfg.pin_d9  = GPIO_NUM_9; // G4
  // cfg.pin_d10 = GPIO_NUM_10;  // G5
  // cfg.pin_d11 = GPIO_NUM_0; // R0
  // cfg.pin_d12 = GPIO_NUM_11; // R1
  // cfg.pin_d13 = GPIO_NUM_12; // R2
  // cfg.pin_d14 = GPIO_NUM_13; // R3
  // cfg.pin_d15 = GPIO_NUM_14; // R4

  //-------------------------------------
  //赤っぽい
      // cfg.pin_d0 = GPIO_NUM_8;    // B0
      // cfg.pin_d1 = GPIO_NUM_3;    // B1
      // cfg.pin_d2 = GPIO_NUM_46;   // B2
      // cfg.pin_d3 = GPIO_NUM_9;    // B3
      // cfg.pin_d4 = GPIO_NUM_1;    // B4

      // cfg.pin_d5 = GPIO_NUM_5;    // G0
      // cfg.pin_d6 = GPIO_NUM_6;    // G1
      // cfg.pin_d7 = GPIO_NUM_7;    // G2
      // cfg.pin_d8 = GPIO_NUM_15;   // G3
      // cfg.pin_d9 = GPIO_NUM_16;   // G4
      // cfg.pin_d10 = GPIO_NUM_4;   // G5

      // cfg.pin_d11 = GPIO_NUM_45;  // R0
      // cfg.pin_d12 = GPIO_NUM_48;  // R1
      // cfg.pin_d13 = GPIO_NUM_47;  // R2
      // cfg.pin_d14 = GPIO_NUM_21;  // R3
      // cfg.pin_d15 = GPIO_NUM_14;  // R4

      //-------------------------------------
//YMC

      // cfg.pin_d0  = GPIO_NUM_11; // B0//4
      // cfg.pin_d1  = GPIO_NUM_12; // B1//5
      // cfg.pin_d2  = GPIO_NUM_13; // B2//6
      // cfg.pin_d3  = GPIO_NUM_14; // B3//7
      // cfg.pin_d4  = GPIO_NUM_0; // B4//15

      // cfg.pin_d5  = GPIO_NUM_8; // G0//8
      // cfg.pin_d6  = GPIO_NUM_20;  // G1//9
      // cfg.pin_d7  = GPIO_NUM_3;  // G2//3
      // cfg.pin_d8  = GPIO_NUM_46;  // G3//46
      // cfg.pin_d9  = GPIO_NUM_9;  // G4//9
      // cfg.pin_d10 = GPIO_NUM_10;  // G5//10

      // cfg.pin_d11 = GPIO_NUM_4;  // R0//11
      // cfg.pin_d12 = GPIO_NUM_5;  // R1//12
      // cfg.pin_d13 = GPIO_NUM_6;  // R2//13
      // cfg.pin_d14 = GPIO_NUM_7;  // R3//14
      // cfg.pin_d15 = GPIO_NUM_15;  // R4//0

      // cfg.pin_d0  = GPIO_NUM_11; // B0//4
      // cfg.pin_d1  = GPIO_NUM_12; // B1//5
      // cfg.pin_d2  = GPIO_NUM_13; // B2//6
      // cfg.pin_d3  = GPIO_NUM_14; // B3//7
      // cfg.pin_d4  = GPIO_NUM_0; // B4//15

      // cfg.pin_d5  = GPIO_NUM_8; // G0//8
      // cfg.pin_d6  = GPIO_NUM_20;  // G1//9
      // cfg.pin_d7  = GPIO_NUM_3;  // G2//3
      // cfg.pin_d8  = GPIO_NUM_46;  // G3//46
      // cfg.pin_d9  = GPIO_NUM_9;  // G4//9
      // cfg.pin_d10 = GPIO_NUM_10;  // G5//10

      // cfg.pin_d11 = GPIO_NUM_4;  // R0//11
      // cfg.pin_d12 = GPIO_NUM_5;  // R1//12
      // cfg.pin_d13 = GPIO_NUM_6;  // R2//13
      // cfg.pin_d14 = GPIO_NUM_7;  // R3//14
      // cfg.pin_d15 = GPIO_NUM_15;  // R4//0

      cfg.pin_d0  = GPIO_NUM_0; // B0//0
      cfg.pin_d1  = GPIO_NUM_14; // B1//14
      cfg.pin_d2  = GPIO_NUM_13; // B2//13
      cfg.pin_d3  = GPIO_NUM_12; // B3//12
      cfg.pin_d4  = GPIO_NUM_11; // B4//11

      cfg.pin_d5  = GPIO_NUM_10; // G0//10
      cfg.pin_d6  = GPIO_NUM_9;  // G1//9
      cfg.pin_d7  = GPIO_NUM_46;  // G2//46
      cfg.pin_d8  = GPIO_NUM_3;  // G3//3
      cfg.pin_d9  = GPIO_NUM_20;  // G4//20
      cfg.pin_d10 = GPIO_NUM_8;  // G5//8

      cfg.pin_d11 = GPIO_NUM_15;  // R0//15
      cfg.pin_d12 = GPIO_NUM_7;  // R1//7
      cfg.pin_d13 = GPIO_NUM_6;  // R2//6
      cfg.pin_d14 = GPIO_NUM_5;  // R3//5
      cfg.pin_d15 = GPIO_NUM_4;  // R4//4

      cfg.pin_henable = GPIO_NUM_18;//18//DE?
      cfg.pin_vsync   = GPIO_NUM_17;//17
      cfg.pin_hsync   = GPIO_NUM_16;//16
      cfg.pin_pclk    = GPIO_NUM_21;//21
      cfg.freq_write  = 16000000;

      //同期にかんする設定
      cfg.hsync_polarity    = 0;
      cfg.hsync_front_porch = 10;
      cfg.hsync_pulse_width = 8;
      cfg.hsync_back_porch  = 50;
      cfg.vsync_polarity    = 0;
      cfg.vsync_front_porch = 10;
      cfg.vsync_pulse_width = 8;
      cfg.vsync_back_porch  = 20;
      cfg.pclk_idle_high    = 0;
      cfg.de_idle_high      = 1;
      _bus_instance.config(cfg);
    }
    _panel_instance.setBus(&_bus_instance);

    {
      auto cfg = _touch_instance.config();
      cfg.x_min      = 0;
      cfg.x_max      = 480;
      cfg.y_min      = 0;
      cfg.y_max      = 480;
      cfg.bus_shared = false;
      cfg.offset_rotation = 0;

      cfg.i2c_port   = I2C_NUM_1;

      cfg.pin_int    = GPIO_NUM_NC;
      cfg.pin_sda    = GPIO_NUM_19;//47
      cfg.pin_scl    = GPIO_NUM_45;//45
      cfg.pin_rst    = GPIO_NUM_NC;//

      cfg.freq       = 400000;
      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance);
    }

    {
      auto cfg = _light_instance.config();
      cfg.pin_bl = GPIO_NUM_38;
      _light_instance.config(cfg);
    }
    _panel_instance.light(&_light_instance);

    setPanel(&_panel_instance);
  }
};
