#include <Adafruit_NeoPixel.h>

#define STRIP_1_PIN 12
#define STRIP_1_LEN 196
#define STRIP_2_PIN 13
#define STRIP_2_LEN 75

Adafruit_NeoPixel strip_1(STRIP_1_LEN, STRIP_1_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip_2(STRIP_2_LEN, STRIP_2_PIN, NEO_GRB + NEO_KHZ800);

// ----- Christmas Palette -----
uint32_t XmasRed(Adafruit_NeoPixel &s)    { return s.Color(255, 0, 0); }
uint32_t XmasGreen(Adafruit_NeoPixel &s)  { return s.Color(0, 255, 0); }
uint32_t XmasGold(Adafruit_NeoPixel &s)   { return s.Color(255, 180, 40); }

uint32_t ChristmasColor(int pos, Adafruit_NeoPixel &s) {
  // Alternate red → green
  if (pos % 2 == 0) return XmasRed(s);
  return XmasGreen(s);
}

void fill_strip_xmas(Adafruit_NeoPixel &strip, int offset);
void sparkle_xmas(Adafruit_NeoPixel &strip, int chance);
void rain_effect_xmas(Adafruit_NeoPixel &strip, int chunks, int offset);
void xmas_chase(Adafruit_NeoPixel &strip, int offset);
void eq_xmas(Adafruit_NeoPixel &strip, int chunks, int volume);

int iteration = 0;
int restart_after_iteration = 5000;

void setup() {
  strip_1.begin();
  strip_2.begin();

  strip_1.setBrightness(150);
  strip_2.setBrightness(150);

  strip_1.clear();
  strip_2.clear();

  strip_1.show();
  strip_2.show();
}

// ----------------------------------------------------
// LOOP
// ----------------------------------------------------
void loop() {
  if (iteration < 500) {
    fill_strip_xmas(strip_1, iteration);
    fill_strip_xmas(strip_2, iteration);

  } else if (iteration < 2000) {
    sparkle_xmas(strip_2, 5);
    rain_effect_xmas(strip_1, 4, iteration);

  } else if (iteration < 4000) {
    int volume = (iteration / 10);
    eq_xmas(strip_1, 4, volume);
    eq_xmas(strip_2, 4, volume);

  } else {
    xmas_chase(strip_1, iteration);
    xmas_chase(strip_2, iteration);
  }

  strip_1.show();
  strip_2.show();

  iteration++;
  if (iteration >= restart_after_iteration) iteration = 0;
}

// ----------------------------------------------------
// EFFECTS
// ----------------------------------------------------

// Alternating red/green fill with slow offset
void fill_strip_xmas(Adafruit_NeoPixel &strip, int offset) {
  for (int i = 0; i < strip.numPixels(); i++) {
    int index = (i + offset / 5) % strip.numPixels();
    strip.setPixelColor(i, ChristmasColor(index, strip));
  }
}

// Sparkles in warm gold / red / green
void sparkle_xmas(Adafruit_NeoPixel &strip, int chance) {
  for (int i = 0; i < strip.numPixels(); i++) {
    if (random(0, 100) < chance) {
      int t = random(0, 3);
      if (t == 0) strip.setPixelColor(i, XmasRed(strip));
      else if (t == 1) strip.setPixelColor(i, XmasGreen(strip));
      else strip.setPixelColor(i, XmasGold(strip));
    } else {
      strip.setPixelColor(i, 0, 0, 0);
    }
  }
}

// Raining gold/red drops
void rain_effect_xmas(Adafruit_NeoPixel &strip, int chunks, int offset) {
  int chunk_length = strip.numPixels() / chunks;
  int gap = 25;

  for (int i = 0; i < chunk_length; i++) {
    int pos = i % gap;
    int brightness = 0;

    if (pos == 9) brightness = 255;
    else if (pos == 8) brightness = 120;
    else if (pos == 7) brightness = 40;

    for (int r = 0; r < chunks; r++) {
      int pixel = (r * chunk_length) + i;
      int randShift = r * 17;

      uint32_t base =
        (random(0, 2) == 0) ? XmasGold(strip) : XmasRed(strip);

      uint8_t R = (base >> 16) & 0xFF;
      uint8_t G = (base >> 8) & 0xFF;
      uint8_t B = base & 0xFF;

      strip.setPixelColor(
        (pixel + offset + randShift) % strip.numPixels(),
        strip.Color(R * brightness / 255, G * brightness / 255, B * brightness / 255)
      );
    }
  }
}

// Christmas chase with tail (red → green → gold)
void xmas_chase(Adafruit_NeoPixel &strip, int offset) {
  int n = strip.numPixels();
  int trail = 10;

  for (int i = 0; i < n; i++) {
    int head = (offset / 2) % n;
    int dist = (i - head + n) % n;

    if (dist < trail) {
      float fade = 1.0 - (float)dist / trail;

      uint32_t base;
      if (dist < 2) base = XmasGold(strip);
      else if (dist < 5) base = XmasGreen(strip);
      else base = XmasRed(strip);

      uint8_t r = ((base >> 16) & 0xFF) * fade;
      uint8_t g = ((base >> 8) & 0xFF) * fade;
      uint8_t b = (base & 0xFF) * fade;

      strip.setPixelColor(i, r, g, b);
    } else {
      strip.setPixelColor(i, 0, 0, 0);
    }
  }
}

// Eq bars in Christmas colours                                                                                                                                                                                                                                                              
void eq_xmas(Adafruit_NeoPixel &strip, int chunks, int volume) {  
  int virtual_chunks = (chunks * 2) - 1;
  int quant = volume % (virtual_chunks + 1);
  int toLight = (quant > chunks) ? (chunks * 2 - quant) : quant;

  strip.clear();
  int px = strip.numPixels() / chunks;

  for (int c = 0; c < chunks; c++) {
    if (c < toLight) {
      for (int p = 0; p < px; p++) {

        int type = (c + p) % 3;
        uint32_t col =
          (type == 0) ? XmasRed(strip) :
          (type == 1) ? XmasGreen(strip) :
                        XmasGold(strip);

        strip.setPixelColor(c * px + p, col);
      }
    }
  }
}
