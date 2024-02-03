#ifndef FEATHERWING_TOUCH_H
#define FEATHERWING_TOUCH_H

#ifdef ESP8266
#define STMPE_CS 16
#define TFT_CS 0
#define TFT_DC 15
#define SD_CS 2
#elif defined(ESP32) && !defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2)
#define STMPE_CS 32
#define TFT_CS 15
#define TFT_DC 33
#define SD_CS 14
#elif defined(TEENSYDUINO)
#define TFT_DC 15
#define TFT_CS 13
#define TFT_DIN 8
#define TFT_RST 12
#define TFT_DOUT 11
#define TFT_CLK 14
#define STMPE_CS 3
#define SD_CS 8

#define ENC1P1 0
#define ENC1P2 1
#define ENC1SW1 2

#elif defined(ARDUINO_STM32_FEATHER)
#define TFT_DC PB4
#define TFT_CS PA15
#define STMPE_CS PC7
#define SD_CS PC5
#elif defined(ARDUINO_NRF52832_FEATHER) /* BSP 0.6.5 and higher! */
#define TFT_DC 11
#define TFT_CS 31
#define STMPE_CS 30
#define SD_CS 27
#elif defined(ARDUINO_MAX32620FTHR) || defined(ARDUINO_MAX32630FTHR)
#define TFT_DC P5_4
#define TFT_CS P5_3
#define STMPE_CS P3_3
#define SD_CS P3_2
#else
// Anything else, defaults!
#define STMPE_CS 6
#define TFT_CS 9
#define TFT_DC 10
#define SD_CS 5
#endif


#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000

#define DEBOUNCETIME 30

#define TS_DOUBLETOUCH_DELAY 150

#endif