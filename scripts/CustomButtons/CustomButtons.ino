
#include <Adafruit_NeoPixel.h>

const int baudRate = 9600;
// Neo Pixel
const int PIN = 12;
const int NUMPIXELS = 2;

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

const int firstButtonPin = 4;  
const int secondButtonPin = 5; 
const int thirdButtonPin = 6;

bool isFirstButtonPressed = false;
bool isSecondButtonPressed = false;
bool isThirdButtonPressed = false;


bool isButtonUp = false;

void setup()
{
    setupSerial();
    setupNeoPixel();
    setupButtons();
}

void setupSerial()
{
    Serial.begin(baudRate);
}

void setupNeoPixel()
{
    pixels.begin();
}

void setupButtons()
{
    pinMode(firstButtonPin, INPUT);
    pinMode(secondButtonPin, INPUT);
    pinMode(thirdButtonPin, INPUT);
}

void loop()
{
    pixels.setBrightness(10);

    if(digitalRead(firstButtonPin) == HIGH) {
        readingButton("first", 0, 255, 0, 0);
        isFirstButtonPressed = true;
    } 

    if(digitalRead(secondButtonPin) == HIGH) {
        readingButton("second", 0, 0, 255, 0);
        isSecondButtonPressed = true;
    }

    if(digitalRead(thirdButtonPin) == HIGH) {
        readingButton("third", 0, 0, 0, 255);
        isThirdButtonPressed = true;
    } 

    if(isFirstButtonPressed && digitalRead(firstButtonPin) == LOW) {
        isFirstButtonPressed = false;
        hideLed();
    }
    if(isSecondButtonPressed && digitalRead(secondButtonPin) == LOW) {
        isSecondButtonPressed = false;
        hideLed();
    }
    if(isThirdButtonPressed && digitalRead(thirdButtonPin) == LOW) {
        isThirdButtonPressed = false;
        hideLed();
    }
}

void checkPush(int pinNumber)
{
  int pushed = digitalRead(pinNumber);
  if (pushed == HIGH) {

    pixels.clear();
    pixels.setPixelColor(0, 0, 255, 0);
    pixels.show();
  }
  else {
    pixels.clear();
    pixels.setPixelColor(0, 255, 255, 255);
    pixels.show();
  }

}

void readingButton(String buttonType, int ledNumber, int redColor, int blueColor, int greenColor) 
{
    Serial.println(buttonType);
    showLed(ledNumber, redColor, blueColor, greenColor);
}

void showLed(int ledNumber, int redColor, int blueColor, int greenColor)
{
    pixels.clear();
    pixels.setPixelColor(ledNumber, redColor, blueColor, greenColor);
    pixels.show();
    delay(10);
}

void hideLed()
{
    pixels.clear();
    pixels.setPixelColor(0, 255, 255, 255);
    pixels.setPixelColor(1, 255, 255, 255);
    pixels.show();
    delay(10);
}