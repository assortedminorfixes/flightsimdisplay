#ifndef DISPLAY_H
#define DISPLAY_H

#include "Adafruit_GFX.h"
#include <GU_K61NA4.h>
#include "Serial_GFX.h"

#include "TouchScreen.h"
#include "Adafruit_STMPE610.h"

/*
#include "fonts/b612reg10pt.h"
#include "fonts/b612bold7pt.h"
#include "fonts/b612bold16pt.h"
#include "fonts/b612monoreg18pt.h"
#include "fonts/b612monoreg18pt-deg.h"
#include "fonts/b612monoreg18pt-dot.h"
#include "fonts/b612monoreg18pt-dash.h"
#include "fonts/b612monoreg24pt.h"
#include "fonts/b612monoreg24pt-deg.h"
#include "fonts/b612monoreg24pt-dot.h"
#include "fonts/b612monoreg24pt-dash.h"

*/

int freeMemory();


template <typename T>
int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}


struct Fonts
{
   const GFXfont *normal;
   const GFXfont *dot;
   const GFXfont *dash;
   const GFXfont *deg;
};

/*
const Fonts font_mono_val_s = {&B612Mono_Regular18pt7b, &B612Mono_Regular18ptDot, &B612Mono_Regular18ptDash, &B612Mono_Regular18ptDeg};
const Fonts font_mono_val_l = {&B612Mono_Regular24pt7b, &B612Mono_Regular24ptDot, &B612Mono_Regular24ptDash, &B612Mono_Regular24ptDeg};
const Fonts font_var_title = {&B612_Bold16pt7b, nullptr, nullptr, nullptr};
const Fonts font_var_lbl = {&B612_Regular10pt7b, nullptr, nullptr, nullptr};
const Fonts font_var_lbl_b = {&B612_Bold7pt7b, nullptr, nullptr, nullptr};
*/

enum TouchEventType
{
   NO_TOUCH,
   NAV_BUTTON,
   CRS_BUTTON,
   SPEED_BUTTON,
   BARO_BUTTON
};

class TouchEvent
{
public:
   TouchEventType event;
   uint8_t value;

   TouchEvent() : event(TouchEventType::NO_TOUCH), value(0) {}
   TouchEvent(TouchEventType e, uint8_t v) : event(e), value(v) {}
};


class Virtual_HX8757: public Serial_GFX
{
   public:
      Virtual_HX8757(uint8_t pin1, uint8_t pin2, uint8_t pin3);
};


class Display
{

protected:

   #ifdef USE_HIRES_DISPLAY
   const char *RADIO_BUTTON_LABEL[5] = {"NAV1", "NAV2", "COM1", "COM2",
                                        "ADF"};
   const char *CRS_LABEL[4] = {"Heading", "Course OBS 1", "Course OBS 2", "GPS DTK"};
   const char *VS_LABEL[1] = {"V/S"};
   const char *IAS_LABEL[1] = {"Speed"};
   const char *BARO_LABEL[2] = {"Baro hPa", "Baro inHg"};

   const char SYM_DEG[1] = {(char)0xB0};
   const char SYM_DOT[1] = {(char)0x2E};

   const uint8_t crs_labels = sizeof(CRS_LABEL) / sizeof(CRS_LABEL[0]);
   const uint8_t speed_labels = sizeof(VS_LABEL) / sizeof(VS_LABEL[0]);
   const uint8_t baro_labels = sizeof(BARO_LABEL) / sizeof(BARO_LABEL[0]);
   #endif // USE_HIRES_DISPLAY

   String dbg_str;

   struct Update
   {
      bool alt = false;
      bool vs = false;
      bool vs_lbl = false;
      bool ias = false;
      bool ias_lbl = false;
      bool hdg = false;
      bool hdg_lbl = false;
      bool radio_active = false;
      bool radio_standby = false;
      bool radio_select_buttons = false;
      bool xpdr = false;
      bool baro = false;
   };

   struct Update update;

   #ifdef USE_HIRES_DISPLAY
	virtual void drawLabel(uint16_t x, uint16_t y, String label);
	virtual void drawLabel(uint16_t x, uint16_t y, const char *label) = 0;

	virtual void drawAltitude() = 0;
	virtual void drawVS() = 0;
	virtual void drawVSLabel() = 0;
	virtual void drawIAS() = 0;
	virtual void drawIASLabel() = 0;
	virtual void drawHeading() = 0;
	virtual void drawHeadingLabel() = 0;
	virtual void drawTransponderCode() = 0;
	virtual void drawRadioActive() = 0;
	virtual void drawRadioStandby() = 0;
	virtual void drawBarometer() = 0;
	virtual void drawRadioSelectButton(uint8_t active = 0) = 0;
   virtual void trimDecimal(float_t num, uint8_t padding, uint8_t decimals, bool dashes, bool force_sign, int x, int y, GFXcanvas1 *canvas, const Fonts *font);
   virtual void printDash(uint8_t num, GFXcanvas1 *canvas, const Fonts *font);
   String getStringValue(String data, char separator, int index);
   Adafruit_STMPE610 ts;
   #endif // USE_HIRES_DISPLAY

   //Virtual_HX8757 disp_device;
   //GFXcanvas1 disp_device;

public:
   Display();

	virtual void initDisplay() = 0;
	virtual void printStatic() = 0;
	virtual void printSplash(String str) = 0;
	virtual void printLastCommand(uint8_t command, uint8_t index, int32_t value) = 0;
	virtual void printLastCommand(uint8_t command, uint8_t index, const String value) = 0;
	virtual void printLastCommand(uint8_t command, uint8_t index, char* value) = 0;
	virtual void printDebug(String msg) = 0;
	virtual void printMem() = 0;

	virtual void redraw(bool full = false) = 0;

   #ifdef USE_HIRES_DISPLAY
   virtual TouchEvent processTouch();
	virtual void clearTouch();

	virtual void updateAltitude();
	virtual void updateVS();
	virtual void updateVSLabel();
	virtual void updateIAS();
	virtual void updateIASLabel();
	virtual void updateHeading();
	virtual void updateHeadingLabel();
	virtual void updateTransponderCode();
	virtual void updateRadioFrequencyActive();
	virtual void updateRadioFrequencyStandby();
	virtual void updateActiveRadio();
	virtual void updateBarometer();
	virtual void updateBarometerLabel(uint8_t selection) = 0;
   #endif // USE_HIRES_DISPLAY

	virtual void printSerial() = 0;
};

#endif