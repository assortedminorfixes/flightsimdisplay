#ifndef STATE_H
#define STATE_H

#define MESSAGING_START_DELAY 1000

struct DisplayField
{
  float_t value = 0;
  bool dashes = false;
  bool dot = false;
  uint8_t vtype = 3;
  String label;
  String out;

  DisplayField()
  {
    label.reserve(10);
    out.reserve(30);
  }

  String to_string() {
    out = "";
    out += value;
    out += ',';
    out += String(dashes);
    out += ',';
    out += String(dot);
    out += ',';
    out += label;
    return out;
  }

};

struct NAVData
{
  DisplayField alt;
  DisplayField vs;
  DisplayField ias;
  DisplayField hdg;
  uint8_t crs_sel = 0;
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
  uint8_t sel = 2;
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
  bool debug = false;
  bool serial_debug = false;

  bool isReady()
  {
    return ((millis() - start_time) > MESSAGING_START_DELAY);
  }
};

extern struct State state;

#endif
