#ifndef STATE_H
#define STATE_H

#define MESSAGING_START_DELAY 1000

struct NAVData
{
    int32_t alt = 0;
    float_t speed = 0;
    uint8_t speed_mode_sel = 0;
    int16_t hdg = 0;
    uint8_t crs_sel = 0;
    int16_t crs = 0;
    float_t baro = 0.0;
    uint8_t baro_mode_sel = 0;
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

struct State
{
  RadioData radio;
  NAVData nav;
  unsigned long last_touch = 0;
  unsigned long start_time = 0;
  bool power = false;
  bool display_static = false;
  bool display_off = false;
  bool configured = false;
  bool debug = true;

  bool isReady () {
    return ((millis() - start_time) > MESSAGING_START_DELAY );
  }

};

extern struct State state;

#endif
