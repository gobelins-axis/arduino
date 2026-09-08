#pragma once
#include <Arduino.h>
#include <OctoWS2811.h>

// Frame buffer for the WS2812 strips, pushed over DMA by OctoWS2811.
//
// The board holds one colour per pixel and nothing else: no animations, no
// idle pattern. Colours are set by the host (axis-api through the launcher),
// coalesced here and sent at a fixed frame rate. OctoWS2811::show() returns
// immediately and the transfer runs on DMA, so the input path is never blocked.
//
// Strips are addressed 0-based here; the serial protocol uses the 1-based
// numbers from axis-api's LED config.
class LedEngine
{
public:
    static const uint8_t  MAX_STRIPS = 4;
    static const uint16_t MAX_LEDS_PER_STRIP = 64;

    LedEngine(OctoWS2811& leds, uint8_t stripCount, const uint16_t* stripLengths,
              uint16_t ledsPerStrip, uint8_t brightness, uint32_t frameIntervalMs)
        : _leds(leds), _stripCount(stripCount < MAX_STRIPS ? stripCount : MAX_STRIPS), _ledsPerStrip(ledsPerStrip),
          _brightness(brightness), _frameIntervalMs(frameIntervalMs)
    {
        for (uint8_t s = 0; s < MAX_STRIPS; s++)
        {
            _stripLengths[s] = (s < _stripCount) ? (stripLengths[s] < MAX_LEDS_PER_STRIP ? stripLengths[s] : MAX_LEDS_PER_STRIP) : 0;
        }
        memset(_colors, 0, sizeof(_colors));
    }

    void begin()
    {
        _leds.begin();
        clear();
        _dirty = true;
        push();   // start dark
    }

    // --- Colour commands (all bounds-checked, out-of-range is ignored) ---

    void setPixel(uint8_t strip, uint16_t index, uint8_t r, uint8_t g, uint8_t b)
    {
        if (strip >= _stripCount || index >= _stripLengths[strip]) return;
        uint8_t* px = _colors[strip][index];
        if (px[0] == r && px[1] == g && px[2] == b) return;
        px[0] = r; px[1] = g; px[2] = b;
        _dirty = true;
    }

    void fillRange(uint8_t strip, uint16_t from, uint16_t to, uint8_t r, uint8_t g, uint8_t b)
    {
        if (strip >= _stripCount || _stripLengths[strip] == 0) return;
        if (from > to) { const uint16_t t = from; from = to; to = t; }
        const uint16_t last = (to < _stripLengths[strip]) ? to : (uint16_t)(_stripLengths[strip] - 1);
        for (uint16_t i = from; i <= last; i++) setPixel(strip, i, r, g, b);
    }

    void fill(uint8_t strip, uint8_t r, uint8_t g, uint8_t b)
    {
        if (strip >= _stripCount || _stripLengths[strip] == 0) return;
        fillRange(strip, 0, _stripLengths[strip] - 1, r, g, b);
    }

    void clear()
    {
        memset(_colors, 0, sizeof(_colors));
        _dirty = true;
    }

    void setBrightness(uint8_t brightness)
    {
        if (brightness == _brightness) return;
        _brightness = brightness;
        _dirty = true;
    }

    uint8_t brightness() const { return _brightness; }

    // --- Frame pacing, call every tick ---

    void update(uint32_t nowMs)
    {
        if (!_dirty) return;
        if ((uint32_t)(nowMs - _lastFrameMs) < _frameIntervalMs) return;
        if (_leds.busy()) return;   // previous DMA transfer still running, try next tick
        _lastFrameMs = nowMs;
        push();
    }

private:
    void push()
    {
        for (uint8_t s = 0; s < _stripCount; s++)
        {
            for (uint16_t i = 0; i < _stripLengths[s]; i++)
            {
                const uint8_t* px = _colors[s][i];
                _leds.setPixel(s * _ledsPerStrip + i, scale(px[0]), scale(px[1]), scale(px[2]));
            }
        }
        _leds.show();
        _dirty = false;
    }

    uint8_t scale(uint8_t v) const
    {
        return (uint8_t)(((uint16_t)v * _brightness + 127) / 255);
    }

    OctoWS2811& _leds;
    uint8_t     _stripCount;
    uint16_t    _stripLengths[MAX_STRIPS];
    uint16_t    _ledsPerStrip;
    uint8_t     _brightness;
    uint32_t    _frameIntervalMs;
    uint32_t    _lastFrameMs = 0;
    bool        _dirty = false;
    uint8_t     _colors[MAX_STRIPS][MAX_LEDS_PER_STRIP][3];
};
