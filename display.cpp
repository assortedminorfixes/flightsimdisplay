#include "Arduino.h"

#include "display.hh"
#include "messaging.hh"
#include "featherwing_touch.hh"

#include "fonts/b612reg10pt.h"
#include "fonts/b612bold7pt.h"
#include "fonts/b612bold16pt.h"
#include "fonts/b612monoreg18pt.h"
#include "fonts/b612monoreg24pt.h"
#include "fonts/b612monoreg24pt-deg.h"

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

#define HX8357_CUST_GRAY 0x9CD3

#define CANVAS_NUM_H 45
#define CANVAS_NUM_W 160
#define CANVAS_NUM_UL_X 5
#define CANVAS_NUM_UL_Y 35
#define CANVAS_NUM_LARGE_H 50
#define CANVAS_NUM_LARGE_W 160
#define CANVAS_CENTER_H 60
#define CANVAS_CENTER_W 200

#define RADIO_BUTTON_H 30
#define RADIO_BUTTON_V 50
#define RADIO_BUTTON_RAD 1
#define RADIO_BUTTON_POS_Y 90
#define RADIO_BUTTON_TXTPOS_Y (RADIO_BUTTON_POS_Y + 20)

#define DATA_RADIO_ACT_POS_X 0
#define DATA_RADIO_ACT_POS_Y 25

#define DATA_RADIO_STANDBY_POS_X 160
#define DATA_RADIO_STANDBY_POS_Y 25

#define DATA_ALT_POS_X 0
#define DATA_ALT_POS_Y 180

#define DATA_VS_POS_X 160
#define DATA_VS_POS_Y 180

#define DATA_HDG_POS_X 0
#define DATA_HDG_POS_Y 265

#define DATA_CRS_POS_X 160
#define DATA_CRS_POS_Y 265

#define DATA_XPDR_POS_X 0
#define DATA_XPDR_POS_Y 360

#define DATA_BARO_POS_X 160
#define DATA_BARO_POS_Y 360

#define CENTER_LABEL_POS_X 60
#define CENTER_LABEL_POS_Y 160

#define DECIMAL_PAD 5

#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000

static const char *const RADIO_BUTTON_LABEL[] = {"NAV1", "NAV2", "COM1", "COM2",
                                                 "ADF"};

Display::Display() : cCenter(CANVAS_CENTER_W, CANVAS_CENTER_H),
                     lcd(TFT_CS, TFT_DC, TFT_RST),
                     ts(STMPE_CS)
{
}

void Display::initDisplay()
{
    // LCD SETUP
    lcd.begin();
    lcd.fillScreen(HX8357_BLACK); // initialize the lcd
    lcd.setRotation(0);
    lcd.setTextSize(1);

    // Start Touch Screen
    ts.begin();

    cCenter.fillScreen(HX8357_BLACK);
    cCenter.setFont(&B612_Bold16pt7b);
    cCenter.setCursor(5, 50);
    cCenter.print(F("Initializing"));
    lcd.drawBitmap(CENTER_LABEL_POS_X, CENTER_LABEL_POS_Y, cCenter.getBuffer(), CANVAS_CENTER_W, CANVAS_CENTER_H, HX8357_WHITE);
}

TouchEvent Display::processTouch()
{
    int8_t nav_sel = -1;
    int8_t crs_sel = -1;

    if (!ts.bufferEmpty())
    {
        while (!ts.bufferEmpty())
        {
            TS_Point p = ts.getPoint();
            p.x = map(p.x, TS_MINX, TS_MAXX, 0, lcd.width());
            p.y = map(p.y, TS_MINY, TS_MAXY, 0, lcd.height());

            if (p.y > RADIO_BUTTON_POS_Y && p.y < (RADIO_BUTTON_POS_Y + RADIO_BUTTON_H))
            {
                for (int i = 0; i < 5; i++)
                {
                    if (p.x > (15 + (RADIO_BUTTON_V + 10) * i) && p.x < (5 + (RADIO_BUTTON_V + 10) * (i + 1)))
                    {
                        lcd.drawCircle(p.x, p.y, 1, HX8357_CYAN);
                        nav_sel = i;
                    }
                }
            }
            else if (p.y > 260 && p.y < 325 && p.x > 160)
            {
                lcd.drawCircle(p.x, p.y, 1, HX8357_CYAN);
                crs_sel = 1;
            }
        }
        if (nav_sel > -1 && (nav_sel != radio.sel))
        {
            radio.sel = nav_sel;
            update.radio_buttons = true;
            TouchEvent te;
            te.event = TouchEventType::NAV_BUTTON;
            te.value = nav_sel;
            return te;
        }
        else if (crs_sel > -1)
        {
            nav_data.crs_sel = (nav_data.crs_sel + 1) % MSG_COURSES;
            update.crs = true;
            TouchEvent te;
            te.event = TouchEventType::CRS_BUTTON;
            te.value = nav_data.crs_sel;
            return te;
        }
        else
        {
            TouchEvent te;
            return te;
        }
    }
    else
    {
        TouchEvent te;
        return te;
    }
}

