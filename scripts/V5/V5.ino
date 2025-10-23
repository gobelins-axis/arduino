#include <Button.h>
#include <ButtonEventCallback.h>
#include <BasicButton.h>
#include <Adafruit_NeoPixel.h>

// Serial
//const int BAUD_RATE = 115200;  // Increased for Teensy's faster capabilities
const int BAUD_RATE = 28800;

bool read = true;
bool write = true;

unsigned long timeCounter = 0;

BasicButton buttonHome = BasicButton(12);

// Controller set 1
BasicButton buttonA1 = BasicButton(10);
BasicButton buttonX1 = BasicButton(8);
BasicButton buttonI1 = BasicButton(6);
BasicButton buttonS1 = BasicButton(4);
BasicButton buttonW1 = BasicButton(2);

 const int Joystick1X = A13;  // Pin 27 on Teensy 4.1
 const int Joystick1Y = A12;  // Pin 26 on Teensy

// // Controller set 2
// BasicButton buttonA2 = BasicButton(9);
// BasicButton buttonX2 = BasicButton(7);
// BasicButton buttonI2 = BasicButton(6);
// BasicButton buttonS2 = BasicButton(4);
// BasicButton buttonW2 = BasicButton(14);  // Changed from A0 to pin 14 (A0 on Teensy)

// const int Joystick2X = A4;  // Pin 18 on Teensy
// const int Joystick2Y = A5;  // Pin 19 on Teensy

// Function declarations with correct signatures for Teensy
void onButtonPress(Button &button);
void onButtonRelease(Button &button, uint16_t duration);
void onButtonHomePress(Button &button);
void onButtonHomeRelease(Button &button, uint16_t duration);

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    setupSerial();
    setupButtons();
}

void setupSerial()
{
    Serial.begin(BAUD_RATE);
}

void setupButtons()
{
    // Button Home
    buttonHome.onPress(onButtonHomePress);
    buttonHome.onRelease(onButtonHomeRelease);

    // Controller set 1
    buttonA1.onPress(onButtonPress);
    buttonA1.onRelease(onButtonRelease);

    buttonX1.onPress(onButtonPress);
    buttonX1.onRelease(onButtonRelease);

    buttonI1.onPress(onButtonPress);
    buttonI1.onRelease(onButtonRelease);

    buttonS1.onPress(onButtonPress);
    buttonS1.onRelease(onButtonRelease);

     buttonW1.onPress(onButtonPress);
     buttonW1.onRelease(onButtonRelease);

    // // Controller set 2
    // buttonA2.onPress(onButtonPress);
    // buttonA2.onRelease(onButtonRelease);

    // buttonX2.onPress(onButtonPress);
    // buttonX2.onRelease(onButtonRelease);

    // buttonI2.onPress(onButtonPress);
    // buttonI2.onRelease(onButtonRelease);

    // buttonS2.onPress(onButtonPress);
    // buttonS2.onRelease(onButtonRelease);

    // buttonW2.onPress(onButtonPress);
    // buttonW2.onRelease(onButtonRelease);
}

void loop()
{
    // Home
    buttonHome.update();

    // Controller set 1
    buttonA1.update();
    buttonX1.update();
    buttonI1.update();
    buttonS1.update();
    buttonW1.update();

    // printJoystickData(1, analogRead(Joystick1X), analogRead(Joystick1Y));

    // // Controller set 2
    // buttonA2.update();
    // buttonX2.update();
    // buttonI2.update();
    // buttonS2.update();
    // buttonW2.update();

    // printJoystickData(2, analogRead(Joystick2X), analogRead(Joystick2Y));

    timeCounter++;
}

void onButtonPress(Button &button)
{
    // Controller set 1
    if (button.is(buttonA1))
        printButtonData("a", 1, "keydown");

    if (button.is(buttonX1))
        printButtonData("x", 1, "keydown");

    if (button.is(buttonI1))
        printButtonData("i", 1, "keydown");

    if (button.is(buttonS1))
        printButtonData("s", 1, "keydown");

     if (button.is(buttonW1))
        printButtonData("w", 1, "keydown");

    // // Controller set 2
    // if (button.is(buttonA2))
    //     printButtonData("a", 2, "keydown");

    // if (button.is(buttonX2))
    //     printButtonData("x", 2, "keydown");

    // if (button.is(buttonI2))
    //     printButtonData("i", 2, "keydown");

    // if (button.is(buttonS2))
    //     printButtonData("s", 2, "keydown");

    // if (button.is(buttonW2))
    //     printButtonData("w", 2, "keydown");
}

// FIXED: Added uint16_t duration parameter for Teensy compatibility
void onButtonRelease(Button &button, uint16_t duration)
{
    // Controller set 1
    if (button.is(buttonA1))
        printButtonData("a", 1, "keyup");

    if (button.is(buttonX1))
        printButtonData("x", 1, "keyup");

    if (button.is(buttonI1))
        printButtonData("i", 1, "keyup");

    if (button.is(buttonS1))
        printButtonData("s", 1, "keyup");

     if (button.is(buttonW1))
         printButtonData("w", 1, "keyup");

    // // Controller set 2
    // if (button.is(buttonA2))
    //     printButtonData("a", 2, "keyup");

    // if (button.is(buttonX2))
    //     printButtonData("x", 2, "keyup");

    // if (button.is(buttonI2))
    //     printButtonData("i", 2, "keyup");

    // if (button.is(buttonS2))
    //     printButtonData("s", 2, "keyup");

    // if (button.is(buttonW2))
    //     printButtonData("w", 2, "keyup");
}

// FIXED: Added Button& parameter and uint16_t duration for Teensy compatibility
void onButtonHomePress(Button &button)
{
    printHomeButtonData("keydown");
}

void onButtonHomeRelease(Button &button, uint16_t duration)
{
    printHomeButtonData("keyup");
}

void printButtonData(String key, int id, String state)
{
    if (write)
        Serial.println("type:button__key:" + String(key) + "__" + "id:" + String(id) + "__" + "state:" + String(state));
}

void printHomeButtonData(String state)
{
    if (write)
        Serial.println("type:button-home__key:home__id:0__state:" + String(state));
}

void printJoystickData(int id, int x, int y)
{
     if (write)
         Serial.println("type:joystick__id:" + String(id) + "__" + "x:" + String(x) + "__" + "y:" + String(y));
}