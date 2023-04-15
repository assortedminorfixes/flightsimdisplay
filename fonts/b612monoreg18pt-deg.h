const uint8_t B612Mono_Regular18ptDegBitmaps[] PROGMEM = {
  0x1C, 0x3F, 0x98, 0xD8, 0x3C, 0x1E, 0x0F, 0x06, 0xC6, 0x7F, 0x0E, 0x00 };

const GFXglyph B612Mono_Regular18ptDegGlyphs[] PROGMEM = {
  {     0,   9,  10,  23,    4,  -26 } }; // 0xB0

const GFXfont B612Mono_Regular18ptDeg PROGMEM = {
  (uint8_t  *)B612Mono_Regular18ptDegBitmaps,
  (GFXglyph *)B612Mono_Regular18ptDegGlyphs,
  0xB0, 0xB0, 43 };

// Approx. 26 bytes