void Display::clearTouch() {
    while (!ts.bufferEmpty()) {
        ts.getPoint();
    }
}

void Display::printSplash(String str)
{
    lcd.fillScreen(HX8357_BLACK);
    cCenter.fillScreen(HX8357_BLACK);
    cCenter.setFont(&B612_Bold16pt7b);
    cCenter.setCursor(5, 50);
    cCenter.print(str);
    lcd.drawBitmap(CENTER_LABEL_POS_X, CENTER_LABEL_POS_Y, cCenter.getBuffer(), CANVAS_CENTER_W, CANVAS_CENTER_H, HX8357_WHITE);
}

void Display::printStatic()
{
    lcd.fillScreen(HX8357_BLACK); // initialize the lcd

    lcd.setTextColor(HX8357_GREEN);
    lcd.setFont(&B612_Regular10pt7b);
    lcd.setCursor(5, 20);
    lcd.print(F("Active"));
    lcd.setCursor(165, 20);
    lcd.print(F("Standby"));

    lcd.drawFastHLine(0, 145, 320, HX8357_WHITE);

    lcd.setCursor(5, 175);
    lcd.print(F("Altitude"));
    lcd.setCursor(165, 175);
    lcd.print(F("VS"));

    lcd.drawFastHLine(0, 230, 320, HX8357_WHITE);

    lcd.setCursor(5, 260);
    lcd.print(F("Heading"));
    lcd.setCursor(165, 260);
    lcd.print(F("Course"));

    lcd.drawFastHLine(0, 325, 320, HX8357_WHITE);

    lcd.setCursor(5, 355);
    lcd.print(F("XPDR"));
    lcd.setCursor(165, 355);
    lcd.print(F("Barometer"));

    lcd.drawFastHLine(0, 410, 320, HX8357_WHITE);
}

void Display::printButtons(uint8_t active)
{

    uint16_t colors[5] = {HX8357_CUST_GRAY, HX8357_CUST_GRAY, HX8357_CUST_GRAY, HX8357_CUST_GRAY, HX8357_CUST_GRAY};
    colors[active] = HX8357_WHITE;

    lcd.setTextColor(HX8357_BLACK);
    lcd.setFont(&B612_Bold7pt7b);

    uint8_t button_pos_v = 0;
    for (int i = 0; i < 5; i++)
    {
        button_pos_v = 15 + (RADIO_BUTTON_V + 10) * i;
        lcd.fillRoundRect(button_pos_v, RADIO_BUTTON_POS_Y, RADIO_BUTTON_V, RADIO_BUTTON_H, RADIO_BUTTON_RAD, colors[i]);
        lcd.setCursor(button_pos_v + 5, RADIO_BUTTON_TXTPOS_Y);
        lcd.print(F(RADIO_BUTTON_LABEL[i]));
    }

    update.radio_buttons = false;
}

void Display::redraw()
{
    if (update.alt)
        drawAltitude();
    else if (update.hdg)
        drawHeading();
    else if (update.crs)
        drawCourse();
    else if (update.vs)
        drawVerticalSpeed();
    else if (update.xpdr)
        drawTransponderCode();
    else if (update.radio_buttons)
        printButtons(radio.sel);
    else if (update.radio_active)
        drawRadioActive();
    else if (update.radio_standby)
        drawRadioStandby();
    else if (update.baro)
        drawBarometer();
}

void Display::setAltitude(int32_t alt)
{
    nav_data.alt = alt;
    update.alt = true;
}

void Display::drawAltitude()
{
    GFXcanvas1 canvas(CANVAS_NUM_W, CANVAS_NUM_H);
    canvas.setFont(&B612Mono_Regular18pt7b);
    canvas.setCursor(CANVAS_NUM_UL_X, CANVAS_NUM_UL_Y);
    if (nav_data.alt != 0)
        canvas.printf("%5i", nav_data.alt);
    else
        canvas.printf("-----", nav_data.alt);
    lcd.drawBitmap(DATA_ALT_POS_X, DATA_ALT_POS_Y, canvas.getBuffer(), CANVAS_NUM_W, CANVAS_NUM_H, HX8357_WHITE, HX8357_BLACK);
    update.alt = false;
}

