#include <Button.h>
#include <ButtonEventCallback.h>
#include <BasicButton.h>
#include <Adafruit_NeoPixel.h>

const int baudRate = 9600;

BasicButton firstButton = BasicButton(4);
BasicButton secondButton = BasicButton(5);
BasicButton thirdButton = BasicButton(6);

// const int PIN = 12;
// const int NUMPIXELS = 2;

// Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup()
{
    setupSerial();
    setupNeoPixel();

    firstButton.onPress(onButtonPressed);
    firstButton.onRelease(onButtonReleased);

    secondButton.onPress(onButtonPressed);
    secondButton.onRelease(onButtonReleased);

    thirdButton.onPress(onButtonPressed);
    thirdButton.onRelease(onButtonReleased);
}

void setupSerial()
{
    Serial.begin(baudRate);
}

void setupNeoPixel()
{
    // pixels.begin();
}

void loop(){
    firstButton.update();
    secondButton.update();
    thirdButton.update();
}

void onButtonPressed(Button& btn){
    if(btn.is(firstButton)) {
        readingButton("first", 0, 255, 0, 0);
    } else if(btn.is(secondButton)) {
        readingButton("second", 0, 0, 255, 0);
    } else if(btn.is(thirdButton)) {
        readingButton("third", 0, 0, 0, 255);
    }
}

void onButtonReleased(Button& btn){
    hideLed();
}

void readingButton(String buttonType, int ledNumber, int redColor, int blueColor, int greenColor) 
{
    Serial.println(buttonType);
    showLed(ledNumber, redColor, blueColor, greenColor);
}

void showLed(int ledNumber, int redColor, int blueColor, int greenColor)
{
    // pixels.clear();
    // pixels.setPixelColor(ledNumber, redColor, blueColor, greenColor);
    // pixels.show();
    delay(10);
}

void hideLed()
{
    // pixels.clear();
    // pixels.setPixelColor(0, 255, 255, 255);
    // pixels.setPixelColor(1, 255, 255, 255);
    // pixels.show();
    delay(10);
}