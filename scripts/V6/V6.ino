// V6 - Teensy 4.1 arcade controller firmware (buttons + joysticks)
//
// Serial protocol (unchanged from V5):
//   type:button__key:a__id:1__state:keydown
//   type:button-home__key:home__id:0__state:keyup
//   type:joystick__id:1__x:512__y:512
//
// Every input is listed once in the BUTTONS / JOYSTICKS tables below.

#include "DebouncedButton.h"

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

// --- Joysticks -------------------------------------------------------------
//
// The joystick modules run on 5V and output 0..5V. Each signal wire goes
// through a 1:2 voltage divider (10k series, 20k to GND) before the Teensy
// pin, because Teensy 4.1 pins are 3.3V only.
//
// The firmware calibrates itself so the host always receives a clean
// 0..1023 signal centred on 512, whatever the physical range of the stick:
//   - the rest position is measured at boot and becomes the centre,
//   - the min / max of each axis start from an assumed travel and expand
//     as the stick is moved, so a full sweep gives full resolution.

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

// ADC: 10-bit to keep the 0..1023 range the host expects. 16 hardware
// samples are averaged per read, which removes most of the pot noise.
const uint8_t  ADC_RESOLUTION_BITS = 10;
const uint8_t  ADC_HW_AVERAGING = 16;

// Extra low-pass on top of hardware averaging. 0 < alpha <= 1, lower is smoother.
const float    JOYSTICK_SMOOTHING_ALPHA = 0.25f;

// Boot calibration: average the rest position for this long.
const uint16_t JOYSTICK_BOOT_CALIBRATION_MS = 300;
// If the measured rest position is outside this window the stick was probably
// held during boot; fall back to the nominal centre instead.
const uint16_t JOYSTICK_CENTER_MIN = 200;
const uint16_t JOYSTICK_CENTER_MAX = 823;
const uint16_t JOYSTICK_CENTER_DEFAULT = 512;
// Assumed travel on each side of the centre before a full sweep has been seen.
// Smaller than the real travel, so full deflection saturates (and menus react)
// straight after boot; the range then grows to the real one as the stick moves.
const uint16_t JOYSTICK_INITIAL_HALF_RANGE = 300;

// A message is sent when either axis moved at least this many counts
// since the last message...
const uint16_t JOYSTICK_CHANGE_THRESHOLD = 4;
// ...but never more often than this (caps the stream at 100 messages/s)...
const uint32_t JOYSTICK_MIN_SEND_INTERVAL_MS = 10;
// ...and at least this often even when idle, so the host keeps a live value.
const uint32_t JOYSTICK_KEEPALIVE_INTERVAL_MS = 50;

// Debug: also send the raw ADC values as "type:joystick-raw__..." lines.
// The launcher ignores unknown types, so this is safe to leave on.
const bool     JOYSTICK_SEND_RAW = false;

// ---------------------------------------------------------------------------
// State
// ---------------------------------------------------------------------------

DebouncedButton* buttons[BUTTON_COUNT];

struct AxisCalibration
{
    float center;
    float min;
    float max;
};

struct JoystickState
{
    float           filteredX;
    float           filteredY;
    AxisCalibration calX;
    AxisCalibration calY;
    int16_t         lastSentX;
    int16_t         lastSentY;
    uint32_t        lastSendMs;
};

JoystickState joysticks[JOYSTICK_COUNT];
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

    analogReadResolution(ADC_RESOLUTION_BITS);
    analogReadAveraging(ADC_HW_AVERAGING);

    for (size_t i = 0; i < JOYSTICK_COUNT; i++)
    {
        calibrateJoystickAtBoot(i);
    }

    lastTickMs = millis();
    sendLine("type:ready__version:6");
}

void loop()
{
    // The host sends LED / action commands we do not handle yet. Drain them so
    // the USB receive buffer never fills up.
    while (Serial.available() > 0) Serial.read();

    // Everything below runs on a fixed 1 ms tick.
    const uint32_t now = millis();
    if (now == lastTickMs) return;
    lastTickMs = now;

    // Buttons first so their events are never queued behind joystick data.
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
}

// ---------------------------------------------------------------------------
// Joysticks
// ---------------------------------------------------------------------------

void calibrateJoystickAtBoot(size_t index)
{
    const JoystickConfig& cfg = JOYSTICKS[index];
    JoystickState& st = joysticks[index];

    // Average the rest position.
    float sumX = 0, sumY = 0;
    uint32_t samples = 0;
    const uint32_t start = millis();
    while (millis() - start < JOYSTICK_BOOT_CALIBRATION_MS)
    {
        sumX += analogRead(cfg.pinX);
        sumY += analogRead(cfg.pinY);
        samples++;
        delay(1);
    }

    initAxisCalibration(st.calX, sumX / samples);
    initAxisCalibration(st.calY, sumY / samples);

    st.filteredX = st.calX.center;
    st.filteredY = st.calY.center;
    st.lastSentX = -1;   // force a first message
    st.lastSentY = -1;
    st.lastSendMs = 0;
}

void initAxisCalibration(AxisCalibration& cal, float restValue)
{
    const bool plausible = restValue >= JOYSTICK_CENTER_MIN && restValue <= JOYSTICK_CENTER_MAX;
    cal.center = plausible ? restValue : JOYSTICK_CENTER_DEFAULT;
    cal.min = max(0.0f,    cal.center - JOYSTICK_INITIAL_HALF_RANGE);
    cal.max = min(1023.0f, cal.center + JOYSTICK_INITIAL_HALF_RANGE);
}

// Maps a filtered raw value to 0..1023 with the calibrated centre at 512.
// Each side of the centre is scaled independently so an off-centre rest
// position still gives a symmetric output.
int16_t normalizeAxis(float value, AxisCalibration& cal)
{
    // Learn the real travel as the stick is moved.
    if (value < cal.min) cal.min = value;
    if (value > cal.max) cal.max = value;

    float n;
    if (value < cal.center) n = (value - cal.center) / (cal.center - cal.min);  // -1..0
    else                    n = (value - cal.center) / (cal.max - cal.center);  //  0..1

    n = constrain(n, -1.0f, 1.0f);
    return (int16_t)constrain(512.0f + n * 512.0f, 0.0f, 1023.0f);
}

void updateJoystick(size_t index, uint32_t now)
{
    const JoystickConfig& cfg = JOYSTICKS[index];
    JoystickState& st = joysticks[index];

    // Sample every tick (two averaged reads cost well under 0.1 ms).
    st.filteredX += (analogRead(cfg.pinX) - st.filteredX) * JOYSTICK_SMOOTHING_ALPHA;
    st.filteredY += (analogRead(cfg.pinY) - st.filteredY) * JOYSTICK_SMOOTHING_ALPHA;

    const int16_t x = normalizeAxis(st.filteredX, st.calX);
    const int16_t y = normalizeAxis(st.filteredY, st.calY);

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

    if (JOYSTICK_SEND_RAW)
    {
        sendJoystickRaw(cfg.id, (int16_t)(st.filteredX + 0.5f), (int16_t)(st.filteredY + 0.5f));
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

void sendJoystickRaw(uint8_t id, int16_t x, int16_t y)
{
    if (!SERIAL_OUTPUT_ENABLED) return;
    Serial.printf("type:joystick-raw__id:%u__x:%d__y:%d\r\n", id, x, y);
}

void sendLine(const char* line)
{
    if (!SERIAL_OUTPUT_ENABLED) return;
    Serial.println(line);
}