void Display::setVerticalSpeed(int16_t vs)
{
    nav_data.vs = vs;
    update.vs = true;
}

void Display::drawVerticalSpeed()
{
    GFXcanvas1 canvas(CANVAS_NUM_W, CANVAS_NUM_H);
    canvas.setFont(&B612Mono_Regular18pt7b);
    canvas.setCursor(CANVAS_NUM_UL_X, CANVAS_NUM_UL_Y);
    if (nav_data.vs != 0)
    {
        canvas.printf("%5i", nav_data.vs);
        lcd.drawBitmap(DATA_VS_POS_X, DATA_VS_POS_Y, canvas.getBuffer(), CANVAS_NUM_W, CANVAS_NUM_H, HX8357_CYAN, HX8357_BLACK);
    }
    else
    {
        canvas.printf("-----");
        lcd.drawBitmap(DATA_VS_POS_X, DATA_VS_POS_Y, canvas.getBuffer(), CANVAS_NUM_W, CANVAS_NUM_H, HX8357_CYAN, HX8357_BLACK);
    }
    update.vs = false;
}

void Display::setHeading(int16_t hdg)
{
    if (hdg == 0)
        nav_data.hdg = 360;
    else
        nav_data.hdg = hdg;

    update.hdg = true;
}

void Display::drawHeading()
{
    GFXcanvas1 canvas(CANVAS_NUM_LARGE_W, CANVAS_NUM_LARGE_H);
    canvas.setFont(&B612Mono_Regular24pt7b);
    canvas.setCursor(5, 45);
    canvas.printf("%03i", nav_data.hdg);
    canvas.setFont(&B612Mono_Regular24pt8b);
    canvas.printf(deg);
    lcd.drawBitmap(DATA_HDG_POS_X, DATA_HDG_POS_Y, canvas.getBuffer(), CANVAS_NUM_LARGE_W, CANVAS_NUM_LARGE_H, HX8357_WHITE, HX8357_BLACK);
    update.hdg = false;
}

void Display::setCourse(int16_t crs)
{
    if (crs == 0)
        nav_data.crs = 360;
    else
        nav_data.crs = crs;

    update.crs = true;
}

void Display::updateCourseLabel(uint8_t selection)
{
    lcd.setFont(&B612_Regular10pt7b);
    lcd.setTextColor(HX8357_BLACK);
    lcd.setCursor(165, 260);
    lcd.printf("Course %s ", crs_subscribe[(selection + (MSG_COURSES - 1)) % MSG_COURSES][1]);

    lcd.setTextColor(HX8357_GREEN);
    lcd.setCursor(165, 260);
    lcd.printf("Course %s ", crs_subscribe[selection][1]);
}

void Display::drawCourse()
{

    GFXcanvas1 canvas(CANVAS_NUM_LARGE_W, CANVAS_NUM_LARGE_H);
    canvas.setFont(&B612Mono_Regular24pt7b);
    canvas.setCursor(5, 45);
    canvas.printf("%03i", nav_data.crs);
    canvas.setFont(&B612Mono_Regular24pt8b);
    canvas.printf(deg);
    lcd.drawBitmap(DATA_CRS_POS_X, DATA_CRS_POS_Y, canvas.getBuffer(), CANVAS_NUM_LARGE_W, CANVAS_NUM_LARGE_H, HX8357_WHITE, HX8357_BLACK);
    update.crs = false;
}

void Display::setTransponderCode(int16_t xpdr)
{
    radio.xpdr = xpdr;
    update.xpdr = true;
}

void Display::drawTransponderCode()
{
    GFXcanvas1 canvas(CANVAS_NUM_W, CANVAS_NUM_H);
    canvas.setFont(&B612Mono_Regular18pt7b);
    canvas.setCursor(CANVAS_NUM_UL_X, CANVAS_NUM_UL_Y);
    if (radio.xpdr != 0)
    {
        canvas.printf("%04i", radio.xpdr);
    }
    else
    {
        canvas.printf("----");
    }
    lcd.drawBitmap(DATA_XPDR_POS_X, DATA_XPDR_POS_Y, canvas.getBuffer(), CANVAS_NUM_W, CANVAS_NUM_H, HX8357_WHITE, HX8357_BLACK);
    update.xpdr = false;
}

void Display::setBarometer(float_t baro)
{
    nav_data.baro = baro;
    update.baro = true;
}

