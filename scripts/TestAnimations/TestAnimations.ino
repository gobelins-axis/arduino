#include <Adafruit_NeoPixel.h>

// Serial
const int BAUD_RATE = 28800;

int time = 0;

// Animations
int currentIndex = 0;
String buildup1AnimationString = "buildup1";
String buildup2AnimationString = "buildup2";
String revealAnimationString = "reveal";
String startAnimationString = "start";

// Leds: Left Strip
const int PIN_STRIP_1 = 12;
const int LED_COUNT_STRIP_1 = 39;
Adafruit_NeoPixel strip1(LED_COUNT_STRIP_1, PIN_STRIP_1, NEO_GRB + NEO_KHZ800);

void setup()
{
    setupSerial();

    strip1.begin();
    strip1.show();
}

void setupSerial()
{
    Serial.begin(BAUD_RATE);
}

void loop()
{
    listenSerialPrez();

    ledAnimations();

    time++;
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

void buildup1Animation()
{
    float speed = 0.005;
    float sineAnimation = (cos(time * speed) + 1.0) / 2.0 * 255.0;

    for (int i = 0; i < LED_COUNT_STRIP_1; i++)
    {
        strip1.setPixelColor(i, sineAnimation, 0, 0);
    }

    strip1.show();
}

void buildup2Animation()
{
    int ledIndex = random(LED_COUNT_STRIP_1);
    strip1.setPixelColor(ledIndex, 255, 0, 0);
    strip1.show();
    strip1.setPixelColor(ledIndex, 0, 0, 0);
    strip1.show();
}

void revealAnimation()
{
    int interval = 2000;
    int speed = 150;

    for (int i = 0; i < LED_COUNT_STRIP_1; i++)
    {
        uint32_t color = strip1.ColorHSV((i * interval) + (time * speed), 255, 255);
        strip1.setPixelColor(i, color);
    }

    strip1.show();
}

void startAnimation()
{
    float animation = 255.0;

    if (animation <= 255.0)
    {
        for (int i = 0; i < LED_COUNT_STRIP_1; i++)
        {
            strip1.setPixelColor(i, int(animation), 0, 0);
        }

        strip1.show();
    }
}