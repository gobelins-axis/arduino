#pragma once
#include <Arduino.h>
#include "LedEngine.h"

// Non-blocking line reader for commands sent by the host (launcher / axis-api).
//
// Accepted lines (terminated by '\n', a trailing '\r' is ignored):
//
//   Legacy per-pixel format sent by every shipped axis-api build. Frozen.
//     strip;index;r,g,b                e.g. 3;1;255,0,0
//
//   Additive commands in the protocol's "type:" style, 1-based strip numbers:
//     type:led__cmd:set__strip:1__index:3__color:255,0,0
//     type:led__cmd:fill__strip:1__color:255,0,0
//     type:led__cmd:range__strip:1__from:0__to:50__color:255,0,0
//     type:led__cmd:clear
//     type:led__cmd:brightness__value:128
//
// Anything else is ignored, so leftovers from older hosts (e.g. "start") are harmless.
class SerialCommands
{
public:
    SerialCommands(Stream& serial, LedEngine& leds) : _serial(serial), _leds(leds) {}

    // Reads whatever bytes are available and handles complete lines.
    void poll()
    {
        while (_serial.available() > 0)
        {
            const char c = (char)_serial.read();

            if (c == '\n')
            {
                _line[_length] = '\0';
                if (_length > 0) handleLine(_line);
                _length = 0;
            }
            else if (c != '\r')
            {
                if (_length < MAX_LINE_LENGTH - 1) _line[_length++] = c;
                else _length = 0;   // line too long: drop it and resync on the next newline
            }
        }
    }

private:
    static const size_t MAX_LINE_LENGTH = 96;

    void handleLine(char* line)
    {
        if (strncmp(line, "type:led__", 10) == 0) handleTyped(line);
        else if (isDigit(line[0]) && strchr(line, ';') != nullptr) handleLegacyPixel(line);
    }

    // strip;index;r,g,b
    void handleLegacyPixel(char* line)
    {
        char* rest = line;
        const char* stripStr = nextField(rest, ';');
        const char* indexStr = nextField(rest, ';');
        const char* colorStr = rest;
        if (!stripStr || !indexStr || !colorStr) return;

        uint8_t rgb[3];
        if (!parseColor(colorStr, rgb)) return;

        _leds.setPixel(toStrip(stripStr), (uint16_t)atoi(indexStr), rgb[0], rgb[1], rgb[2]);
    }

    // type:led__cmd:<name>__key:value...
    void handleTyped(const char* line)
    {
        char cmd[16], strip[8], index[8], from[8], to[8], color[16], value[8];
        if (!getValue(line, "cmd", cmd, sizeof(cmd))) return;

        if (strcmp(cmd, "clear") == 0)
        {
            _leds.clear();
            return;
        }

        if (strcmp(cmd, "brightness") == 0)
        {
            if (getValue(line, "value", value, sizeof(value)))
                _leds.setBrightness((uint8_t)constrain(atoi(value), 0, 255));
            return;
        }

        uint8_t rgb[3];
        if (!getValue(line, "strip", strip, sizeof(strip))) return;
        if (!getValue(line, "color", color, sizeof(color)) || !parseColor(color, rgb)) return;
        const uint8_t s = toStrip(strip);

        if (strcmp(cmd, "set") == 0)
        {
            if (getValue(line, "index", index, sizeof(index)))
                _leds.setPixel(s, (uint16_t)atoi(index), rgb[0], rgb[1], rgb[2]);
        }
        else if (strcmp(cmd, "fill") == 0)
        {
            _leds.fill(s, rgb[0], rgb[1], rgb[2]);
        }
        else if (strcmp(cmd, "range") == 0)
        {
            if (getValue(line, "from", from, sizeof(from)) && getValue(line, "to", to, sizeof(to)))
                _leds.fillRange(s, (uint16_t)atoi(from), (uint16_t)atoi(to), rgb[0], rgb[1], rgb[2]);
        }
    }

    // --- Helpers ---

    // 1-based on the wire (axis-api config), 0-based in the engine. Unknown -> 255 (ignored by the engine).
    static uint8_t toStrip(const char* str)
    {
        const int n = atoi(str);
        return (n >= 1 && n <= LedEngine::MAX_STRIPS) ? (uint8_t)(n - 1) : 255;
    }

    // Splits at the next `sep`, returns the field and advances `rest` past it.
    static char* nextField(char*& rest, char sep)
    {
        if (!rest) return nullptr;
        char* field = rest;
        char* p = strchr(rest, sep);
        if (p) { *p = '\0'; rest = p + 1; }
        else rest = nullptr;
        return field;
    }

    // "r,g,b" -> rgb, each clamped to 0..255
    static bool parseColor(const char* str, uint8_t rgb[3])
    {
        int values[3];
        for (int i = 0; i < 3; i++)
        {
            char* end;
            const long v = strtol(str, &end, 10);
            if (end == str) return false;
            values[i] = constrain(v, 0L, 255L);
            if (i < 2)
            {
                if (*end != ',') return false;
                str = end + 1;
            }
        }
        rgb[0] = values[0]; rgb[1] = values[1]; rgb[2] = values[2];
        return true;
    }

    // Finds "key:" at the start of a "__"-separated token and copies its value.
    static bool getValue(const char* line, const char* key, char* out, size_t outSize)
    {
        const size_t keyLength = strlen(key);
        const char* p = line;

        while (p && *p)
        {
            if (strncmp(p, key, keyLength) == 0 && p[keyLength] == ':')
            {
                const char* value = p + keyLength + 1;
                const char* end = strstr(value, "__");
                const size_t length = end ? (size_t)(end - value) : strlen(value);
                if (length == 0 || length >= outSize) return false;
                memcpy(out, value, length);
                out[length] = '\0';
                return true;
            }
            p = strstr(p, "__");
            if (p) p += 2;
        }
        return false;
    }

    Stream&    _serial;
    LedEngine& _leds;
    char       _line[MAX_LINE_LENGTH];
    size_t     _length = 0;
};
