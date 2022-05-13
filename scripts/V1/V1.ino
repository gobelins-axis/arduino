#include <Button.h>
#include <ButtonEventCallback.h>
#include <BasicButton.h>

const int baudRate = 9600;

// Controller set 1
BasicButton buttonA1 = BasicButton(2);
BasicButton buttonB1 = BasicButton(3);
BasicButton buttonC1 = BasicButton(4);
BasicButton buttonD1 = BasicButton(5);
BasicButton buttonE1 = BasicButton(6);

// Controller set 2
BasicButton buttonA2 = BasicButton(7);
BasicButton buttonB2 = BasicButton(8);
BasicButton buttonC2 = BasicButton(9);
BasicButton buttonD2 = BasicButton(10);
BasicButton buttonE2 = BasicButton(11);

const int Joystick1X = 0;
const int Joystick1Y = 1;
const int SW1 = 7;

const int Joystick2X = 2;
const int Joystick2Y = 3;
const int SW2 = 8;

void setup()
{
    setupSerial();
    setupButtons();
}

void setupSerial()
{
    Serial.begin(baudRate);
}

void setupButtons()
{
    // Controller set 1
    buttonA1.onPress(onButtonPress);
    buttonA1.onRelease(onButtonRelease);

    buttonB1.onPress(onButtonPress);
    buttonB1.onRelease(onButtonRelease);

    buttonC1.onPress(onButtonPress);
    buttonC1.onRelease(onButtonRelease);

    buttonD1.onPress(onButtonPress);
    buttonD1.onRelease(onButtonRelease);

    buttonE1.onPress(onButtonPress);
    buttonE1.onRelease(onButtonRelease);

    // Controller set 2
    buttonA2.onPress(onButtonPress);
    buttonA2.onRelease(onButtonRelease);

    buttonB2.onPress(onButtonPress);
    buttonB2.onRelease(onButtonRelease);

    buttonC2.onPress(onButtonPress);
    buttonC2.onRelease(onButtonRelease);

    buttonD2.onPress(onButtonPress);
    buttonD2.onRelease(onButtonRelease);

    buttonE2.onPress(onButtonPress);
    buttonE2.onRelease(onButtonRelease);
}

void loop()
{
    // Controller set 1
    buttonA1.update();
    buttonB1.update();
    buttonC1.update();
    buttonD1.update();
    buttonE1.update();

    printJoystickData(1, analogRead(Joystick1X), analogRead(Joystick1Y));

    // Controller set 2
    buttonA2.update();
    buttonB2.update();
    buttonC2.update();
    buttonD2.update();
    buttonE2.update();

    printJoystickData(2, analogRead(Joystick2X), analogRead(Joystick2Y));
}

void onButtonPress(Button &button)
{
    // Controller set 1
    if (button.is(buttonA1))
        printButtonData("a", 1, "keydown");

    if (button.is(buttonB1))
        printButtonData("b", 1, "keydown");

    if (button.is(buttonC1))
        printButtonData("c", 1, "keydown");

    if (button.is(buttonD1))
        printButtonData("d", 1, "keydown");

    if (button.is(buttonE1))
        printButtonData("e", 1, "keydown");

    // Controller set 2
    if (button.is(buttonA2))
        printButtonData("a", 2, "keydown");

    if (button.is(buttonB2))
        printButtonData("b", 2, "keydown");

    if (button.is(buttonC2))
        printButtonData("c", 2, "keydown");

    if (button.is(buttonD2))
        printButtonData("d", 2, "keydown");

    if (button.is(buttonE2))
        printButtonData("e", 2, "keydown");
}

void onButtonRelease(Button &button)
{
    // Controller set 1
    if (button.is(buttonA1))
        printButtonData("a", 1, "keyup");

    if (button.is(buttonB1))
        printButtonData("b", 1, "keyup");

    if (button.is(buttonC1))
        printButtonData("c", 1, "keyup");

    if (button.is(buttonD1))
        printButtonData("d", 1, "keyup");

    if (button.is(buttonE1))
        printButtonData("e", 1, "keyup");

    // Controller set 2
    if (button.is(buttonA2))
        printButtonData("a", 2, "keyup");

    if (button.is(buttonB2))
        printButtonData("b", 2, "keyup");

    if (button.is(buttonC2))
        printButtonData("c", 2, "keyup");

    if (button.is(buttonD2))
        printButtonData("d", 2, "keyup");

    if (button.is(buttonE2))
        printButtonData("e", 2, "keyup");
}

void printButtonData(String key, int id, String state)
{
    Serial.println("type:button__key:" + String(key) + "__" + "id:" + String(id) + "__" + "state:" + String(state));
}

void printJoystickData(int id, int x, int y)
{
    Serial.println("type:joystick__id:" + String(id) + "__" + "x:" + String(x) + "__" + "y:" + String(y));
}