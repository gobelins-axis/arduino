const int VRx1 = 0; // Connect to Analog Pin 0
const int VRy1 = 1; // Connect to Analog Pin 1
const int SW1 = 7; // Connect to Digital Pin 7

const int baudRate = 9600;

void setup()
{
    setupSerial();
    pinMode(SW1, INPUT);
    digitalWrite(SW1, HIGH);
}

void setupSerial()
{
    Serial.begin(baudRate);
}

void loop()
{
// Joystick 1:
    Serial.println("hello");
    Serial.println(analogRead(VRx1));
    Serial.print("y-axis tilt: ");
    Serial.println(analogRead(VRy1));
    delay(800);
}
