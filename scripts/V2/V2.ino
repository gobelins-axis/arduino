#include <Button.h>
#include <ButtonEventCallback.h>
#include <BasicButton.h>
#include <Adafruit_NeoPixel.h>

// Serial
const int BAUD_RATE = 28800;

bool read = true;
bool write = false;

BasicButton buttonHome = BasicButton(A2);

// Controller set 1
BasicButton buttonA1 = BasicButton(6);
BasicButton buttonX1 = BasicButton(7);
BasicButton buttonI1 = BasicButton(8);
BasicButton buttonS1 = BasicButton(9);
BasicButton buttonW1 = BasicButton(A0);

const int Joystick1X = 4;
const int Joystick1Y = 5;

// Controller set 2
BasicButton buttonA2 = BasicButton(2);
BasicButton buttonX2 = BasicButton(3);
BasicButton buttonI2 = BasicButton(4);
BasicButton buttonS2 = BasicButton(5);
BasicButton buttonW2 = BasicButton(A1);

const int Joystick2X = 6;
const int Joystick2Y = 7;

// Leds: Left Strip
const int PIN_STRIP_1 = 12;
const int LED_COUNT_STRIP_1 = 51;
Adafruit_NeoPixel strip1(LED_COUNT_STRIP_1, PIN_STRIP_1, NEO_GRB + NEO_KHZ800);

// Leds: Right Strip
const int PIN_STRIP_2 = 13;
const int LED_COUNT_STRIP_2 = 51;
Adafruit_NeoPixel strip2(LED_COUNT_STRIP_2, PIN_STRIP_2, NEO_GRB + NEO_KHZ800);

// Leds: Controller 1 Strip
const int PIN_STRIP_3 = 11;
const int LED_COUNT_STRIP_3 = 6;
Adafruit_NeoPixel strip3(LED_COUNT_STRIP_3, PIN_STRIP_3, NEO_GRB + NEO_KHZ800);

// Leds: Controller 2 Strip
const int PIN_STRIP_4 = 10;
const int LED_COUNT_STRIP_4 = 5;
Adafruit_NeoPixel strip4(LED_COUNT_STRIP_4, PIN_STRIP_4, NEO_GRB + NEO_KHZ800);

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);

    setupSerial();
    setupButtons();
    setupLeds();
}

void setupSerial()
{
    Serial.begin(BAUD_RATE);
}

void setupLeds()
{
    strip1.begin();
    strip2.begin();
    strip3.begin();
    strip4.begin();
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

    // Controller set 2
    buttonA2.onPress(onButtonPress);
    buttonA2.onRelease(onButtonRelease);

    buttonX2.onPress(onButtonPress);
    buttonX2.onRelease(onButtonRelease);

    buttonI2.onPress(onButtonPress);
    buttonI2.onRelease(onButtonRelease);

    buttonS2.onPress(onButtonPress);
    buttonS2.onRelease(onButtonRelease);

    buttonW2.onPress(onButtonPress);
    buttonW2.onRelease(onButtonRelease);
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

    printJoystickData(1, analogRead(Joystick1X), analogRead(Joystick1Y));

    // Controller set 2
    buttonA2.update();
    buttonX2.update();
    buttonI2.update();
    buttonS2.update();
    buttonW2.update();

    printJoystickData(2, analogRead(Joystick2X), analogRead(Joystick2Y));

    // Serial
    if (read)
        listenSerial();

    read = !read;
    write = !write;
}

void listenSerial()
{
    if (Serial.available() > 0)
    {
        String serialData = Serial.readStringUntil('\n');

        String data[3];
        parseSerialData(serialData, data);

        String strip = data[0];
        String indexString = data[1];
        String colorString = data[2];

        int index = indexString.toInt();
        int color[3];
        parseColorString(color, colorString);

        // Set pixel colors
        if (strip == "1")
        {
            strip1.setPixelColor(index, color[0], color[1], color[2]);
            strip1.show();
        }

        if (strip == "2")
        {
            strip2.setPixelColor(index, color[0], color[1], color[2]);
            strip2.show();
        }

        if (strip == "3")
        {
            strip3.setPixelColor(index, color[0], color[1], color[2]);
            strip3.show();
        }

        if (strip == "4")
        {
            strip4.setPixelColor(index, color[0], color[1], color[2]);
            strip4.show();
        }
    }
}

// Input pattern: strip;index;color
void parseSerialData(String serialData, String data[3])
{
    int stringIndex;

    // Get strip
    stringIndex = serialData.indexOf(';');
    String strip = serialData.substring(0, stringIndex);
    serialData = serialData.substring(stringIndex + 1, serialData.length());

    // Get index
    stringIndex = serialData.indexOf(';');
    String index = serialData.substring(0, stringIndex);
    serialData = serialData.substring(stringIndex + 1, serialData.length());

    // Get color string
    stringIndex = serialData.indexOf(';');
    String colorString = serialData.substring(0, stringIndex);

    data[0] = strip;
    data[1] = index;
    data[2] = colorString;
}

// Input pattern: red,green,blue
void parseColorString(int color[3], String colorString)
{
    int indexRed = colorString.indexOf(',');
    String red = colorString.substring(0, indexRed);
    colorString = colorString.substring(indexRed + 1, colorString.length());

    int indexGreen = colorString.indexOf(',');
    String green = colorString.substring(0, indexGreen);
    colorString = colorString.substring(indexGreen + 1, colorString.length());

    int indexBlue = colorString.indexOf(',');
    String blue = colorString.substring(0, indexBlue);
    colorString = colorString.substring(indexBlue + 1, colorString.length());

    color[0] = red.toInt();
    color[1] = green.toInt();
    color[2] = blue.toInt();
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

    // Controller set 2
    if (button.is(buttonA2))
        printButtonData("a", 2, "keydown");

    if (button.is(buttonX2))
        printButtonData("x", 2, "keydown");

    if (button.is(buttonI2))
        printButtonData("i", 2, "keydown");

    if (button.is(buttonS2))
        printButtonData("s", 2, "keydown");

    if (button.is(buttonW2))
        printButtonData("w", 2, "keydown");
}

void onButtonRelease(Button &button)
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

    // Controller set 2
    if (button.is(buttonA2))
        printButtonData("a", 2, "keyup");

    if (button.is(buttonX2))
        printButtonData("x", 2, "keyup");

    if (button.is(buttonI2))
        printButtonData("i", 2, "keyup");

    if (button.is(buttonS2))
        printButtonData("s", 2, "keyup");

    if (button.is(buttonW2))
        printButtonData("w", 2, "keyup");
}

void onButtonHomePress()
{
    printHomeButtonData("keydown");
}

void onButtonHomeRelease()
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