# Electronic-Thermometer

An analog electronic thermometer that measures temperature using the
temperature-dependent Base-Emitter voltage (VBE) of NPN transistors,
displayed on a 128x32 OLED screen.

## How it works

NPN transistors have a predictable relationship between their
Base-Emitter voltage (VBE) and temperature. By measuring their VBE at
a known temperature for calibration, we can code a function to calculate
the temperature at any given point by measuring the voltage changes. 

The Arduino reads both VBE values via its ADC using the internal 1.1V
reference for better precision, averages 32 samples per reading to
reduce noise.

The result is shown live on the OLED display and also printed to
Serial for debugging.

Apart from that, the project includes a Pyhton code that registers temperature readings
from the sensor every 10 minutes and then automatically builds a Temperature/Time 
graph for better visualisation of how the temperature has changed over time. 

## Hardware

- Arduino (Uno or compatible)
- 1× NPN transistors (2N3904)
- Adafruit SSD1306 128×32 OLED display (I2C)
- 10kOhm Resistor
- Connecting wires / breadboard

## Wiring

| Signal         | Arduino Pin |
|----------------|-------------|
| ADC input      | A0          |
| Transistor     | 3.3V        |
| OLED SDA       | A4 (Uno)    |
| OLED SCL       | A5 (Uno)    |

## Calibration

The calibration of the transistor has been made by measuring ΔVBE at 
a known temperature (ice water at 0°C)

```
VBE low: 0.6471 V  |   Temp: ~0 C
```
