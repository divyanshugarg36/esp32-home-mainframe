# GPIO Testing Results for ESP32

This document lists the results of testing GPIOs on the ESP32, including their usability and specific constraints. The GPIOs are categorized as follows:
- **Available GPIOs**: Usable as output or input.
- **Input-Only GPIOs**: Pins restricted to input functionality.
- **Unavailable GPIOs**: Pins that should not be used.

## Results

### Available GPIOs
The following GPIOs can be configured as **OUTPUT** or **INPUT**:
- GPIO 10
- GPIO 13
- GPIO 12
- GPIO 14
- GPIO 27
- GPIO 26
- GPIO 25
- GPIO 33
- GPIO 32
- GPIO 15
- GPIO 2
- GPIO 0 (This is a Strapping Pin)
- GPIO 4
- GPIO 16
- GPIO 17
- GPIO 5
- GPIO 18
- GPIO 19
- GPIO 21
- GPIO 3
- GPIO 1
- GPIO 22
- GPIO 23

### Input-Only GPIOs
The following GPIOs are restricted to **INPUT** functionality only:
- GPIO 35
- GPIO 34
- GPIO 39
- GPIO 36

### Unavailable GPIOs
The following GPIOs should **not be used**:
- GPIO 11
- GPIO 9
- GPIO 6
- GPIO 7
- GPIO 8

## Notes
- Ensure you do not configure "Input-Only" GPIOs as output pins to avoid unexpected behavior or hardware damage.
- Avoid using the "Unavailable GPIOs" marked with an `X` to maintain proper functionality of the ESP32.
- GPIO 10 is usable but may have specific constraints depending on the board or configuration.

For further details, refer to the ESP32 datasheet or documentation specific to your development board.

