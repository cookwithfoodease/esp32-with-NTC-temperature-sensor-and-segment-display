#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>
#include <Arduino.h>

// Define constants and variables
#define BUZZER_PIN 23  // Buzzer pin
#define NTC_PIN 34

// Constants for the NTC thermistor
#define SERIES_RESISTOR 100000  // 100k resistor in series with the NTC
#define NOMINAL_RESISTANCE 100000  // Resistance of the NTC at 25°C
#define NOMINAL_TEMPERATURE 25     // Nominal temperature in Celsius
#define BETA_COEFFICIENT 3950      // Beta coefficient of the thermistor
#define ADC_RESOLUTION 4095        // 12-bit ADC resolution
#define VCC 3.3                    // Voltage supplied to the NTC circuit

Adafruit_7segment display = Adafruit_7segment(); // 7-segment display instance

bool userInputEnabled = true;  // Flag to enable/disable user inputs
uint16_t lastKeyStatus = 0;  // Store the last key status
unsigned long buttonPressStart = 0;  // Time when button press started
const unsigned long longPressThreshold = 2000;  // Long press threshold in milliseconds
bool buttonHeld = false;  // Tracks if the button is held

// Temperature buffering to reduce noise
int lastTemperature = -100;  // Unused initial value
const int temperatureThreshold = 1; // Minimum change in temperature to update

/**
 * @brief Reads and calculates the temperature from the NTC thermistor.
 * @return int The current temperature (integer, no decimal places).
 */
int readTemperature() {
  // Read the raw ADC value from the NTC pin
  int rawADC = analogRead(NTC_PIN);

  // Convert the ADC value to resistance
  float resistance = SERIES_RESISTOR * ((float)ADC_RESOLUTION / rawADC - 1);

  // Calculate the temperature in Kelvin using the Beta equation
  float kelvinTemperature = 1.0 / (
      (1.0 / (NOMINAL_TEMPERATURE + 273.15)) +
      (log(resistance / NOMINAL_RESISTANCE) / BETA_COEFFICIENT));

  // Convert Kelvin to Celsius
  int currentTemperature = (int)(kelvinTemperature - 273.15);

  // Check if the temperature change is significant enough
  if (lastTemperature == -100 || abs(currentTemperature - lastTemperature) >= temperatureThreshold) {
    lastTemperature = currentTemperature;  // Update the last known temperature
    return currentTemperature;
  }

  // Return the last temperature if the change is not significant
  return lastTemperature;
}


// Buzzer function with duration
void buzzIndicator(int duration) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(duration);
    digitalWrite(BUZZER_PIN, LOW);
}

// Read which button is pressed
int readButtonPress() {
    uint16_t keyStatus = (readRegister(0x08) | (readRegister(0x09) << 8));
    if (keyStatus != 0xFFFF && keyStatus != 0) {
        return keyStatus;  // Return the detected button status
    }
    return 0;  // No button pressed
}

// Main function to process touch inputs
int processTouch() {
    if (!userInputEnabled) return 0;

    int buttonId = readButtonPress();

    if (buttonId != 0) {  // A button is pressed
        if (!buttonHeld) {  // New button press
            buttonPressStart = millis();  // Record the start time
            buttonHeld = true;  // Mark button as held
            buzzIndicator(100);  // Trigger buzzer for the initial press
            return buttonId;  // Return positive value for short press immediately
        } else {  // Button is being held
            unsigned long pressDuration = millis() - buttonPressStart;

            if (pressDuration >= longPressThreshold) {  // Long press detected
                buzzIndicator(300);  // Longer buzz for long press
                buttonPressStart = millis();  // Reset start time to avoid repeated long press events
                return -buttonId;  // Return negative value for long press
            }
        }
    } else {  // No button is pressed
        if (buttonHeld) {  // Button was released
            buttonHeld = false;  // Reset the button held state
            buttonPressStart = 0;  // Reset the press timer
        }
    }

    return 0;  // No new button press detected
}

// Write to a register (for BS8112A-3)
void writeRegister(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(0x50);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

// Read from a register (for BS8112A-3)
uint8_t readRegister(uint8_t reg) {
    Wire.beginTransmission(0x50);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom(0x50, (uint8_t)1);
    return (Wire.available() ? Wire.read() : 0xFF);
}

/**
 * @brief Updates the 7-segment display with a unit and value.
 * 
 * @param unit The character representing the unit (e.g., 'C' for Celsius, 'S' for speed, etc.).
 * @param value The integer value to display (e.g., temperature, speed, etc.).
 */
void updateSegmentDisplay(char unit, int value) {
    display.clear();                // Clear the display
    display.print(value);           // Print the integer value
    display.writeDigitAscii(0, unit); // Show the unit symbol on the display
    display.writeDisplay();         // Update the display
}


// Setup function
void setup() {
    Serial.begin(115200);
    analogReadResolution(12); // Set ADC resolution to 12-bit for ESP32
    Wire.begin();

    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);

    // Initialize the 7-segment display
    display.begin(0x70); // Default I2C address for HT16K33

    calibrateTouchSensitivity(100);  // Default sensitivity calibration
    updateSegmentDisplay('C', 150);
}

// Calibrate touch sensitivity
void calibrateTouchSensitivity(int level) {
    if (level < 8 || level > 63) return;

    for (uint8_t i = 0; i < 12; i++) {
        uint8_t regAddress = 0xB5 + i;  // Calculate register address for each key
        writeRegister(regAddress, level);
    }
}

// Loop function
void loop() {
    int result = processTouch();
    if (result > 0) {
        Serial.println("Short press detected for button: " + String(result));
    } else if (result < 0) {
        Serial.println("Long press detected for button: " + String(-result));
    }
    int currentTemperature = readTemperature();

  // Print the current temperature to the serial monitor
  Serial.print("Current Temperature: ");
  Serial.print(currentTemperature);
  Serial.println(" °C");
  updateSegmentDisplay('C', currentTemperature );




  delay(1000); // Delay 1 second between readings
}