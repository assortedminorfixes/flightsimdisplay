#ifdef USE_HIRES_DISPLAY
#include <Arduino.h>

#include "hires_display.hh"
#include "messaging.hh"
#include "featherwing_touch.hh"
#include "state.hh"
#include "Serial_GFX.h"

#include <SPI.h>

/*
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

template <typename T>
int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}
*/

#define HX8357_CUST_GRAY 0x9CD3

#define CANVAS_NUM_H 45
#define CANVAS_NUM_W 160
#define CANVAS_NUM_UL_X 5
#define CANVAS_NUM_UL_Y 35
#define CANVAS_NUM_LARGE_H 50
#define CANVAS_NUM_LARGE_W 160
#define CANVAS_NUM_LARGE_UL_X 5
#define CANVAS_NUM_LARGE_UL_Y 45
#define CANVAS_CENTER_H 60
#define CANVAS_CENTER_W 200
#define CANVAS_LABEL_H 20
#define CANVAS_LABEL_W 160

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

#define DATA_BARO_POS_X 160
#define DATA_BARO_POS_Y 360
#define LABEL_BARO_POS_X 160
#define LABEL_BARO_POS_Y 340

#define CENTER_LABEL_POS_X 60
#define CENTER_LABEL_POS_Y 160

#define DECIMAL_PAD 5
#define DEBUG_STR_LENGTH 25

#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000


/*
Virtual_HX8757::Virtual_HX8757(uint8_t pin1, uint8_t pin2, uint8_t pin3) : Serial_GFX(320, 480)
{
    raw = true;
}
*/

Hires_Display::Hires_Display() : cCenter(CANVAS_CENTER_W, CANVAS_CENTER_H),
                     disp_device(TFT_DC, TFT_RST, TFT_CS)
                     //,ts(STMPE_CS)
{
}

void Hires_Display::initDisplay()
{
    // LCD SETUP
    disp_device.hardReset();
    disp_device.fillScreen(0); // initialize the lcd
    disp_device.setRotation(0);
    //lcd.setTextSize(1);

    // Start Touch Screen
    //ts.begin();

    cCenter.fillScreen(0);
    cCenter.setFont(font_var_title.normal);
    cCenter.setCursor(5, 32);
    cCenter.print(F("Initializing"));
    disp_device.drawBitmap(CENTER_LABEL_POS_X, CENTER_LABEL_POS_Y, cCenter.getBuffer(), CANVAS_CENTER_W, CANVAS_CENTER_H, 1);
    disp_device.printSerial(1);
}

TouchEvent Hires_Display::processTouch()
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

void Hires_Display::clearTouch()
{
    while (!ts.bufferEmpty())
    {
        ts.getPoint();
    }
}

void Hires_Display::printSplash(String str)
{
    state.display_static = false;
    disp_device.fillScreen(0);
    cCenter.fillScreen(0);
    cCenter.setFont(font_var_title.normal);
    cCenter.setCursor(5, 31);
    cCenter.print(str);
    disp_device.drawBitmap(CENTER_LABEL_POS_X, CENTER_LABEL_POS_Y, cCenter.getBuffer(), CANVAS_CENTER_W, CANVAS_CENTER_H, 1);
}

void Hires_Display::printStatic()
{
    disp_device.fillScreen(0); // initialize the lcd

    disp_device.setTextColor(1);
    disp_device.setFont(font_var_lbl.normal);

    this->drawLabel(LABEL_RADIO_ACT_POS_X, LABEL_RADIO_ACT_POS_Y, F("Active"));
    this->drawLabel(LABEL_RADIO_STANDBY_POS_X, LABEL_RADIO_STANDBY_POS_Y, F("Standby"));

    disp_device.drawFastHLine(0, 145, 320, 1);
    this->drawLabel(LABEL_ALT_POS_X, LABEL_ALT_POS_Y, F("Altitude"));

    disp_device.drawFastHLine(0, 230, 320, 1);

    disp_device.drawFastHLine(0, 325, 320, 1);
    this->drawLabel(LABEL_XPDR_POS_X, LABEL_XPDR_POS_Y, F("XPDR"));

    disp_device.drawFastHLine(0, 410, 320, 1);

    state.display_static = true;
    state.display_off = false;
}

