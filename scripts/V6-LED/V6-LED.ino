// V6-LED - Teensy 4.1 arcade controller firmware: buttons + joysticks + LED strips
//
// Work-in-progress copy of V6 with the LED engine. V6 stays the reference until
// this is validated on the machine; the input code is identical to V6.
//
// Serial protocol out (unchanged):
//   type:button__key:a__id:1__state:keydown
//   type:button-home__key:home__id:0__state:keyup
//   type:joystick__id:1__x:512__y:512
//
// Serial protocol in: see SerialCommands.h (legacy "strip;index;r,g,b" lines
// from axis-api, plus "type:led__cmd:..." commands).

#include <OctoWS2811.h>
#include "DebouncedButton.h"
#include "LedEngine.h"
#include "SerialCommands.h"

// ---------------------------------------------------------------------------
// Configuration
// ---------------------------------------------------------------------------

// Baud rate is ignored by Teensy USB serial but kept for host compatibility.
const uint32_t BAUD_RATE = 28800;

// Enable / disable serial output (useful when probing with a logic analyzer).
const bool SERIAL_OUTPUT_ENABLED = true;

// --- Buttons ---------------------------------------------------------------

// Wiring polarity.
//   true : button connects the pin to 3.3V, external pull-down to GND (current wiring)
//   false: button connects the pin to GND, internal pull-up (recommended if rewiring)
const bool BUTTONS_ACTIVE_HIGH = true;

// A level must be stable this long before it is accepted.
const uint16_t DEBOUNCE_MS = 10;

struct ButtonConfig
{
    uint8_t     pin;
    const char* type;  // "button" or "button-home"
    const char* key;   // "a", "x", "i", "s", "w", "home"
    uint8_t     id;    // controller id, 0 for home
};

const ButtonConfig BUTTONS[] = {
    { 12, "button-home", "home", 0 },

    // Controller 1
    { 10, "button", "a", 1 },
    {  8, "button", "x", 1 },
    {  6, "button", "i", 1 },
    {  4, "button", "s", 1 },
    {  2, "button", "w", 1 },

    // Controller 2 (only W is wired today)
    { 24, "button", "w", 2 },
    // {  9, "button", "a", 2 },
    // {  7, "button", "x", 2 },
    // {  5, "button", "i", 2 },
    // {  3, "button", "s", 2 },
};

const size_t BUTTON_COUNT = sizeof(BUTTONS) / sizeof(BUTTONS[0]);

// --- Joysticks -------------------------------------------------------------
//
// The joystick modules run on 5V and output 0..5V. Each signal wire goes
// through a 1:2 voltage divider (10k series, 20k to GND) before the Teensy
// pin, because Teensy 4.1 pins are 3.3V only.
//
// The firmware sends smoothed RAW 10-bit values (0..1023). Centre, range and
// axis orientation are calibrated in the launcher, not here.

struct JoystickConfig
{
    uint8_t id;
    uint8_t pinX;
    uint8_t pinY;
};

const JoystickConfig JOYSTICKS[] = {
    { 1, A13, A12 },   // pins 27 / 26
    // { 2, A4,  A5  },   // pins 18 / 19
};

const size_t JOYSTICK_COUNT = sizeof(JOYSTICKS) / sizeof(JOYSTICKS[0]);

const uint8_t  ADC_RESOLUTION_BITS = 10;
const uint8_t  ADC_HW_AVERAGING = 16;
const float    JOYSTICK_SMOOTHING_ALPHA = 0.25f;
const uint16_t JOYSTICK_CHANGE_THRESHOLD = 4;
const uint32_t JOYSTICK_MIN_SEND_INTERVAL_MS = 10;
const uint32_t JOYSTICK_KEEPALIVE_INTERVAL_MS = 50;

// --- LED strips ------------------------------------------------------------
//
// WS2812 strips driven by OctoWS2811 over DMA (non-blocking). Data lines go
// through a 74AHCT125 level shifter (3.3V -> 5V); strips are powered by a
// dedicated 5V supply sharing ground with the Teensy.
//
// Strip numbers match axis-api's LED config (1-based on the wire):
//   1: left strip (51)   2: right strip (55)
//   3: controller 1 buttons (6: W A X I S Home)   4: controller 2 buttons (5: W A X I S)

const uint8_t  LED_STRIP_COUNT = 4;
const uint8_t  LED_PINS[LED_STRIP_COUNT] = { 14, 15, 16, 17 };
const uint16_t LED_STRIP_LENGTHS[LED_STRIP_COUNT] = { 51, 55, 6, 5 };
// OctoWS2811 sends the same length on every pin; extra pixels past a strip's end are just black.
const uint16_t LEDS_PER_STRIP = 55;

// Global brightness cap (0..255). 117 LEDs draw up to 7 A at full white; set this
// from the measured supply current. Games can lower it further, never raise it above 255.
const uint8_t  LED_BRIGHTNESS = 96;

// Frame rate for pushing colour changes to the strips.
const uint32_t LED_FRAME_INTERVAL_MS = 16;   // ~60 Hz

// Debug: light every strip full red at boot to check wiring, level shifter and
// power. The colour stays until the host sends its first LED command.
const bool LED_DEBUG_RED_ON_START = true;

// ---------------------------------------------------------------------------
// State
// ---------------------------------------------------------------------------

DebouncedButton* buttons[BUTTON_COUNT];

struct JoystickState
{
    float    filteredX;
    float    filteredY;
    int16_t  lastSentX;
    int16_t  lastSentY;
    uint32_t lastSendMs;
};

JoystickState joysticks[JOYSTICK_COUNT];
uint32_t lastTickMs = 0;

