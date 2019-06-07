#include <Adafruit_NeoPixel.h>

#define STRIP_1_PIN 12
#define STRIP_1_LEN 196 // 4 strips of 49
#define STRIP_2_PIN 13
#define STRIP_2_LEN 75

Adafruit_NeoPixel strip_1(STRIP_1_LEN, STRIP_1_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip_2(STRIP_2_LEN, STRIP_2_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  strip_1.begin();
  strip_2.begin();

  strip_1.setBrightness(50);
  strip_2.setBrightness(50);

  strip_1.clear();
  strip_2.clear();

  strip_1.show();
  strip_2.show();
}

void loop() {
}
