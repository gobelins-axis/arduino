// V6 - Teensy 4.1 arcade controller firmware (buttons only for now)
//
// Serial protocol (unchanged from V5):
//   type:button__key:a__id:1__state:keydown
//   type:button-home__key:home__id:0__state:keyup
//
// Every button is listed once in BUTTONS below. Nothing else is per-button.

#include "DebouncedButton.h"

// ---------------------------------------------------------------------------
// Configuration
// ---------------------------------------------------------------------------

// Baud rate is ignored by Teensy USB serial but kept for host compatibility.
const uint32_t BAUD_RATE = 28800;

// Wiring polarity.
//   true : button connects the pin to 3.3V, external pull-down to GND (current wiring)
//   false: button connects the pin to GND, internal pull-up (recommended if rewiring)
const bool BUTTONS_ACTIVE_HIGH = true;

// A level must be stable this long before it is accepted.
const uint16_t DEBOUNCE_MS = 10;

// Enable / disable serial output (useful when probing with a logic analyzer).
const bool SERIAL_OUTPUT_ENABLED = true;

struct ButtonConfig
{
    uint8_t     pin;
    const char* type;  // "button" or "button-home"
    const char* key;   // "a", "x", "i", "s", "w", "home"
    uint8_t     id;    // controller id, 0 for home
};

// Pins 10 to 13 are reserved for the LED strips, keep buttons off them.
// (Home and A1 are still on 12 and 10 to match the current wiring; move them
// before the strips come back.)
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

// ---------------------------------------------------------------------------
// State
// ---------------------------------------------------------------------------

DebouncedButton* buttons[BUTTON_COUNT];
uint32_t lastTickMs = 0;

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

    lastTickMs = millis();
    sendLine("type:ready__version:6");
}

void loop()
{
    // Poll on a fixed 1 ms tick. digitalRead on 7 pins takes microseconds,
    // so a free-running loop would only oversample the contact bounce.
    const uint32_t now = millis();
    if (now == lastTickMs) return;
    lastTickMs = now;

    for (size_t i = 0; i < BUTTON_COUNT; i++)
    {
        const DebouncedButton::Event evt = buttons[i]->update(now);
        if (evt == DebouncedButton::PRESSED)  sendButtonEvent(BUTTONS[i], "keydown");
        if (evt == DebouncedButton::RELEASED) sendButtonEvent(BUTTONS[i], "keyup");
    }
}

// ---------------------------------------------------------------------------
// Serial output
// ---------------------------------------------------------------------------

void sendButtonEvent(const ButtonConfig& cfg, const char* state)
{
    if (!SERIAL_OUTPUT_ENABLED) return;
    Serial.printf("type:%s__key:%s__id:%u__state:%s\n", cfg.type, cfg.key, cfg.id, state);
}

void sendLine(const char* line)
{
    if (!SERIAL_OUTPUT_ENABLED) return;
    Serial.println(line);
}
