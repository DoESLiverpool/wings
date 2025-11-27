#include <Adafruit_NeoPixel.h>

#define STRIP_1_PIN 12
#define STRIP_1_LEN 196   // 4 strips of 49
#define STRIP_2_PIN 13
#define STRIP_2_LEN 75

Adafruit_NeoPixel strip_1(STRIP_1_LEN, STRIP_1_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip_2(STRIP_2_LEN, STRIP_2_PIN, NEO_GRB + NEO_KHZ800);

// Function prototypes
uint32_t halloween_color(int index);
void fill_strip_halloween(Adafruit_NeoPixel &strip, int offset);
void sparkle(Adafruit_NeoPixel &strip, int chance);
void rain_effect(Adafruit_NeoPixel &strip, int chunks, int offset);
void halloween_chase(Adafruit_NeoPixel &strip, int offset);
void eq_halloween(Adafruit_NeoPixel &strip, int chunks, int volume);

int iteration = 0;
int restart_after_iteration = 5000;

bool testing = false;
bool logging = false;

void setup() {
  if (logging) {
    Serial.begin(9600);
    Serial.println("setup");
  }

  strip_1.begin();
  strip_2.begin();

  strip_1.setBrightness(150);
  strip_2.setBrightness(150);

  strip_1.clear();
  strip_2.clear();

  strip_1.show();
  strip_2.show();

  if (logging) Serial.println("setup complete");
}

// ============================================================
//                          MAIN LOOP
// ============================================================
void loop() {

  if (iteration < 500) {
    fill_strip_halloween(strip_1, iteration);
    fill_strip_halloween(strip_2, iteration);

  } else if (iteration < 2000) {
    sparkle(strip_2, 5);
    rain_effect(strip_1, 4, iteration);

  } else if (iteration < 4000) {
    int volume = iteration / 10;
    eq_halloween(strip_1, 4, volume);
    eq_halloween(strip_2, 4, volume);

  } else {
    halloween_chase(strip_1, iteration);
    halloween_chase(strip_2, iteration);
  }

  strip_1.show();
  strip_2.show();

  if (iteration < restart_after_iteration) iteration++;
  else iteration = 0;
}

// ============================================================
//                 HALLOWEEN COLOR PALETTE
// ============================================================
//
// Returns ORANGE → PURPLE → GREEN cycling
//
uint32_t halloween_color(int index) {
  int mode = index % 3;

  switch (mode) {
    case 0:  // ORANGE
      return strip_1.Color(255, 80, 0);
    case 1:  // PURPLE
      return strip_1.Color(160, 0, 255);
    case 2:  // SLIME GREEN
      return strip_1.Color(0, 255, 40);
  }
  return strip_1.Color(0, 0, 0);
}

// ============================================================
//         HALLOWEEN “RAINBOW” (Orange / Purple / Green)
// ============================================================
void fill_strip_halloween(Adafruit_NeoPixel &strip, int offset) {
  for (int i = 0; i < strip.numPixels(); i++) {
    int index = (i + offset / 4) % 300;
    int colorIndex = index % 90;  // 30 pixels per color range

    if (colorIndex < 30)
      strip.setPixelColor(i, halloween_color(0)); // orange
    else if (colorIndex < 60)
      strip.setPixelColor(i, halloween_color(1)); // purple
    else
      strip.setPixelColor(i, halloween_color(2)); // green
  }
}

// ============================================================
//                      SPARKLE EFFECT
// ============================================================
void sparkle(Adafruit_NeoPixel &strip, int chance) {
  for (int i = 0; i < strip.numPixels(); i++) {
    if (random(0, 100) < chance) {
      strip.setPixelColor(i, halloween_color(random(0,3)));
    } else {
      strip.setPixelColor(i, 0, 0, 0);
    }
  }
}

// ============================================================
//                      RAIN EFFECT
// ============================================================
void rain_effect(Adafruit_NeoPixel &strip, int chunks, int offset) {
  int chunk_length = strip.numPixels() / chunks;

  for (int i = 0; i < chunk_length; i++) {
    int brightness = 0;
    int pos = i % 20;

    if (pos == 5) brightness = 255;
    else if (pos == 4) brightness = 80;
    else if (pos == 3) brightness = 40;

    for (int r = 0; r < chunks; r++) {
      int pixel = (r * chunk_length) + i;
      uint32_t base = halloween_color((i + r + offset / 30) % 3);

      uint8_t R = ((base >> 16) & 0xFF) * brightness / 255;
      uint8_t G = ((base >> 8) & 0xFF) * brightness / 255;
      uint8_t B = (base & 0xFF) * brightness / 255;

      strip.setPixelColor((pixel + offset) % strip.numPixels(), strip.Color(R,G,B));
    }
  }
}

// ============================================================
//                HALLOWEEN CHASE (with glow)
// ============================================================
void halloween_chase(Adafruit_NeoPixel &strip, int offset) {
  int num = strip.numPixels();
  int head = offset % num;
  int tail = 8;

  for (int i = 0; i < num; i++) {
    int dist = (i - head + num) % num;

    if (dist < tail) {
      float fade = 1.0 - (float)dist / tail;

      uint32_t c = halloween_color((offset / 10 + dist) % 3);

      uint8_t R = ((c >> 16) & 0xFF) * fade;
      uint8_t G = ((c >> 8) & 0xFF) * fade;
      uint8_t B = (c & 0xFF) * fade;

      strip.setPixelColor(i, strip.Color(R,G,B));
    } else {
      strip.setPixelColor(i, 0, 0, 0);
    }
  }
}

// ============================================================
//                   HALLOWEEN EQ BAR GRAPH
// ============================================================
void eq_halloween(Adafruit_NeoPixel &strip, int chunks, int volume) {
  int virtual_chunks = (chunks * 2) - 1;
  int quant = volume % (virtual_chunks + 1);
  int lit = quant > chunks ? (chunks * 2) - quant : quant;

  strip.clear();

  int pixels_per_chunk = strip.numPixels() / chunks;

  for (int chunk = 0; chunk < chunks; chunk++) {
    if (chunk < lit) {
      for (int p = 0; p < pixels_per_chunk; p++) {
        int idx = (chunk * pixels_per_chunk + p + volume / 3);
        uint32_t color = halloween_color(idx % 3);
        strip.setPixelColor(chunk * pixels_per_chunk + p, color);
      }
    }
  }
}
