#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#if defined(ESP8266)
  #define BUTTON_A  0
  #define BUTTON_B 16
  #define BUTTON_C  2
  #define WIRE Wire
#elif defined(ARDUINO_ADAFRUIT_FEATHER_ESP32C6)
  #define BUTTON_A  7
  #define BUTTON_B  6
  #define BUTTON_C  5
  #define WIRE Wire
#elif defined(ESP32)
  #define BUTTON_A 15
  #define BUTTON_B 32
  #define BUTTON_C 14
  #define WIRE Wire
#elif defined(ARDUINO_STM32_FEATHER)
  #define BUTTON_A PA15
  #define BUTTON_B PC7
  #define BUTTON_C PC5
  #define WIRE Wire
#elif defined(TEENSYDUINO)
  #define BUTTON_A  4
  #define BUTTON_B  3
  #define BUTTON_C  8
  #define WIRE Wire
#elif defined(ARDUINO_FEATHER52832)
  #define BUTTON_A 31
  #define BUTTON_B 30
  #define BUTTON_C 27
  #define WIRE Wire
#elif defined(ARDUINO_ADAFRUIT_FEATHER_RP2040)
  #define BUTTON_A  9
  #define BUTTON_B  8
  #define BUTTON_C  7
  #define WIRE Wire
#else
  #define BUTTON_A  9
  #define BUTTON_B  6
  #define BUTTON_C  5
  #define WIRE Wire
#endif

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &WIRE);

// ------------------------ Sensor Pins ------------------------
const int sensorPin = A0;  // ADC pin connected to transistor nodes
const int lowPin  = 6;     // Digital pin to drive low-current transistor
const int highPin = 7;     // Digital pin to drive high-current transistor

// ------------------------ ADC & Calibration ------------------------
const float Vref = 1.1;    // Internal reference voltage
const float ADCmax = 1023.0;

float slope = -95.0;
float offset = 4.1;


float temperature = 0;

// ------------------------ Functions ------------------------
void setup() {
  Serial.begin(9600);
  while (!Serial) delay(10); 

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("OLED NOT FOUND! Check your wiring/soldering."));
    for(;;); 
  }
  
  Serial.println(F("OLED Found and Initialized"));
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE); 
  display.display();

  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);

  pinMode(lowPin, OUTPUT);
  pinMode(highPin, OUTPUT);
  digitalWrite(lowPin, LOW);
  digitalWrite(highPin, LOW);

  analogReference(INTERNAL); // Use 1.1V internal ref for better precision
}

// Read VBE voltage of one transistor
float readVBE(int pin) {
  digitalWrite(pin, HIGH);   // Turn transistor on
  delay(2);                  // Let voltage stabilize

  long sum = 0;
  for (int i = 0; i < 32; i++) {
    sum += analogRead(sensorPin);
    delay(1);  // small delay to reduce noise
  }
  digitalWrite(pin, LOW);    // Turn transistor off
  float VBE = (sum / 32.0) * Vref / ADCmax;
  return VBE;
}

void loop() {
  // -------------------- Measure ΔVBE --------------------
  float VBE_low  = readVBE(lowPin);
  float VBE_high = readVBE(highPin);

  float dVBE = VBE_high - VBE_low;

  // Convert ΔVBE to temperature using calibration
  temperature = slope * dVBE + offset;

  // -------------------- OLED Display --------------------
  display.clearDisplay();      
  display.setCursor(0, 0);
  display.setTextSize(1);      
  display.println("Temperature:");

  display.setCursor(0, 15);
  display.setTextSize(2);      
  display.print(temperature, 1);
  display.print(" C");
  
  display.display(); // Push buffer to screen

  // -------------------- Serial Debug --------------------
  Serial.print("VBE low: ");
  Serial.print(VBE_low, 4);
  Serial.print(" V  |  VBE high: ");
  Serial.print(VBE_high, 4);
  Serial.print(" V  |  dVBE: ");
  Serial.print(dVBE * 1000, 1); // show mV
  Serial.print(" mV  |  Temp: ");
  Serial.print(temperature, 1);
  Serial.println(" C");

  delay(1000);
}
