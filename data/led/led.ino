#include <Adafruit_NeoPixel.h>

#define PIN A1 // ← ここがポイント
#define NUMPIXELS 1

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup()
{
    pixels.begin();
    pixels.clear();
}

void loop()
{
    pixels.setPixelColor(0, pixels.Color(255, 0, 0)); // 赤
    pixels.show();
    delay(1000);

    pixels.setPixelColor(0, pixels.Color(0, 0, 255)); // 青
    pixels.show();
    delay(1000);
}
