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

      struct Update update;

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
      void printSplash(String str);
      void redraw(bool full = false);
      TouchEvent processTouch();
      void clearTouch();
      
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
      void lastCommand(uint8_t command, uint8_t index, int32_t value);
      void printDebug(String msg);
      void printMem();
};

extern Display disp;

#endif