#pragma once
#include <Arduino.h>

// Stable-time debounce: a new raw level must persist for `debounceMs`
// before it becomes the accepted state. Events are only emitted on
// transitions of the accepted state, so PRESSED and RELEASED always
// strictly alternate, whatever the contacts do.
class DebouncedButton
{
public:
    enum Event : uint8_t { NONE, PRESSED, RELEASED };

    DebouncedButton(uint8_t pin, bool activeHigh, uint16_t debounceMs)
        : _pin(pin), _activeHigh(activeHigh), _debounceMs(debounceMs) {}

    void begin()
    {
        pinMode(_pin, _activeHigh ? INPUT : INPUT_PULLUP);
        _lastRaw = readRaw();
        _lastChangeMs = millis();
        // Start as "released". If the button is physically held at boot,
        // a PRESSED event is emitted once the debounce window has elapsed.
        _stable = false;
    }

    // Call at a regular interval (1 ms). Returns the event that occurred, if any.
    Event update(uint32_t nowMs)
    {
        const bool raw = readRaw();

        if (raw != _lastRaw)
        {
            _lastRaw = raw;
            _lastChangeMs = nowMs;
            return NONE;
        }

        if (raw != _stable && (uint32_t)(nowMs - _lastChangeMs) >= _debounceMs)
        {
            _stable = raw;
            return _stable ? PRESSED : RELEASED;
        }

        return NONE;
    }

    bool isPressed() const { return _stable; }

private:
    bool readRaw() const
    {
        const int level = digitalRead(_pin);
        return _activeHigh ? (level == HIGH) : (level == LOW);
    }

    uint8_t  _pin;
    bool     _activeHigh;
    uint16_t _debounceMs;
    bool     _lastRaw = false;
    bool     _stable = false;
    uint32_t _lastChangeMs = 0;
};
