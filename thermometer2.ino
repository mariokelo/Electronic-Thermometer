#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ------------------------ OLED ------------------------
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

// ------------------------ Sensor Pin ------------------------
const int sensorPin = A0;

// ------------------------ ADC Reference ------------------------
const float Vref   = 1.1;
const float ADCmax = 1023.0;

// ------------------------ Calibration ------------------------
const float VBE_CAL  = 0.6471; // VBE at 0°C (your measured value)
const float TEMP_CAL = 0.0;
const float SLOPE    = -0.002; // V/°C

float calOffset = 0.0;

// ------------------------ Averaging ------------------------
const int NUM_SAMPLES  = 64;
const int SAMPLE_DELAY = 1;

// ------------------------ Logging interval ------------------------
const unsigned long LOG_INTERVAL = 2UL * 60UL * 1000UL; // 10 minutes in ms
unsigned long lastLogTime = 0;
unsigned long startTime   = 0;

// ------------------------ Setup ------------------------
void setup() {
  Serial.begin(9600);
  unsigned long t = millis();
  while (!Serial && millis() - t < 3000);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED not found!"));
    for (;;);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  analogReference(INTERNAL);
  analogRead(sensorPin); // discard first reading
  delay(10);

  startTime   = millis();
  lastLogTime = millis() - LOG_INTERVAL; // log immediately on first loop

  // CSV header for Python to parse
  Serial.println(F("millis,minutes,temperature_C"));

  Serial.println(F("Commands: 'u' +0.5C  |  'd' -0.5C  |  'r' reset"));
}

// ------------------------ Read VBE ------------------------
float readVBE() {
  long sum = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    sum += analogRead(sensorPin);
    delay(SAMPLE_DELAY);
  }
  return (sum / (float)NUM_SAMPLES) * Vref / ADCmax;
}

// ------------------------ Main Loop ------------------------
void loop() {
  // Handle serial calibration commands
  if (Serial.available()) {
    char cmd = Serial.read();
    if      (cmd == 'u') { calOffset += 0.5; Serial.print(F("# calOffset = ")); Serial.println(calOffset, 1); }
    else if (cmd == 'd') { calOffset -= 0.5; Serial.print(F("# calOffset = ")); Serial.println(calOffset, 1); }
    else if (cmd == 'r') { calOffset  = 0.0; Serial.println(F("# calOffset reset")); }
  }

  float VBE         = readVBE();
  float temperature = ((VBE - VBE_CAL) / SLOPE) + TEMP_CAL + calOffset;
  unsigned long now = millis();

  // Log every 10 minutes as CSV (lines starting with # are comments, ignored by Python)
  if (now - lastLogTime >= LOG_INTERVAL) {
    lastLogTime = now;
    float minutes = (now - startTime) / 60000.0;
    Serial.print(now);
    Serial.print(F(","));
    Serial.print(minutes, 1);
    Serial.print(F(","));
    Serial.println(temperature, 2);
  }

  // OLED — show temp + elapsed time
  display.clearDisplay();

  display.setCursor(0, 0);
  display.setTextSize(1);
  unsigned long elapsed = (now - startTime) / 60000UL;
  display.print(F("t+"));
  display.print(elapsed);
  display.println(F(" min"));

  display.setCursor(0, 15);
  display.setTextSize(2);
  display.print(temperature, 1);
  display.print(F(" C"));

  display.display();

  delay(1000);
}
