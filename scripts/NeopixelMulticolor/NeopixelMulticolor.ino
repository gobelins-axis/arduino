#include <Adafruit_NeoPixel.h>

const int baudRate = 9600;

const int PIN = 12;
const int NUMPIXELS = 3; 

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup()
{
    setupSerial();
    setupNeoPixel();
}

void setupSerial()
{
    Serial.begin(baudRate);
}

void setupNeoPixel()
{
    pixels.begin();
}

void loop() {
  rainbow(10);
}

void rainbow(int wait) {
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    for(int i=0; i<pixels.numPixels(); i++) { 
      int pixelHue = firstPixelHue + (i * 65536L / pixels.numPixels());
      pixels.setPixelColor(i, pixels.gamma32(pixels.ColorHSV(pixelHue)));
    }
    pixels.show();
    delay(wait);
  }
}