// OctoWS2811 buffers must be global (display buffer in DMA-capable memory).
const int LED_BYTES_PER_PIXEL = 3;
DMAMEM int ledDisplayMemory[LEDS_PER_STRIP * LED_STRIP_COUNT * LED_BYTES_PER_PIXEL / 4];
int ledDrawingMemory[LEDS_PER_STRIP * LED_STRIP_COUNT * LED_BYTES_PER_PIXEL / 4];

OctoWS2811 octo(LEDS_PER_STRIP, ledDisplayMemory, ledDrawingMemory,
                WS2811_GRB | WS2811_800kHz, LED_STRIP_COUNT, LED_PINS);
LedEngine ledEngine(octo, LED_STRIP_COUNT, LED_STRIP_LENGTHS, LEDS_PER_STRIP,
                    LED_BRIGHTNESS, LED_FRAME_INTERVAL_MS);
SerialCommands serialCommands(Serial, ledEngine);

// ---------------------------------------------------------------------------
// Setup / loop
// ---------------------------------------------------------------------------

void setup()
{
    Serial.begin(BAUD_RATE);

    for (size_t i = 0; i < BUTTON_COUNT; i++)
    {
        buttons[i] = new DebouncedButton(BUTTONS[i].pin, BUTTONS_ACTIVE_HIGH, DEBOUNCE_MS);
        buttons[i]->begin();
    }

    analogReadResolution(ADC_RESOLUTION_BITS);
    analogReadAveraging(ADC_HW_AVERAGING);

    for (size_t i = 0; i < JOYSTICK_COUNT; i++)
    {
        joysticks[i].filteredX = analogRead(JOYSTICKS[i].pinX);
        joysticks[i].filteredY = analogRead(JOYSTICKS[i].pinY);
        joysticks[i].lastSentX = -1;   // force a first message
        joysticks[i].lastSentY = -1;
        joysticks[i].lastSendMs = 0;
    }

    ledEngine.begin();   // strips start dark
    if (LED_DEBUG_RED_ON_START) debugLightAllRed();

    lastTickMs = millis();
    sendLine("type:ready__version:6-led");
}

void loop()
{
    // Everything runs on a fixed 1 ms tick.
    const uint32_t now = millis();
    if (now == lastTickMs) return;
    lastTickMs = now;

    // Inputs first: their events are never queued behind LED work.
    for (size_t i = 0; i < BUTTON_COUNT; i++)
    {
        const DebouncedButton::Event evt = buttons[i]->update(now);
        if (evt == DebouncedButton::PRESSED)  sendButtonEvent(BUTTONS[i], "keydown");
        if (evt == DebouncedButton::RELEASED) sendButtonEvent(BUTTONS[i], "keyup");
    }

    for (size_t i = 0; i < JOYSTICK_COUNT; i++)
    {
        updateJoystick(i, now);
    }

    // Host commands (LED colours) update the frame buffer, the engine pushes
    // it at most once per frame, over DMA.
    serialCommands.poll();
    ledEngine.update(now);
}

// ---------------------------------------------------------------------------
// Joysticks
// ---------------------------------------------------------------------------

void updateJoystick(size_t index, uint32_t now)
{
    const JoystickConfig& cfg = JOYSTICKS[index];
    JoystickState& st = joysticks[index];

    st.filteredX += (analogRead(cfg.pinX) - st.filteredX) * JOYSTICK_SMOOTHING_ALPHA;
    st.filteredY += (analogRead(cfg.pinY) - st.filteredY) * JOYSTICK_SMOOTHING_ALPHA;

    const int16_t x = (int16_t)(st.filteredX + 0.5f);
    const int16_t y = (int16_t)(st.filteredY + 0.5f);

    const uint32_t sinceLastSend = now - st.lastSendMs;
    const bool moved = abs(x - st.lastSentX) >= JOYSTICK_CHANGE_THRESHOLD
                    || abs(y - st.lastSentY) >= JOYSTICK_CHANGE_THRESHOLD;

    const bool sendOnMove = moved && sinceLastSend >= JOYSTICK_MIN_SEND_INTERVAL_MS;
    const bool sendKeepalive = sinceLastSend >= JOYSTICK_KEEPALIVE_INTERVAL_MS;

    if (!sendOnMove && !sendKeepalive) return;

    st.lastSentX = x;
    st.lastSentY = y;
    st.lastSendMs = now;
    sendJoystick(cfg.id, x, y);
}

// ---------------------------------------------------------------------------
// LED debug
// ---------------------------------------------------------------------------

// Fills every strip with full red. The engine pushes it on the first frame of loop().
void debugLightAllRed()
{
    for (uint8_t s = 0; s < LED_STRIP_COUNT; s++)
    {
        ledEngine.fill(s, 255, 0, 0);
    }
}

// ---------------------------------------------------------------------------
// Serial output
// ---------------------------------------------------------------------------

// The host splits lines on "\r\n" (same terminator as Serial.println).

void sendButtonEvent(const ButtonConfig& cfg, const char* state)
{
    if (!SERIAL_OUTPUT_ENABLED) return;
    Serial.printf("type:%s__key:%s__id:%u__state:%s\r\n", cfg.type, cfg.key, cfg.id, state);
}

void sendJoystick(uint8_t id, int16_t x, int16_t y)
{
    if (!SERIAL_OUTPUT_ENABLED) return;
    Serial.printf("type:joystick__id:%u__x:%d__y:%d\r\n", id, x, y);
}

void sendLine(const char* line)
{
    if (!SERIAL_OUTPUT_ENABLED) return;
    Serial.println(line);
}