void Hires_Display::drawRadioSelectButton(uint8_t active)
{

    uint16_t colors[5] = {HX8357_CUST_GRAY, HX8357_CUST_GRAY, HX8357_CUST_GRAY, HX8357_CUST_GRAY, HX8357_CUST_GRAY};
    colors[active] = 1;

    disp_device.setTextColor(0);
    disp_device.setFont(font_var_lbl_b.normal);

    uint8_t button_pos_v = 0;
    for (int i = 0; i < 5; i++)
    {
        button_pos_v = 15 + (RADIO_BUTTON_V + 10) * i;
        disp_device.fillRoundRect(button_pos_v, RADIO_BUTTON_POS_Y, RADIO_BUTTON_V, RADIO_BUTTON_H, RADIO_BUTTON_RAD, colors[i]);
        disp_device.setCursor(button_pos_v + 5, RADIO_BUTTON_TXTPOS_Y);
        disp_device.print(F(RADIO_BUTTON_LABEL[i]));
    }

    update.radio_select_buttons = false;
}

void Hires_Display::drawLabel(uint16_t x, uint16_t y, const char *label)
{
    GFXcanvas1 canvas(CANVAS_LABEL_W, CANVAS_LABEL_H);
    canvas.setFont(font_var_lbl.normal);
    canvas.setCursor(5, 15);
    canvas.printf("%s", label);
    disp_device.drawBitmap(x, y, canvas.getBuffer(), CANVAS_LABEL_W, CANVAS_LABEL_H, 1, 0);
}

void Hires_Display::drawLabel(uint16_t x, uint16_t y, String label)
{
    this->drawLabel(x, y, label.c_str());
}

void Hires_Display::redraw(bool full)
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

void Hires_Display::updateAltitude()
{
    update.alt = true;
}

void Hires_Display::drawAltitude()
{
    GFXcanvas1 canvas(CANVAS_NUM_W, CANVAS_NUM_H);
    canvas.setFont(font_mono_val_s.normal);
    canvas.setCursor(CANVAS_NUM_UL_X, CANVAS_NUM_UL_Y);
    if (state.nav.alt.value != 0)
    {
        canvas.printf("%5i", (uint16_t)state.nav.alt.value);
    }
    else
    {
        printDash(5, &canvas, &font_mono_val_s);
    }
    if (state.nav.alt.dot == true)
    {
        canvas.setFont(font_mono_val_s.dot);
        canvas.printf(SYM_DOT);
    }

    disp_device.drawBitmap(DATA_ALT_POS_X, DATA_ALT_POS_Y, canvas.getBuffer(), CANVAS_NUM_W, CANVAS_NUM_H, 1, 0);
    update.alt = false;
}

void Hires_Display::updateVS()
{
    update.vs = true;
}

void Hires_Display::updateVSLabel()
{
    update.vs_lbl = true;
    update.vs = true;
}

void Hires_Display::drawVSLabel()
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

void Hires_Display::drawVS()
{
    GFXcanvas1 canvas(CANVAS_NUM_W, CANVAS_NUM_H);
    canvas.setFont(font_mono_val_s.normal);
    canvas.setCursor(CANVAS_NUM_UL_X, CANVAS_NUM_UL_Y);

    if (state.nav.vs.dashes)
    {
        printDash(5, &canvas, &font_mono_val_s);
    }
    else if (fabsf(state.nav.vs.value) > 10.0) // VS
    {
        this->trimDecimal(state.nav.vs.value, 4, 0, false, true, CANVAS_NUM_UL_X, CANVAS_NUM_UL_Y, &canvas, &font_mono_val_s);
    }
    else // FPA
    {
        this->trimDecimal(state.nav.vs.value, 1, 1, false, true, CANVAS_NUM_UL_X, CANVAS_NUM_UL_Y, &canvas, &font_mono_val_s);
    }

    disp_device.drawBitmap(DATA_SPEED_POS_X, DATA_SPEED_POS_Y, canvas.getBuffer(), CANVAS_NUM_W, CANVAS_NUM_H, 1, 0);
    update.vs = false;
}

void Hires_Display::updateIAS()
{
    update.ias = true;
}

void Hires_Display::updateIASLabel()
{
    update.ias_lbl = true;
    update.ias = true;
}

void Hires_Display::drawIASLabel()
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

