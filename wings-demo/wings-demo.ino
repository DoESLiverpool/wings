#include <Adafruit_NeoPixel.h>

#define STRIP_1_PIN 12
#define STRIP_1_LEN 196 // 4 strips of 49
#define STRIP_2_PIN 13
#define STRIP_2_LEN 75

Adafruit_NeoPixel strip_1(STRIP_1_LEN, STRIP_1_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip_2(STRIP_2_LEN, STRIP_2_PIN, NEO_GRB + NEO_KHZ800);

uint32_t hue_as_color(int hue);
void fill_strip_with_rainbow(Adafruit_NeoPixel &strip, int startingHue);
void sparkle(Adafruit_NeoPixel &strip, int chance);
void rain_effect(Adafruit_NeoPixel &strip, int chunks, int offset);
void rainbow_chase(Adafruit_NeoPixel &strip, int offset);

int iteration = 0;
int restart_after_iteration = 3000;
bool testing = false;
bool logging = false;

void setup() {
  if ( logging ) {
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

  if ( logging ) {
    Serial.println("setup complete");
  }
}

void loop() {
  if ( logging ) {
    Serial.print("iteration ");
    Serial.println(iteration);
  }

  if ( testing ) {
    
  }

  // Cycle through the different animations.
  else if ( iteration < 500 ) {
    fill_strip_with_rainbow(strip_1, iteration * -2);
    fill_strip_with_rainbow(strip_2, iteration * -2);

  } else if ( iteration < 2000 ) {
    sparkle(strip_2, 5);
    rain_effect(strip_1, 4, iteration);

  } else {  
    rainbow_chase(strip_1, iteration);
    rainbow_chase(strip_2, iteration);
  }

  strip_1.show();
  strip_2.show();

  if ( iteration < restart_after_iteration ) {
    iteration++;
  } else {
    iteration = 0;
  }
}

uint32_t hue_as_color(int hue) {
  // hue should be an integer 0-255.
  return strip_1.gamma32( strip_1.ColorHSV( map(hue, 0, 255, 0, 65536) ) );
}

void fill_strip_with_rainbow(Adafruit_NeoPixel &strip, int startingHue = 0) {
  for (int i=0; i<strip.numPixels(); i++) {
    int hue = map(i, 0, strip.numPixels(), 0, 255);
    hue = (startingHue + hue) % 255;
    strip.setPixelColor(i, hue_as_color(hue) );
  }
}

void sparkle(Adafruit_NeoPixel &strip, int chance = 10) {
  for (int i=0; i<strip.numPixels(); i++) {
    if ( random(0, 100) < chance ) {
      strip.setPixelColor(i, strip.Color(255, 255, 255) );
    } else {
      strip.setPixelColor(i, strip.Color(0, 0, 0) );
    }
  }
}

void rain_effect(Adafruit_NeoPixel &strip, int chunks, int offset = 0) {
  int chunk_length = strip.numPixels() / chunks;
  int gap_between_droplets = 25;
  for (int i=0; i<chunk_length; i++) {
    int brightness = 0;
    int position_in_droplet = i % gap_between_droplets;
    if ( position_in_droplet == 9 ) {
      brightness = 255;
    } else if ( position_in_droplet == 8 ) {
      brightness = 25;
    } else if ( position_in_droplet == 7 ) {
      brightness = 10;
    }
    for (int r=0; r<chunks; r++) {
      int pixel = (r * chunk_length) + i;
      int randomness = r * 17;
      strip.setPixelColor(
        (pixel + offset + randomness) % strip.numPixels(),
        strip.Color(brightness, brightness, brightness)
      );
    }
  }
}

void rainbow_chase(Adafruit_NeoPixel &strip, int offset = 0) {
  for (int i=0; i<strip.numPixels(); i++) {
    uint32_t color;
    if ( ( (i - offset) % 10 ) * -1 == 1 ) {
      color = hue_as_color( map(i, 0, strip.numPixels(), 0, 255) + offset % 255 );
    } else {
      color = strip.Color(0, 0, 0);
    }
    strip.setPixelColor(i, color);
  }
}
