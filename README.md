# Electronic-Thermometer
# Electronic Thermometer (Arduino)

An analog electronic thermometer that measures temperature using the
temperature-dependent Base-Emitter voltage (VBE) of NPN transistors,
displayed on a 128x32 OLED screen.

## How it works

NPN transistors have a predictable relationship between their
Base-Emitter voltage (VBE) and temperature. By biasing two identical
transistors at different current levels and measuring the difference
in their VBE voltages (ΔVBE), temperature can be derived with good
linearity and reduced sensitivity to transistor-to-transistor variation.

The Arduino reads both VBE values via its ADC using the internal 1.1V
reference for better precision, averages 32 samples per reading to
reduce noise, then applies a linear calibration formula:

**T = slope × ΔVBE + offset**

The result is shown live on the OLED display and also printed to
Serial for debugging.

## Hardware

- Arduino (Uno or compatible)
- 2× NPN transistors (same type)
- Adafruit SSD1306 128×32 OLED display (I2C)
- Resistors for current biasing
- Connecting wires / breadboard

## Wiring

| Signal         | Arduino Pin |
|----------------|-------------|
| ADC input      | A0          |
| Low-current transistor  | D6 |
| High-current transistor | D7 |
| OLED SDA       | A4 (Uno)    |
| OLED SCL       | A5 (Uno)    |

## Calibration

The slope and offset constants in the code were determined empirically
by measuring ΔVBE at known temperatures and fitting a linear curve.
If you rebuild this circuit, you will likely need to recalibrate:

1. Measure ΔVBE at two known temperatures (e.g. ice water at 0°C and
   boiling water at 100°C)
2. Solve for slope and offset
3. Update these lines in the code:

   float slope = -95.0;
   float offset = 4.1;

```
VBE low: 0.6123 V  |  VBE high: 0.6251 V  |  dVBE: 12.8 mV  |  Temp: 23.4 C
```