void Hires_Display::drawIAS()
{
    GFXcanvas1 canvas(CANVAS_NUM_LARGE_W, CANVAS_NUM_LARGE_H);
    canvas.setFont(font_mono_val_l.normal);
    canvas.setCursor(CANVAS_NUM_LARGE_UL_X, CANVAS_NUM_LARGE_UL_Y);

    if (state.nav.ias.value > 0 and state.nav.ias.value <= 2)
    {
        this->trimDecimal(state.nav.ias.value, 1, 2, false, false, CANVAS_NUM_LARGE_UL_X, CANVAS_NUM_LARGE_UL_Y, &canvas, &font_mono_val_l);
    }
    else if (state.nav.ias.value > 2)
    {
        canvas.printf("%3i", (uint16_t)state.nav.ias.value); // IAS
    }
    else
    {
        printDash(3, &canvas, &font_mono_val_l);
    }

    if (state.nav.ias.dot == true)
    {
        canvas.setFont(font_mono_val_l.dot);
        canvas.printf(SYM_DOT);
    }

    disp_device.drawBitmap(DATA_IAS_POS_X, DATA_IAS_POS_Y, canvas.getBuffer(), CANVAS_NUM_LARGE_W, CANVAS_NUM_LARGE_H, 1, 0);
    update.ias = false;
}

void Hires_Display::updateHeading()
{
    update.hdg = true;
}

void Hires_Display::updateHeadingLabel()
{
    update.hdg_lbl = true;
}

void Hires_Display::drawHeading()
{
    int16_t print_hdg;
    if (state.nav.hdg.value == 0)
        print_hdg = 360;
    else
        print_hdg = state.nav.hdg.value;

    GFXcanvas1 canvas(CANVAS_NUM_LARGE_W, CANVAS_NUM_LARGE_H);
    canvas.setFont(font_mono_val_l.normal);
    canvas.setCursor(CANVAS_NUM_LARGE_UL_X, CANVAS_NUM_LARGE_UL_Y);
    if (state.nav.hdg.dashes == true || state.nav.hdg.value < 0)
    {
        printDash(3, &canvas, &font_mono_val_l);
    }
    else
    {
        canvas.printf("%03i", print_hdg);

        if (state.nav.hdg.dot == false)
        {
            canvas.setFont(font_mono_val_l.deg);
            canvas.printf(SYM_DEG);
        }
    }
    if (state.nav.hdg.dot == true)
    {
        canvas.setFont(font_mono_val_l.dot);
        canvas.printf(SYM_DOT);
    }

    disp_device.drawBitmap(DATA_HDG_POS_X, DATA_HDG_POS_Y, canvas.getBuffer(), CANVAS_NUM_LARGE_W, CANVAS_NUM_LARGE_H, 1, 0);
    update.hdg = false;
}

void Hires_Display::drawHeadingLabel()
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

void Hires_Display::updateTransponderCode()
{
    update.xpdr = true;
}

void Hires_Display::drawTransponderCode()
{
    GFXcanvas1 canvas(CANVAS_NUM_W, CANVAS_NUM_H);
    canvas.setFont(font_mono_val_s.normal);
    canvas.setCursor(CANVAS_NUM_UL_X, CANVAS_NUM_UL_Y);
    if (state.radio.xpdr != 0)
    {
        canvas.printf("%04i", state.radio.xpdr);
    }
    else
    {
        printDash(4, &canvas, &font_mono_val_s);
    }
    disp_device.drawBitmap(DATA_XPDR_POS_X, DATA_XPDR_POS_Y, canvas.getBuffer(), CANVAS_NUM_W, CANVAS_NUM_H, 1, 0);
    update.xpdr = false;
}

void Hires_Display::updateBarometer()
{
    update.baro = true;
}

void Hires_Display::updateBarometerLabel(uint8_t selection)
{
    disp_device.setFont(font_var_lbl.normal);
    disp_device.setTextColor(0);
    disp_device.setCursor(165, 355);
    disp_device.printf("%s ", BARO_LABEL[(selection + (this->baro_labels - 1)) % this->baro_labels]);

    disp_device.setTextColor(1);
    disp_device.setCursor(165, 355);
    disp_device.printf("%s ", BARO_LABEL[selection]);

    update.baro = true;
}

void Hires_Display::printSerial()
{
    disp_device.printSerial();
}

void Hires_Display::drawBarometer()
{
    GFXcanvas1 canvas(CANVAS_NUM_W, CANVAS_NUM_H);
    canvas.setFont(font_mono_val_s.normal);
    canvas.setCursor(CANVAS_NUM_UL_X, CANVAS_NUM_UL_Y);

    if (state.nav.baro < 1.0)
    {
        printDash(4, &canvas, &font_mono_val_s);
        disp_device.drawBitmap(DATA_BARO_POS_X, DATA_BARO_POS_Y, canvas.getBuffer(), CANVAS_NUM_W, CANVAS_NUM_H, 1, 0);
    }
    else if (state.nav.baro_mode_sel == 1)
    {
        this->trimDecimal(state.nav.baro, 2, 2, true, false, CANVAS_NUM_UL_X, CANVAS_NUM_UL_Y, &canvas, &font_mono_val_s);
        disp_device.drawBitmap(DATA_BARO_POS_X, DATA_BARO_POS_Y, canvas.getBuffer(), CANVAS_NUM_W, CANVAS_NUM_H, 1, 0);
    }
    else
    {
        canvas.printf("%4i", (int16_t)state.nav.baro); // hPa
        disp_device.drawBitmap(DATA_BARO_POS_X, DATA_BARO_POS_Y, canvas.getBuffer(), CANVAS_NUM_W, CANVAS_NUM_H, 1, 0);
    }
    update.baro = false;
}

