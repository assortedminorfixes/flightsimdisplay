#include <Arduino.h>

#include "display.hh"
#include "messaging.hh"
#include "featherwing_touch.hh"
#include "state.hh"
#include "Serial_GFX.h"

#include <SPI.h>

#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char *sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif // __arm__

int freeMemory()
{
    char top;
#ifdef __arm__
    return &top - reinterpret_cast<char *>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
    return &top - __brkval;
#else  // __arm__
    return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif // __arm__
}

/*
template <typename T>
int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}
*/

#define CANVAS_NUM_H 45
#define CANVAS_NUM_W 160

#define RADIO_BUTTON_H 30
#define RADIO_BUTTON_V 50
#define RADIO_BUTTON_RAD 1
#define RADIO_BUTTON_POS_Y 90
#define RADIO_BUTTON_TXTPOS_Y (RADIO_BUTTON_POS_Y + 20)

#define DATA_RADIO_ACT_POS_X 0
#define DATA_RADIO_ACT_POS_Y 25
#define LABEL_RADIO_ACT_POS_X 0
#define LABEL_RADIO_ACT_POS_Y 5

#define DATA_RADIO_STANDBY_POS_X 160
#define DATA_RADIO_STANDBY_POS_Y 25
#define LABEL_RADIO_STANDBY_POS_X 160
#define LABEL_RADIO_STANDBY_POS_Y 5

#define DATA_ALT_POS_X 0
#define DATA_ALT_POS_Y 180
#define LABEL_ALT_POS_X 0
#define LABEL_ALT_POS_Y 160

#define DATA_SPEED_POS_X 160
#define DATA_SPEED_POS_Y 180
#define LABEL_SPEED_POS_X 160
#define LABEL_SPEED_POS_Y 160

#define DATA_HDG_POS_X 0
#define DATA_HDG_POS_Y 265
#define LABEL_HDG_POS_X 0
#define LABEL_HDG_POS_Y 245

#define DATA_IAS_POS_X 160
#define DATA_IAS_POS_Y 265
#define LABEL_IAS_POS_X 160
#define LABEL_IAS_POS_Y 245

#define DATA_XPDR_POS_X 0
#define DATA_XPDR_POS_Y 360
#define LABEL_XPDR_POS_X 0
#define LABEL_XPDR_POS_Y 340

#define DECIMAL_PAD 5
#define DEBUG_STR_LENGTH 25

#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000

Virtual_HX8757::Virtual_HX8757(uint8_t pin1, uint8_t pin2, uint8_t pin3) : Serial_GFX(320, 480)
{
    raw = true;
}

Display::Display()
{
}

#ifdef USE_HIRES_DISPLAY

TouchEvent Display::processTouch()
{
    int8_t nav_sel = -1;
    int8_t crs_sel = -1;
    //int8_t speed_sel = -1;
    int8_t baro_sel = -1;

    if (!ts.bufferEmpty())
    {
        while (!ts.bufferEmpty())
        {
            TS_Point p = ts.getPoint();
            p.x = map(p.x, TS_MINX, TS_MAXX, 0, disp_device.width());
            p.y = map(p.y, TS_MINY, TS_MAXY, 0, disp_device.height());

            if (p.y > RADIO_BUTTON_POS_Y && p.y < (RADIO_BUTTON_POS_Y + RADIO_BUTTON_H))
            {
                for (int i = 0; i < 5; i++)
                {
                    if (p.x > (15 + (RADIO_BUTTON_V + 10) * i) && p.x < (5 + (RADIO_BUTTON_V + 10) * (i + 1)))
                    {
                        if (state.debug)
                            disp_device.drawCircle(p.x, p.y, 1, 1);
                        nav_sel = i;
                    }
                }
            }
            // CRS Selector
            else if (p.y > 235 && p.y < 320 && p.x < 160)
            {
                if (state.debug)
                    disp_device.drawCircle(p.x, p.y, 1, 1);
                crs_sel = 1;
            }
            // Baro Selector
            else if (p.y > 340 && p.y < 415 && p.x > 160)
            {
                if (state.debug)
                    disp_device.drawCircle(p.x, p.y, 1, 1);
                baro_sel = 1;
            }
        }
        if (nav_sel > -1 && (nav_sel != state.radio.sel))
        {
            state.radio.sel = nav_sel;
            update.radio_select_buttons = true;
            return TouchEvent(TouchEventType::NAV_BUTTON, state.radio.sel);
        }
        else if (crs_sel > -1)
        {
            state.nav.crs_sel = (state.nav.crs_sel + 1) % this->crs_labels;
            update.hdg = true;
            return TouchEvent(TouchEventType::CRS_BUTTON, state.nav.crs_sel);
        }
        else if (baro_sel > -1)
        {
            state.nav.baro_mode_sel = (state.nav.baro_mode_sel + 1) % this->baro_labels;
            update.baro = true;
            return TouchEvent(TouchEventType::BARO_BUTTON, state.nav.baro_mode_sel);
        }
        else
        {
            return TouchEvent();
        }
    }
    else
    {
        return TouchEvent();
    }
}

void Display::clearTouch()
{
    while (!ts.bufferEmpty())
    {
        ts.getPoint();
    }
}

void Display::drawLabel(uint16_t x, uint16_t y, String label)
{
    this->drawLabel(x, y, label.c_str());
}

