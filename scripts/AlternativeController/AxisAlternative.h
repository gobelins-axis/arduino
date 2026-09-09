#pragma once
#include <Arduino.h>

// Alternative controller protocol for the Axis arcade machine.
//
// Any board with a USB serial port (Uno, Nano, ESP32, Teensy...) can be plugged
// into the machine while it runs. The launcher opens the port at 115200 baud,
// sends "type:ping" and waits three seconds for the board to announce itself:
//
//     type:ready__controller:<name>__version:<n>
//
// Boards that stay silent are closed and ignored until unplugged. Once accepted
// the board sends its own events, one per line, always prefixed with
// "type:alternative__" and made of key:value fields joined by "__":
//
//     type:alternative__key:fire__state:keydown
//     type:alternative__key:knob__value:512
//
// The launcher forwards the fields untouched to the page, so the game and the
// controller agree on the names between them. Every line ends with "\r\n".
//
// Most boards reset when the port is opened and send "ready" from setup(); a
// Teensy does not reset, which is what the ping is for. Both paths are handled
// here: call begin() in setup() and poll() in loop().
class AxisAlternative
{
public:
    AxisAlternative(Stream& serial, const char* name, int version = 1)
        : _serial(serial), _name(name), _version(version) {}

    // Announces the controller. Call once the serial port is up.
    void begin()
    {
        sendReady();
    }

    // Answers the launcher's ping. Call every loop().
    void poll()
    {
        while (_serial.available() > 0)
        {
            const char c = (char)_serial.read();

            if (c == '\n')
            {
                _line[_length] = '\0';
                if (strcmp(_line, "type:ping") == 0) sendReady();
                _length = 0;
            }
            else if (c != '\r')
            {
                if (_length < MAX_LINE_LENGTH - 1) _line[_length++] = c;
                else _length = 0;
            }
        }
    }

    // Sends "type:alternative__<fields>". `fields` is already formatted,
    // e.g. "key:fire__state:keydown".
    void send(const char* fields)
    {
        _serial.print("type:alternative__");
        _serial.print(fields);
        _serial.print("\r\n");
    }

    // Convenience: a button-like event.
    void sendState(const char* key, bool pressed)
    {
        _serial.print("type:alternative__key:");
        _serial.print(key);
        _serial.print("__state:");
        _serial.print(pressed ? "keydown" : "keyup");
        _serial.print("\r\n");
    }

    // Convenience: a numeric value (potentiometer, distance, encoder...).
    void sendValue(const char* key, long value)
    {
        _serial.print("type:alternative__key:");
        _serial.print(key);
        _serial.print("__value:");
        _serial.print(value);
        _serial.print("\r\n");
    }

private:
    static const size_t MAX_LINE_LENGTH = 32;

    void sendReady()
    {
        _serial.print("type:ready__controller:");
        _serial.print(_name);
        _serial.print("__version:");
        _serial.print(_version);
        _serial.print("\r\n");
    }

    Stream& _serial;
    const char* _name;
    int _version;
    char _line[MAX_LINE_LENGTH];
    size_t _length = 0;
};
