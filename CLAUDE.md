# Axis arcade machine – controller firmware

Firmware for the input board of the Axis arcade machine: reads the arcade buttons
and joysticks on a **Teensy 4.1** and streams events over USB serial to the
Electron launcher (`../axis-launcher-electron`). The cross-repo picture, the serial
protocol contract and the design system live in `../CLAUDE.md`.

`scripts/V6` is the current firmware. `V1`–`V5` are kept for history only; V5 is the
last version of the old code (r89m Button library, Nano-era assumptions).

`scripts/AlternativeController` is not cabinet firmware: it is the reference sketch for
**alternative controllers**, extra boards (Uno, Nano, ESP32, Teensy...) plugged into the
machine while it runs. `AxisAlternative.h` implements the protocol the launcher expects:
115200 baud, answer `type:ping` (and announce from `setup()`) with
`type:ready__controller:<name>__version:<n>`, then send `type:alternative__<fields>` lines.
Copy the header into a new sketch; the `.ino` currently does the handshake only, no inputs yet.

## Hardware

- **Teensy 4.1**: 3.3V logic, pins are **not** 5V tolerant. Replaced an Arduino Nano.
- Buttons wired **active high**: pressing connects the pin to 3.3V, external pull-down to GND. Pin mode `INPUT`, HIGH = pressed. One constant switches polarity (`BUTTONS_ACTIVE_HIGH`, set it to `false` for buttons-to-GND with internal pull-ups).
- Pin map (the `BUTTONS` table in `V6.ino` is the source of truth):

  | Input | Pin |
  |---|---|
  | Home | 12 |
  | Controller 1 A / X / I / S / W | 10 / 8 / 6 / 4 / 2 |
  | Controller 2 W | 24 |
  | Controller 2 A / X / I / S | planned 9 / 7 / 5 / 3, not wired |
  | Joystick 1 X / Y | A13 / A12 (pins 27 / 26) |
  | Joystick 2 X / Y | planned A4 / A5 (pins 18 / 19) |

- **Pins 10–13 are reserved for the WS2812 LED strips** (disconnected today, all LED code removed). Home (12) and A1 (10) must move off those pins before strips return; NeoPixel `begin()` turns them into outputs and shorts the buttons.
- Joystick modules have 4 wires (X, Y, 5V, GND) and only work on 5V, so outputs swing 0–5V. Each signal wire goes through a **1:2 divider** (10k series, 20k to GND) before the Teensy pin. With that divider the Teensy reads the same counts the Nano did.

## Serial output (contract, see `../CLAUDE.md`)

Every line ends with `\r\n`. Button events: `type:button__key:a__id:1__state:keydown`, home: `type:button-home__key:home__id:0__state:keyup`. Joysticks: `type:joystick__id:1__x:512__y:512` with **raw** smoothed 10-bit values, never scaled or centred here: calibration is done in the launcher. `type:ready__version:6` once at boot.

## V6 design

- `DebouncedButton.h`: own stable-time debouncer (a level must hold 10 ms; events only on transitions, so keydown/keyup strictly alternate). No external library. Why: the old r89m `Button` library had no debounce and only fired its release callback if ≥1 ms had passed since the press, so contact bounce on the fast Teensy produced repeated keydowns and dropped keyups. On the Nano the slow 28800-baud prints had masked it.
- Table-driven config; fixed 1 ms tick; buttons processed before joysticks.
- Joysticks: `analogReadResolution(10)`, 16-sample hardware averaging, light low-pass. A message is sent when an axis moves ≥4 counts, at most every 10 ms, plus a 50 ms keep-alive. The old firmware printed every loop iteration and starved button events.
- Incoming serial is drained (the launcher writes action strings the board does not handle yet).
- Adding an input = one line in `BUTTONS` or `JOYSTICKS`.

## Build and flash

- Arduino IDE (on the cabinet's Mac): copy the whole `scripts/V6` folder, open `V6.ino`, board Teensy 4.1. The IDE compiles `DebouncedButton.h` from the same folder; no library to install.
- CLI: `arduino-cli compile --fqbn teensy:avr:teensy41 scripts/V6` (Teensy core installed on the dev Mac).
- `.env` still holds the Nano FQBN used by `build.js`; set `FQBN=teensy:avr:teensy41` to use `node build.js --name V6`. `build.js` and the repo's `serialport` v9 do not load on Node 22; use an older Node or bump `serialport`.
- Close the Arduino IDE serial monitor before launching the app, or the app cannot open the port.

## Pending

- Wire controller 2 A/X/I/S and joystick 2, uncomment their table lines.
- Bring LED strips back with a non-blocking library (WS2812Serial or OctoWS2811; NeoPixel's `show()` disables interrupts) on pins that do not clash with buttons, and add a serial command parser for the launcher's action strings.
- `scripts/V6/HANDOFF.md` is a narrative of the V5 → V6 investigation, useful context, not documentation to maintain.
