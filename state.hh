#ifndef STATE_H
#define STATE_H

struct State
{
  uint8_t radio = 0;
  uint8_t crs = 0;
  long last_touch = 0;
};

extern struct State state;

#endif
