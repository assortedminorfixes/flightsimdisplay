const uint8_t B612Mono_Regular24ptDashBitmaps[] PROGMEM = {
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0,
  0x3C, 0x7E, 0xFF, 0xFF, 0xFF, 0x7E, 0x3C };

const GFXglyph B612Mono_Regular24ptDashGlyphs[] PROGMEM = {
  {     0,  17,   4,  31,    7,  -13 },
  {     9,   8,   7,  31,    3,   -6 } }; // 0x2013

const GFXfont B612Mono_Regular24ptDash PROGMEM = {
  (uint8_t  *)B612Mono_Regular24ptDashBitmaps,
  (GFXglyph *)B612Mono_Regular24ptDashGlyphs,
  0x2D, 0x2E, 57 };
// Referencing em dash (0x2013) here as normal minus (0x2D)
// Approx. 23 bytes