void Display::drawBarometer()
{
    GFXcanvas1 canv = this->trimDecimal(nav_data.baro, 2, 2, CANVAS_NUM_UL_X, CANVAS_NUM_UL_Y, &B612Mono_Regular18pt7b);
    lcd.drawBitmap(DATA_BARO_POS_X, DATA_BARO_POS_Y, canv.getBuffer(), CANVAS_NUM_W, CANVAS_NUM_H, HX8357_WHITE, HX8357_BLACK);
    update.baro = false;
}

void Display::setActiveRadio(uint8_t sel)
{
    radio.sel = sel;
    update.radio_active = true;
    update.radio_standby = true;
    update.radio_buttons = true;
}

void Display::setRadioFrequencyActive(float_t freq)
{
    radio.freq.active = freq;
    update.radio_active = true;
}

void Display::setRadioFrequencyStandby(float_t freq)
{
    radio.freq.standby = freq;
    update.radio_standby = true;
}

void Display::drawRadioActive()
{
    uint8_t frac_digits = 3;
    uint8_t pad_digits = 3;

    if (radio.sel <= 1)
        frac_digits = 2;
    else if (radio.sel == 4) {
        pad_digits = 4;
        frac_digits = 1;
    }

    GFXcanvas1 canv = this->trimDecimal(radio.freq.active, pad_digits, frac_digits, CANVAS_NUM_UL_X, CANVAS_NUM_UL_Y, &B612Mono_Regular18pt7b);
    lcd.drawBitmap(DATA_RADIO_ACT_POS_X, DATA_RADIO_ACT_POS_Y, canv.getBuffer(), CANVAS_NUM_W, CANVAS_NUM_H, HX8357_WHITE, HX8357_BLACK);
    update.radio_active = false;
}

void Display::drawRadioStandby()
{
    uint8_t frac_digits = 3;
    uint8_t pad_digits = 3;

    if (radio.sel <= 1)
        frac_digits = 2;
    else if (radio.sel == 4) {
        frac_digits = 1;
        pad_digits = 4;
    }

    GFXcanvas1 canv = this->trimDecimal(radio.freq.standby, pad_digits, frac_digits, CANVAS_NUM_UL_X, CANVAS_NUM_UL_Y, &B612Mono_Regular18pt7b);
    lcd.drawBitmap(DATA_RADIO_STANDBY_POS_X, DATA_RADIO_STANDBY_POS_Y, canv.getBuffer(), CANVAS_NUM_W, CANVAS_NUM_H, HX8357_CYAN, HX8357_BLACK);
    update.radio_standby = false;
}

void Display::lastCommand(uint8_t command, uint8_t idx, int32_t val)
{
    lcd.setTextColor(HX8357_CYAN, HX8357_BLACK);
    lcd.setFont(NULL);
    lcd.setCursor(5, 465);
    lcd.printf("Cmd: %2d %2d %d        ", command, idx, val);
}

void Display::printMem()
{
    lcd.setTextColor(HX8357_CYAN, HX8357_BLACK);
    lcd.setFont(NULL);
    lcd.setCursor(5, 455);
    lcd.printf("Mem: %d             ", freeMemory());
}

void Display::printDebug(String msg)
{
    lcd.setTextColor(HX8357_CYAN, HX8357_BLACK);
    lcd.setFont(NULL);
    lcd.setCursor(165, 465);
    lcd.printf(msg.c_str());
}

GFXcanvas1 Display::trimDecimal(float_t num, uint8_t padding, uint8_t decimals, int x, int y, const GFXfont *font)
{

    GFXcanvas1 canvas(CANVAS_NUM_W, CANVAS_NUM_H);
    String numStr;
    if (num != 0.0)
        numStr = String(num, decimals);
    else
    {
        numStr = "";
        for (int i = 0; i < padding; i++)
            numStr.concat('-');
        numStr.concat('.');
        for (int i = 0; i < decimals; i++)
            numStr.concat('-');
    }

    String numStr_int = getStringValue(numStr, '.', 0);
    String numStr_int_pad = "";
    uint8_t pads = (padding)-numStr_int.length();
    for (int i = 0; i < pads; i++)
    {
        numStr_int_pad.concat('0');
    }
    numStr_int_pad.concat(numStr_int);
    numStr_int_pad.concat('.');

    String numStr_frac = getStringValue(numStr, '.', 1);

    // canvas.fillScreen(HX8357_BLACK);
    canvas.setFont(font);
    canvas.setCursor(x, y);

    int16_t x1, y1;
    uint16_t w, h;
    canvas.getTextBounds(numStr_int_pad.c_str(), x, y, &x1, &y1, &w, &h); // calc width of new string
    canvas.printf("%s", numStr_int_pad.c_str());
    canvas.setCursor(x + w + DECIMAL_PAD, y);
    canvas.printf("%s", numStr_frac.c_str());

    return canvas;
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
