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

// Leds: Left Strip
const int PIN_STRIP_1 = 12;
const int LED_COUNT_STRIP_1 = 51;
Adafruit_NeoPixel strip1(LED_COUNT_STRIP_1, PIN_STRIP_1, NEO_GRB + NEO_KHZ800);

// Leds: Right Strip
const int PIN_STRIP_2 = 13;
const int LED_COUNT_STRIP_2 = 51 + 4;
Adafruit_NeoPixel strip2(LED_COUNT_STRIP_2, PIN_STRIP_2, NEO_GRB + NEO_KHZ800);

// Leds: Controller 1 Strip
const int PIN_STRIP_3 = 11;
const int LED_COUNT_STRIP_3 = 6;
Adafruit_NeoPixel strip3(LED_COUNT_STRIP_3, PIN_STRIP_3, NEO_GRB + NEO_KHZ800);

// Leds: Controller 2 Strip
const int PIN_STRIP_4 = 10;
const int LED_COUNT_STRIP_4 = 5;
Adafruit_NeoPixel strip4(LED_COUNT_STRIP_4, PIN_STRIP_4, NEO_GRB + NEO_KHZ800);

// Animations
int currentIndex = 0;
String buildup1AnimationString = "buildup1";
String buildup2AnimationString = "buildup2";
String revealAnimationString = "reveal";
String startAnimationString = "start";

// Function declarations with correct signatures for Teensy
void onButtonPress(Button &button);
void onButtonRelease(Button &button, uint16_t duration);
void onButtonHomePress(Button &button);
void onButtonHomeRelease(Button &button, uint16_t duration);

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    setupSerial();
    Serial.println("DDD1");

    setupButtons();
    Serial.println("DDD2");

    setupLeds();
    Serial.println("DDD3");
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

    strip1.show();
    strip2.show();
    strip3.show();
    strip4.show();
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

    // Animations
    // listenSerialPrez();
    // ledAnimations();

    timeCounter++;
}

// Animations
void listenSerialPrez()
{
    if (Serial.available() > 0)
    {
        String serialData = Serial.readStringUntil('\n');

        // Buildup 1
        if (serialData == String(buildup1AnimationString))
        {
            currentIndex = 1;
        }

        // Buildup 2
        if (serialData == String(buildup2AnimationString))
        {
            currentIndex = 2;
        }

        // Reveal
        if (serialData == String(revealAnimationString))
        {
            currentIndex = 3;
        }

        // Start
        if (serialData == String(startAnimationString))
        {
            currentIndex = 4;
        }
    }
}

void ledAnimations()
{
    // Buildup 1
    if (currentIndex == 1)
    {
        buildup1Animation();
    }
    else if (currentIndex == 2)
    {
        buildup2Animation();
    }
    else if (currentIndex == 3)
    {
        revealAnimation();
    }
    else if (currentIndex == 4)
    {
        startAnimation();
    }
}

void buildup1Animation()
{
    float speed = 0.001;  // Adjusted for Teensy's faster processing
    float sineAnimation = (cos(timeCounter * speed) + 1.0) / 2.0 * 255.0;

    // STRIP 1
    for (int i = 0; i < LED_COUNT_STRIP_1; i++)
    {
        strip1.setPixelColor(i, sineAnimation, 0, 0);
    }

    strip1.show();

    // STRIP 2
    for (int i = 0; i < LED_COUNT_STRIP_2; i++)
    {
        strip2.setPixelColor(i, sineAnimation, 0, 0);
    }

    strip2.show();

    // STRIP 3
    for (int i = 0; i < LED_COUNT_STRIP_3; i++)
    {
        strip3.setPixelColor(i, sineAnimation, 0, 0);
    }

    strip3.show();

    // STRIP 4
    for (int i = 0; i < LED_COUNT_STRIP_4; i++)
    {
        strip4.setPixelColor(i, sineAnimation, 0, 0);
    }

    strip4.show();
}

void buildup2Animation()
{
    // STRIP 1
    int ledIndex1 = random(LED_COUNT_STRIP_1);
    strip1.setPixelColor(ledIndex1, 255, 0, 0);
    strip1.show();
    strip1.setPixelColor(ledIndex1, 0, 0, 0);
    strip1.show();

    // STRIP 2
    int ledIndex2 = random(LED_COUNT_STRIP_2);
    strip2.setPixelColor(ledIndex2, 255, 0, 0);
    strip2.show();
    strip2.setPixelColor(ledIndex2, 0, 0, 0);
    strip2.show();

    // STRIP 3
    int ledIndex3 = random(LED_COUNT_STRIP_3);
    strip3.setPixelColor(ledIndex3, 255, 0, 0);
    strip3.show();
    strip3.setPixelColor(ledIndex3, 0, 0, 0);
    strip3.show();

    // STRIP 4
    int ledIndex4 = random(LED_COUNT_STRIP_4);
    strip4.setPixelColor(ledIndex4, 255, 0, 0);
    strip4.show();
    strip4.setPixelColor(ledIndex4, 0, 0, 0);
    strip4.show();
}

void revealAnimation()
{
    int interval = 2000;
    int speed = 150;

    // STRIP 1
    for (int i = 0; i < LED_COUNT_STRIP_1; i++)
    {
        uint32_t color = strip1.ColorHSV((i * interval) + (timeCounter * speed), 255, 255);
        strip1.setPixelColor(i, color);
    }

    strip1.show();

    // STRIP 2
    for (int i = 0; i < LED_COUNT_STRIP_2; i++)
    {
        uint32_t color = strip2.ColorHSV((i * interval) + (timeCounter * speed), 255, 255);
        strip2.setPixelColor(i, color);
    }

    strip2.show();

    // STRIP 3
    for (int i = 0; i < LED_COUNT_STRIP_3; i++)
    {
        uint32_t color = strip3.ColorHSV((i * interval) + (timeCounter * speed), 255, 255);
        strip3.setPixelColor(i, color);
    }

    strip3.show();

    // STRIP 4
    for (int i = 0; i < LED_COUNT_STRIP_4; i++)
    {
        uint32_t color = strip4.ColorHSV((i * interval) + (timeCounter * speed), 255, 255);
        strip4.setPixelColor(i, color);
    }

    strip4.show();
}

void startAnimation()
{
    // STRIP 1
    for (int i = 0; i < LED_COUNT_STRIP_1; i++)
    {
        strip1.setPixelColor(i, 255, 0, 0);
    }

    strip1.show();

    // STRIP 2
    for (int i = 0; i < LED_COUNT_STRIP_2; i++)
    {
        strip2.setPixelColor(i, 255, 0, 0);
    }

    strip2.show();

    // STRIP 3
    for (int i = 0; i < LED_COUNT_STRIP_3; i++)
    {
        strip3.setPixelColor(i, 255, 0, 0);
    }

    strip3.show();

    // STRIP 4
    for (int i = 0; i < LED_COUNT_STRIP_4; i++)
    {
        strip4.setPixelColor(i, 255, 0, 0);
    }

    strip4.show();
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