const uint8_t B612Mono_Regular18ptDashBitmaps[] PROGMEM = {
  0xFF, 0xFF, 0xFF, 0xFF, 0xFE,
  0x7B, 0xFF, 0xFF, 0xFD, 0xE0 };

const GFXglyph B612Mono_Regular18ptDashGlyphs[] PROGMEM = {
  {     0,  13,   3,  23,    5,  -10 },    // 0x2013
  {     5,   6,   6,  23,    2,  -5  } };  // 0x2E '.'

const GFXfont B612Mono_Regular18ptDash PROGMEM = {
  (uint8_t  *)B612Mono_Regular18ptDashBitmaps,
  (GFXglyph *)B612Mono_Regular18ptDashGlyphs,
  0x2D, 0x2E, 43 };
// Referencing em dash (0x2013) here as normal minus (0x2D)
// Approx. 19 bytes