void Display::redraw(bool full)
{
    if (update.radio_active || full)
        drawRadioActive();
    if (update.radio_standby || full)
        drawRadioStandby();
    if (update.radio_select_buttons || full)
        drawRadioSelectButton(state.radio.sel);
    if (update.alt || full)
        drawAltitude();
    if (update.vs_lbl || full)
        drawVSLabel();
    if (update.vs || full)
        drawVS();
    if (update.ias_lbl || full)
        drawIASLabel();
    if (update.ias || full)
        drawIAS();
    if (update.hdg || full)
        drawHeading();
    if (update.hdg_lbl || full)
        drawHeadingLabel();
    if (update.xpdr || full)
        drawTransponderCode();
    if (update.baro || full)
        drawBarometer();
}

void Display::updateAltitude()
{
    update.alt = true;
}

void Display::updateVS()
{
    update.vs = true;
}

void Display::updateVSLabel()
{
    update.vs_lbl = true;
    update.vs = true;
}

void Display::drawVSLabel()
{
    if (state.nav.vs.label.length() > 0)
    {
        this->drawLabel(LABEL_SPEED_POS_X, LABEL_SPEED_POS_Y, state.nav.vs.label);
    }
    else
    {
        this->drawLabel(LABEL_SPEED_POS_X, LABEL_SPEED_POS_Y, VS_LABEL[0]);
    }
    update.vs_lbl = false;
}

void Display::updateIAS()
{
    update.ias = true;
}

void Display::updateIASLabel()
{
    update.ias_lbl = true;
    update.ias = true;
}

void Display::drawIASLabel()
{
    if (state.nav.ias.label.length() > 0)
    {
        this->drawLabel(LABEL_IAS_POS_X, LABEL_IAS_POS_Y, state.nav.ias.label);
    }
    else
    {
        this->drawLabel(LABEL_IAS_POS_X, LABEL_IAS_POS_Y, IAS_LABEL[0]);
    }
    update.ias_lbl = false;
}

void Display::updateHeading()
{
    update.hdg = true;
}

void Display::updateHeadingLabel()
{
    update.hdg_lbl = true;
}

void Display::drawHeadingLabel()
{
    if (state.nav.hdg.label.length() > 0)
    {
        this->drawLabel(LABEL_HDG_POS_X, LABEL_HDG_POS_Y, state.nav.hdg.label);
    }
    else
    {
        this->drawLabel(LABEL_HDG_POS_X, LABEL_HDG_POS_Y, CRS_LABEL[state.nav.crs_sel]);
    }
    update.hdg_lbl = false;
}

void Display::updateTransponderCode()
{
    update.xpdr = true;
}

void Display::updateBarometer()
{
    update.baro = true;
}

void Display::updateActiveRadio()
{
    update.radio_active = true;
    update.radio_standby = true;
    update.radio_select_buttons = true;
}

void Display::updateRadioFrequencyActive()
{
    update.radio_active = true;
}

void Display::updateRadioFrequencyStandby()
{
    update.radio_standby = true;
}

void Display::printLastCommand(uint8_t command, uint8_t idx, int32_t val)
{
    this->printLastCommand(command, idx, String(val));
}

void Display::trimDecimal(float_t num, uint8_t padding, uint8_t decimals, bool dashes, bool force_sign, int x, int y, GFXcanvas1 *canvas, const Fonts *font)
{
    const float_t COMP_EPSILON = 1.0 / pow(10, decimals + 1);

    // GFXcanvas1 canvas(CANVAS_NUM_W, CANVAS_NUM_H);
    String num_str = "";

    bool negative = (sgn(num) == -1);

    if (fabsf(num) < COMP_EPSILON && dashes)
    {
        for (int i = 0; i < padding; i++)
            num_str.concat('-');
        num_str.concat('.');
        for (int i = 0; i < decimals; i++)
            num_str.concat('-');
        canvas->setFont(font->normal);
    }
    else
    {
        num_str.concat(String(fabsf(num), decimals));
        canvas->setFont(font->normal);
    }

    String num_str_int = getStringValue(num_str, '.', 0);
    String num_str_int_pad;
    if (force_sign && !negative)
    {
        num_str_int_pad.concat('+');
    }
    else if (negative)
    {
        num_str_int_pad.concat('-');
    }
    uint8_t pads = padding - num_str_int.length();
    for (int i = 0; i < pads; i++)
    {
        num_str_int_pad.concat('0');
    }
    num_str_int_pad.concat(num_str_int);

    if (decimals > 0)
        num_str_int_pad.concat('.');

    String num_str_frac = getStringValue(num_str, '.', 1);

    canvas->setCursor(x, y);

    int16_t x1, y1;
    uint16_t w, h;
    canvas->getTextBounds(num_str_int_pad.c_str(), x, y, &x1, &y1, &w, &h); // calc width of new string
    canvas->printf("%s", num_str_int_pad.c_str());
    canvas->setCursor(x + w + DECIMAL_PAD, y);
    canvas->printf("%s", num_str_frac.c_str());

    return;
}

void Display::printDash(uint8_t num, GFXcanvas1 *canvas, const Fonts *font)
{
    canvas->setFont(font->dash);

    for (uint8_t i = 0; i < num; i++)
    {
        canvas->print('-');
    }
}

// https://stackoverflow.com/questions/9072320/split-string-into-string-array
String Display::getStringValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = {0, -1};
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++)
    {
        if (data.charAt(i) == separator || i == maxIndex)
        {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i + 1 : i;
        }
    }

    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

#endif // USE_HIRES_DISPLAY