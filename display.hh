#ifndef DISPLAY_H
#define DISPLAY_H

#include "Adafruit_GFX.h"
#include "Adafruit_HX8357.h"

#include "TouchScreen.h"
#include "Adafruit_STMPE610.h"

enum TouchEventType {
   NO_TOUCH,
   NAV_BUTTON,
   CRS_BUTTON
};

class TouchEvent {
   public:
      TouchEventType event = NO_TOUCH;
      uint8_t value = 0;
};

class Display {

   private:
      GFXcanvas1 cCenter;
      const char deg[1]  = {0xB0};
      struct NAVData
      {
         int32_t alt = 0;
         int16_t vs = 0;
         int16_t hdg = 0;
         uint8_t crs_sel = 0;
         int16_t crs = 0;
         float_t baro = 0.0;
      };

      struct Freq
      {
         float_t active = 0.0;
         float_t standby = 0.0;
      };

      struct RadioData
      {
         Freq freq;
         uint8_t sel = 0;
         uint16_t xpdr = 0;
      };

      struct Update
      {
         bool alt = false;
         bool vs = false;
         bool hdg = false;
         bool crs = false;
         bool radio_active = false;
         bool radio_standby = false;
         bool radio_buttons = false;
         bool xpdr = false;
         bool baro = false;
      };

      struct NAVData nav_data;
      struct Update update;
      struct RadioData radio;

      void drawAltitude();
      void drawVerticalSpeed();
      void drawHeading();
      void drawCourse();
      void drawTransponderCode();
      void drawRadioActive();
      void drawRadioStandby();
      void drawBarometer();
      GFXcanvas1 trimDecimal(float_t num, uint8_t padding, uint8_t decimals, int x, int y, const GFXfont *font);
      String getStringValue(String data, char separator, int index);
   // Define TFT
   public:
      
      Display();
      Adafruit_HX8357 lcd;
      Adafruit_STMPE610 ts;
      void initDisplay();
      void printButtons(uint8_t active = 0);
      void printStatic();
      void startConfig();
      void redraw();
      TouchEvent processTouch();

      void setAltitude(int32_t alt);
      void setVerticalSpeed(int16_t vs);
      void setHeading(int16_t hdg);
      void setCourse(int16_t crs);
      void updateCourseLabel(uint8_t selection);
      void setTransponderCode(int16_t xpdr);
      void setRadioFrequencyActive(float_t freq);
      void setRadioFrequencyStandby(float_t freq);
      void setActiveRadio(uint8_t radio);
      void setBarometer(float_t baro);
      void lastCommand(uint8_t command, int32_t value);
      void printDebug(String msg);
      void printMem();
};

extern Display disp;

#endif