void Hires_Display::updateActiveRadio()
{
    update.radio_active = true;
    update.radio_standby = true;
    update.radio_select_buttons = true;
}

void Hires_Display::updateRadioFrequencyActive()
{
    update.radio_active = true;
}

void Hires_Display::updateRadioFrequencyStandby()
{
    update.radio_standby = true;
}

void Hires_Display::drawRadioActive()
{
    uint8_t frac_digits = 3;
    uint8_t pad_digits = 3;

    if (state.radio.sel <= 1)
        frac_digits = 2;
    else if (state.radio.sel == 4)
    {
        pad_digits = 4;
        frac_digits = 1;
    }

    GFXcanvas1 canvas(CANVAS_NUM_W, CANVAS_NUM_H);
    this->trimDecimal(state.radio.freq.active, pad_digits, frac_digits, true, false, CANVAS_NUM_UL_X, CANVAS_NUM_UL_Y, &canvas, &font_mono_val_s);
    disp_device.drawBitmap(DATA_RADIO_ACT_POS_X, DATA_RADIO_ACT_POS_Y, canvas.getBuffer(), CANVAS_NUM_W, CANVAS_NUM_H, 1, 0);
    update.radio_active = false;
}

void Hires_Display::drawRadioStandby()
{
    uint8_t frac_digits = 3;
    uint8_t pad_digits = 3;

    if (state.radio.sel <= 1)
        frac_digits = 2;
    else if (state.radio.sel == 4)
    {
        frac_digits = 1;
        pad_digits = 4;
    }

    GFXcanvas1 canvas(CANVAS_NUM_W, CANVAS_NUM_H);
    this->trimDecimal(state.radio.freq.standby, pad_digits, frac_digits, true, false, CANVAS_NUM_UL_X, CANVAS_NUM_UL_Y, &canvas, &font_mono_val_s);
    disp_device.drawBitmap(DATA_RADIO_STANDBY_POS_X, DATA_RADIO_STANDBY_POS_Y, canvas.getBuffer(), CANVAS_NUM_W, CANVAS_NUM_H, 1, 0);
    update.radio_standby = false;
}

void Hires_Display::printLastCommand(uint8_t command, uint8_t idx, int32_t val)
{
    this->printLastCommand(command, idx, String(val));
}

void Hires_Display::printLastCommand(uint8_t command, uint8_t idx, const String val)
{
    disp_device.setTextColor(1, 0);
    disp_device.setFont(NULL);
    disp_device.setCursor(5, 465);
    dbg_str = F("Cmd: ");
    dbg_str.concat((int)command);
    dbg_str.concat(' ');
    dbg_str.concat((int)idx);
    dbg_str.concat(' ');
    dbg_str.concat(val);

    while (dbg_str.length() < DEBUG_STR_LENGTH)
    {
        dbg_str.concat(' ');
    }
    disp_device.printf("%s", dbg_str.substring(0, min(dbg_str.length(), DEBUG_STR_LENGTH) - 1).c_str());
}

void Hires_Display::printMem()
{
    disp_device.setTextColor(1, 0);
    disp_device.setFontFromNative(1);
    disp_device.setCursor(0, 31);
    disp_device.printf("Mem: %d             ", freeMemory());
}

void Hires_Display::printDebug(String msg)
{
    disp_device.setTextColor(1, 0);
    disp_device.setFontFromNative(1);
    disp_device.setCursor(0, 31);
    disp_device.printf(msg.c_str());
}

void Hires_Display::trimDecimal(float_t num, uint8_t padding, uint8_t decimals, bool dashes, bool force_sign, int x, int y, GFXcanvas1 *canvas, const Fonts *font)
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

void Hires_Display::printDash(uint8_t num, GFXcanvas1 *canvas, const Fonts *font)
{
    canvas->setFont(font->dash);

    for (uint8_t i = 0; i < num; i++)
    {
        canvas->print('-');
    }
}

// https://stackoverflow.com/questions/9072320/split-string-into-string-array
String Hires_Display::getStringValue(String data, char separator, int index)
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
#endif