const int VRx1 = 0; // Connect to Analog Pin 0
const int VRy1 = 1; // Connect to Analog Pin 1
const int SW1 = 7; // Connect to Digital Pin 7

const int VRx2 = 2; // Connect to Analog Pin 0
const int VRy2 = 3; // Connect to Analog Pin 1
const int SW2 = 8; // Connect to Digital Pin 7

const int baudRate = 9600;

void setup()
{
    setupSerial();
}

void setupSerial()
{
    Serial.begin(baudRate);
}

void loop()
{
    printJoystickData(1, analogRead(VRx1), analogRead(VRy1));
    printJoystickData(2, analogRead(VRx2), analogRead(VRy2));
}

void printJoystickData(int joystickId, int joystickDataX, int joystickDataY)
{
    Serial.println("id:" + String(joystickId) + "_" + "x:" + String(joystickDataX) + "_" + "y:" + String(joystickDataY));
}