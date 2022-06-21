// Serial
const int BAUD_RATE = 28800;

const int Joystick1X = 4;
const int Joystick1Y = 5;

int time = 0;

void setup()
{
    setupSerial();
}

void setupSerial()
{
    Serial.begin(BAUD_RATE);
}

void loop()
{
    time = time + 1;

    if (time % 2 == 0)
    {
        Serial.println('0');
    }
    else
    {
        Serial.println('1');
    }
}