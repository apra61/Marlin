# AGENTS.md

## Cursor Cloud specific instructions

This is **Marlin 3D Printer Firmware (v1)** — embedded C/C++ firmware for AVR microcontrollers. There are no runtime services, databases, or web servers. The development workflow is compile-only (no "run" step, as firmware is flashed to physical hardware).

### Build commands

The primary build target is the RAMPS board (ATmega2560). Build from the `Marlin/` directory:

```bash
cd Marlin && make ARDUINO_INSTALL_DIR=/opt/arduino HARDWARE_MOTHERBOARD=33 ARDUINO_VERSION=105 \
  SRC="wiring.c wiring_analog.c wiring_digital.c wiring_pulse.c wiring_shift.c WInterrupts.c hooks.c"
```

- `HARDWARE_MOTHERBOARD=33` = RAMPS 1.3/1.4 (most common). Other options: `3` (RAMPS old), `34` (RAMPS 1.4 with servo), `7` (Ultimaker), `301` (Rambo).
- The extra `hooks.c` in `SRC` is required because the installed Arduino core (1.8.6) defines `yield()` there — without it, linking fails with `undefined reference to 'yield'`.
- Output: `applet/Marlin.hex` (Intel HEX firmware file).
- Clean: `make clean`

### Environment layout

- **AVR toolchain**: `avr-gcc`, `avr-g++`, `avr-objcopy`, `avr-size` (from `gcc-avr`, `binutils-avr`, `avr-libc` packages)
- **Arduino core**: `/opt/arduino/` — a compatibility directory mapping the newer Ubuntu `arduino-core-avr` package layout to the old-style flat layout the Makefile expects:
  - `/opt/arduino/hardware/arduino/cores/arduino` → `/usr/share/arduino/hardware/arduino/avr/cores/arduino`
  - `/opt/arduino/hardware/arduino/variants/mega` → `/usr/share/arduino/hardware/arduino/avr/variants/mega`
  - `/opt/arduino/libraries/SPI` → SPI library `src/`
  - `/opt/arduino/libraries/Wire` → Wire library `src/`
  - `/opt/arduino/libraries/LiquidCrystal` → downloaded from `arduino-libraries/LiquidCrystal`

### Known limitations

- **No automated tests**: This firmware has no test suite. Verification = successful compilation.
- **No linter**: No linting tools configured in the repo.
- **Non-arduino board variants** (Gen7, Sanguinololu): The `ArduinoAddons/` directory structure doesn't fully match what the Makefile expects for `Arduino_1.x.x`. The Gen7 variant only works with `ARDUINO_VERSION < 100`. Sanguino boards have a path mismatch (`hardware/Sanguino/` vs expected `Sanguino/`). These are pre-existing repo issues.
- **No upload possible**: `avrdude` can compile-verify only; no physical hardware to flash to.

### Configuration

Edit `Marlin/Configuration.h` and `Marlin/Configuration_adv.h` for printer-specific settings. Example configurations for specific printers are in `Marlin/example_configurations/`.
