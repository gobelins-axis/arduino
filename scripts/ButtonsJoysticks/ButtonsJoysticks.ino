#include <Button.h>
#include <ButtonEventCallback.h>
#include <BasicButton.h>

const int baudRate = 9600;

BasicButton firstButton = BasicButton(4);
BasicButton secondButton = BasicButton(5);
BasicButton thirdButton = BasicButton(6);

const int JoyStickX1 = 0;
const int JoyStickY1 = 1;
const int SW1 = 7;

const int JoyStickX2 = 2;
const int JoyStickY2 = 3;
const int SW2 = 8;

void setup()
{
    setupSerial();

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

void loop(){
    firstButton.update();
    secondButton.update();
    thirdButton.update();

    printJoystickData(1, analogRead(JoyStickX1), analogRead(JoyStickY1));
    printJoystickData(2, analogRead(JoyStickX2), analogRead(JoyStickY2));
}

void onButtonPressed(Button& btn){
    if(btn.is(firstButton)) {
        readingButton("a", "keydown");
    } else if(btn.is(secondButton)) {
        readingButton("b", "keydown");
    } else if(btn.is(thirdButton)) {
        readingButton("c", "keydown");
    }
}

void onButtonReleased(Button& btn){
        if(btn.is(firstButton)) {
        readingButton("a", "keyup");
    } else if(btn.is(secondButton)) {
        readingButton("b", "keyup");
    } else if(btn.is(thirdButton)) {
        readingButton("c", "keyup");
    }
}

void readingButton(String buttonName, String buttonState) 
{
    Serial.println("buttonName:" + String(buttonName) + "_" + "buttonState:" + String(buttonState));
}

void printJoystickData(int joystickId, int joystickDataX, int joystickDataY)
{
    Serial.println("joystickId:" + String(joystickId) + "_" + "x:" + String(joystickDataX) + "_" + "y:" + String(joystickDataY));
}