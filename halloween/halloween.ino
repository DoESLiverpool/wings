#include <Adafruit_NeoPixel.h>

#define STRIP_1_PIN 12
#define STRIP_1_LEN 196   // 4 strips of 49
#define STRIP_2_PIN 13
#define STRIP_2_LEN 75

Adafruit_NeoPixel strip_1(STRIP_1_LEN, STRIP_1_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip_2(STRIP_2_LEN, STRIP_2_PIN, NEO_GRB + NEO_KHZ800);

uint32_t hue_as_color(int hue);
void fill_strip_with_rainbow(Adafruit_NeoPixel &strip, int startingHue = 0);
void sparkle(Adafruit_NeoPixel &strip, int chance = 10);
void rain_effect(Adafruit_NeoPixel &strip, int chunks, int offset = 0);
void rainbow_chase(Adafruit_NeoPixel &strip, int offset = 0);
void eq(Adafruit_NeoPixel &strip, int chunks, int volume = 0);

int iteration = 0;
int restart_after_iteration = 5000;
bool testing = false;
bool logging = false;

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

void loop() {
  if (iteration < 500) {
    fill_strip_with_rainbow(strip_1, iteration * -2);
    fill_strip_with_rainbow(strip_2, iteration * -2);

  } else if (iteration < 2000) {
    sparkle(strip_2, 6);
    rain_effect(strip_1, 4, iteration);

  } else if (iteration < 4000) {
    int volume = iteration / 10;
    eq(strip_1, 4, volume);
    eq(strip_2, 4, volume);

  } else {
    rainbow_chase(strip_1, iteration);
    rainbow_chase(strip_2, iteration);
  }

  strip_1.show();
  strip_2.show();

  iteration = (iteration < restart_after_iteration) ? iteration + 1 : 0;
}

// ------------------------------------------------------------
// MAP HUE 0–255 → HALLOWEEN PALETTE (orange → yellow → green)
// ------------------------------------------------------------
uint32_t hue_as_color(int hue) {
  int startHue = 5000;     // Orange / pumpkin
  int endHue   = 22000;    // Slime-green
  int mappedHue = map(hue % 255, 0, 255, startHue, endHue);
  return strip_1.gamma32(strip_1.ColorHSV(mappedHue));
}

// ------------------------------------------------------------
void fill_strip_with_rainbow(Adafruit_NeoPixel &strip, int startingHue) {
  for (int i = 0; i < strip.numPixels(); i++) {
    int hue = (startingHue + i * 4) % 256;
    strip.setPixelColor(i, hue_as_color(hue));
  }
}

// ------------------------------------------------------------
// FLICKER LIKE CANDLES / EMBERS — NO WHITE
// ------------------------------------------------------------
void sparkle(Adafruit_NeoPixel &strip, int chance) {
  for (int i = 0; i < strip.numPixels(); i++) {
    if (random(0, 100) < chance) {
      strip.setPixelColor(i, hue_as_color(random(140, 255)));
    } else {
      strip.setPixelColor(i, 0, 0, 0);
    }
  }
}

// ------------------------------------------------------------
// GHOSTLY RAINDROPS USING HALLOWEEN PALETTE
// ------------------------------------------------------------
void rain_effect(Adafruit_NeoPixel &strip, int chunks, int offset) {
  int chunk_length = strip.numPixels() / chunks;
  int gap = 25;

  for (int i = 0; i < chunk_length; i++) {
    int brightness = 0;
    int pos = i % gap;

    if (pos == 9) brightness = 255;
    else if (pos == 8) brightness = 100;
    else if (pos == 7) brightness = 40;

    for (int r = 0; r < chunks; r++) {
      int pixel = (r * chunk_length) + i;
      int hue = (offset + i * 6 + r * 10) % 256;

      uint32_t baseColor = hue_as_color(hue);
      uint8_t rr = (baseColor >> 16) & 0xFF;
      uint8_t gg = (baseColor >> 8) & 0xFF;
      uint8_t bb = baseColor & 0xFF;

      strip.setPixelColor(
        (pixel + offset + r * 17) % strip.numPixels(),
        strip.Color(rr * brightness / 255, gg * brightness / 255, bb * brightness / 255)
      );
    }
  }
}

// ------------------------------------------------------------
// EMBER / GHOST-CHASE WITH FADE-OUT TRAIL
// ------------------------------------------------------------
void rainbow_chase(Adafruit_NeoPixel &strip, int offset) {
  int numPixels = strip.numPixels();
  int tail = 10;

  for (int i = 0; i < numPixels; i++) {
    int head = (offset / 2) % numPixels;
    int dist = (i - head + numPixels) % numPixels;

    if (dist < tail) {
      float fade = 1.0 - (float)dist / tail;

      int hue = (offset / 4 + i * 3 - dist * 10 + 256) % 256;

      uint32_t base = hue_as_color(hue);
      uint8_t r = ((base >> 16) & 0xFF) * fade;
      uint8_t g = ((base >> 8) & 0xFF) * fade;
      uint8_t b = (base & 0xFF) * fade;

      strip.setPixelColor(i, strip.Color(r, g, b));
    } else {
      strip.setPixelColor(i, 0, 0, 0);
    }
  }
}

// ------------------------------------------------------------
// HALLOWEEN FLAME-STYLE EQ BARS
// ------------------------------------------------------------
void eq(Adafruit_NeoPixel &strip, int chunks, int volume) {
  int virtual_chunks = chunks * 2 - 1;
  int quantized = volume % (virtual_chunks + 1);

  int bars_to_light = (quantized > chunks)
                        ? (chunks * 2 - quantized)
                        : quantized;

  strip.clear();

  int pixels_per_chunk = strip.numPixels() / chunks;

  for (int chunk = 0; chunk < chunks; chunk++) {
    if (chunk < bars_to_light) {
      for (int p = 0; p < pixels_per_chunk; p++) {

        int pixelIndex = chunk * pixels_per_chunk + p;

        int hue = map(pixelIndex, 0, strip.numPixels(), 0, 255);
        uint32_t color = hue_as_color(hue);

        // occasional flicker highlight
        if (random(0, 20) == 1) {
          color = hue_as_color(random(160, 255));
        }

        strip.setPixelColor(pixelIndex, color);
      }
    }
  }
}
