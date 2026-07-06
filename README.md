# esp32-macropad

USB HID macropad firmware for an ESP32-S3, built with PlatformIO + Arduino framework.
Uses the S3's native USB peripheral so the board enumerates directly as a USB keyboard
(no separate USB-to-serial chip or extra library needed).

## Which USB port to use

Most ESP32-S3 dev boards have two USB-C ports:
- **"USB" / "USB-OTG"** — wired straight to GPIO19/20, the native USB peripheral. **Use this one.**
- **"UART" / "COM"** — goes through a USB-serial bridge chip, can't do USB HID.

If your board only has one port, it's almost certainly the native one — that's fine.

## Wiring (defaults in `src/main.cpp`)

| Function | GPIO | Other leg |
|---|---|---|
| Button 1 | 4 | GND |
| Button 2 | 5 | GND |
| Button 3 | 6 | GND |
| Encoder A | 7 | — |
| Encoder B | 15 | — |
| Encoder push-switch | 16 | GND |
| Encoder common | GND | — |

Buttons and the encoder switch use the ESP32's internal pull-ups (`INPUT_PULLUP`), so no
external resistors are needed — just wire each contact between the GPIO and GND.

Avoid these pins when adding more buttons/encoders: `0` (BOOT), `3, 45, 46` (strapping
pins), `19, 20` (native USB, don't repurpose), `43, 44` (default UART TX/RX), and
`35-37` (used for PSRAM on N16R8-style boards — check your board's pinout diagram).

## Build & flash

```sh
pio run -t upload   # build and flash
pio device monitor   # serial console (also over the native USB port)
```

(Requires [PlatformIO](https://platformio.org/install/cli) — `pip install platformio` or the VS Code extension.)

## Customizing

- More buttons/encoders: add pins to `buttonPins`/`buttonKeys` in `src/main.cpp`, or copy
  the `encA/encB/encSw` block for a second encoder.
- Different keys: `buttonKeys[]` takes any char the Arduino `Keyboard` API accepts, or use
  `keyboard.press(KEY_...)` media/modifier constants for non-character keys.
- Encoder currently sends volume up/down + mute; change the `CONSUMER_CONTROL_*` constants
  in `pollEncoder()` for other actions (see `USBHIDConsumerControl.h`).
