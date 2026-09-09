// Reference alternative controller for the Axis arcade machine.
//
// Handshake only for now: the board announces itself at boot and answers the
// launcher's "type:ping". It sends no input events yet. See AxisAlternative.h
// for the protocol and the send helpers to use once inputs are wired.

#include "AxisAlternative.h"

const long BAUD_RATE = 115200; // what the launcher opens alternatives at

AxisAlternative axis(Serial, "reference", 1);

void setup()
{
    Serial.begin(BAUD_RATE);
    // Boards with native USB (Leonardo, ESP32-S2/S3) need the host to open the
    // port before anything is sent; harmless elsewhere.
    while (!Serial && millis() < 2000) {}
    axis.begin();
}

void loop()
{
    axis.poll();
}
