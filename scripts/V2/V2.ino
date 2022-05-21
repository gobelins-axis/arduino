#include <Button.h>
#include <ButtonEventCallback.h>
#include <BasicButton.h>
#include <Adafruit_NeoPixel.h>

// Serial
const int BAUD_RATE = 9600;

// Controller set 1
// BasicButton buttonA1 = BasicButton(2);
// BasicButton buttonB1 = BasicButton(3);
// BasicButton buttonC1 = BasicButton(4);
// BasicButton buttonD1 = BasicButton(5);
// BasicButton buttonE1 = BasicButton(6);

const int Joystick1X = 4;
const int Joystick1Y = 5;

// Controller set 2
// BasicButton buttonA2 = BasicButton(7);
// BasicButton buttonB2 = BasicButton(8);
// BasicButton buttonC2 = BasicButton(9);
// BasicButton buttonD2 = BasicButton(10);
// BasicButton buttonE2 = BasicButton(11);

const int Joystick2X = 6;
const int Joystick2Y = 7;

// Leds
const int PIN_STRIP_1 = 12;
const int LED_COUNT_STRIP_1 = 4;
Adafruit_NeoPixel strip1(LED_COUNT_STRIP_1, PIN_STRIP_1, NEO_GRB + NEO_KHZ800);

const int PIN_STRIP_2 = 11;
const int LED_COUNT_STRIP_2 = 4;
Adafruit_NeoPixel strip2(LED_COUNT_STRIP_2, PIN_STRIP_2, NEO_GRB + NEO_KHZ800);

void setup()
{
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
}

void setupButtons()
{
    // Controller set 1
    // buttonA1.onPress(onButtonPress);
    // buttonA1.onRelease(onButtonRelease);

    // buttonB1.onPress(onButtonPress);
    // buttonB1.onRelease(onButtonRelease);

    // buttonC1.onPress(onButtonPress);
    // buttonC1.onRelease(onButtonRelease);

    // buttonD1.onPress(onButtonPress);
    // buttonD1.onRelease(onButtonRelease);

    // buttonE1.onPress(onButtonPress);
    // buttonE1.onRelease(onButtonRelease);

    // Controller set 2
    // buttonA2.onPress(onButtonPress);
    // buttonA2.onRelease(onButtonRelease);

    // buttonB2.onPress(onButtonPress);
    // buttonB2.onRelease(onButtonRelease);

    // buttonC2.onPress(onButtonPress);
    // buttonC2.onRelease(onButtonRelease);

    // buttonD2.onPress(onButtonPress);
    // buttonD2.onRelease(onButtonRelease);

    // buttonE2.onPress(onButtonPress);
    // buttonE2.onRelease(onButtonRelease);
}

void loop()
{
    // Controller set 1
    // buttonA1.update();
    // buttonB1.update();
    // buttonC1.update();
    // buttonD1.update();
    // buttonE1.update();

    // printJoystickData(1, analogRead(Joystick1X), analogRead(Joystick1Y));

    // Controller set 2
    // buttonA2.update();
    // buttonB2.update();
    // buttonC2.update();
    // buttonD2.update();
    // buttonE2.update();

    // printJoystickData(2, analogRead(Joystick2X), analogRead(Joystick2Y));

    // Serial
    listenSerial();

    // Leds
    // pixels.setPixelColor(0, 0, 255, 0);
    strip1.show();
    strip2.show();
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
            strip1.setPixelColor(index, color[0], color[1], color[2]);

        if (strip == "2")
            strip2.setPixelColor(index, color[0], color[1], color[2]);
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
    // if (button.is(buttonA1))
    //     printButtonData("a", 1, "keydown");

    // if (button.is(buttonB1))
    //     printButtonData("b", 1, "keydown");

    // if (button.is(buttonC1))
    //     printButtonData("c", 1, "keydown");

    // if (button.is(buttonD1))
    //     printButtonData("d", 1, "keydown");

    // if (button.is(buttonE1))
    //     printButtonData("e", 1, "keydown");

    // Controller set 2
    // if (button.is(buttonA2))
    //     printButtonData("a", 2, "keydown");

    // if (button.is(buttonB2))
    //     printButtonData("b", 2, "keydown");

    // if (button.is(buttonC2))
    //     printButtonData("c", 2, "keydown");

    // if (button.is(buttonD2))
    //     printButtonData("d", 2, "keydown");

    // if (button.is(buttonE2))
    //     printButtonData("e", 2, "keydown");
}

void onButtonRelease(Button &button)
{
    // Controller set 1
    // if (button.is(buttonA1))
    //     printButtonData("a", 1, "keyup");

    // if (button.is(buttonB1))
    //     printButtonData("b", 1, "keyup");

    // if (button.is(buttonC1))
    //     printButtonData("c", 1, "keyup");

    // if (button.is(buttonD1))
    //     printButtonData("d", 1, "keyup");

    // if (button.is(buttonE1))
    //     printButtonData("e", 1, "keyup");

    // Controller set 2
    // if (button.is(buttonA2))
    //     printButtonData("a", 2, "keyup");

    // if (button.is(buttonB2))
    //     printButtonData("b", 2, "keyup");

    // if (button.is(buttonC2))
    //     printButtonData("c", 2, "keyup");

    // if (button.is(buttonD2))
    //     printButtonData("d", 2, "keyup");

    // if (button.is(buttonE2))
    //     printButtonData("e", 2, "keyup");
}

void printButtonData(String key, int id, String state)
{
    Serial.println("type:button__key:" + String(key) + "__" + "id:" + String(id) + "__" + "state:" + String(state));
}

void printJoystickData(int id, int x, int y)
{
    Serial.println("type:joystick__id:" + String(id) + "__" + "x:" + String(x) + "__" + "y:" + String(y));
}