# ESP32-Based Temperature and Touch Control System

This project demonstrates the implementation of a temperature monitoring and touch-based control system using an ESP32, an induction touch chip, a 7-segment display, and a buzzer.

---

## Features

1. **Temperature Monitoring:**
   - Uses an NTC thermistor (100k ohms) to measure temperature.
   - Converts raw ADC readings to Celsius using the Beta equation.
   - Includes noise reduction in temperature readings.
   - Displays the current temperature on a 7-segment display.

2. **Induction Touch Controls:**
   - Features a capacitive touch sensing chip (**BS8112A-3** or similar) for reliable touch detection.
   - Supports multiple touch buttons with distinct touch values.
   - Detects **short presses** and **long presses**:
     - **Short Press:** Quick input for changing temperature or stirring speed.
     - **Long Press:** Special commands like enabling Wi-Fi or dispensing.

3. **Buzzer Feedback:**
   - Alerts the user with different tones based on the input type.

4. **7-Segment Display:**
   - Displays the current temperature, stirring speed, or other parameters based on user interaction.
   - Uses the Adafruit HT16K33-based 7-segment display library.

5. **Adjustable Touch Sensitivity:**
   - Sensitivity levels for touch inputs can be calibrated programmatically.

---

## Induction Touch Chip: **BS8112A-3**

### Description:
The **BS8112A-3** is an induction-based touch-sensing chip used for capacitive touch inputs. It provides reliable and accurate touch sensing for multiple buttons, making it ideal for applications requiring user interaction.

### Key Functionalities:
1. **Multi-Key Support:**
   - The chip supports up to 12 touch buttons.
   - Each button has a unique **register address** for detection.

2. **Sensitivity Calibration:**
   - Sensitivity can be adjusted programmatically to ensure optimal touch detection in various environments.

3. **Communication:**
   - The chip communicates with the ESP32 over I²C, enabling fast and reliable data transfer.

4. **Touch Detection:**
   - Detects both **short presses** and **long presses**.
   - Long press functionality is used for special commands like enabling Wi-Fi or increasing touch sensitivity.

5. **Key Status Register:**
   - Reads touch status using registers `0x08` and `0x09`.
   - Combines register data to detect which button is pressed.

6. **Power Efficiency:**
   - Designed for low-power consumption, making it suitable for battery-operated devices.

### How It Works:
- The ESP32 communicates with the **BS8112A-3** chip using the I²C protocol.
- Touch detection is performed by reading the status registers of the chip.
- A short or long press is determined by measuring the duration of the touch event.

### Example Touch Values:
- **Temperature Up:** 4
- **Temperature Down:** 2
- **Stirring Up:** 512
- **Stirring Down:** 256
- **Long Press Commands:** Enable Wi-Fi, adjust sensitivity, or start/stop dispensing.

---

## Hardware Requirements

- **ESP32 Dev Board**
- **BS8112A-3 Induction Touch Chip**
- **NTC Thermistor (100k ohms at 25°C)**
- **100k Resistor** (for the voltage divider circuit)
- **Adafruit 7-Segment Display Backpack (HT16K33)**
- **Buzzer**
- **Touch Sensor Buttons**
- **Jumper Wires**
- **Power Source (e.g., USB or external supply)**

---

## Software Requirements

- **Arduino IDE** (version 1.8 or higher)
- **Adafruit GFX Library**
- **Adafruit LED Backpack Library**

### Library Links:
- [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)
- [Adafruit LED Backpack Library](https://github.com/adafruit/Adafruit_LED_Backpack)

To install the libraries:
1. Open Arduino IDE.
2. Go to `Sketch` > `Include Library` > `Manage Libraries...`.
3. Search for and install:
   - **Adafruit GFX Library**
   - **Adafruit LED Backpack Library**

---

## How Temperature is Calculated

The temperature is calculated using the **Beta Equation** for NTC thermistors. Here’s the mathematical breakdown:

### Voltage Divider Circuit
The thermistor is part of a voltage divider circuit:
\[
V_{out} = V_{CC} \times \frac{R_{NTC}}{R_{NTC} + R_{SERIES}}
\]
Where:
- \( V_{out} \): ADC value read by the ESP32.
- \( R_{NTC} \): Resistance of the thermistor.
- \( R_{SERIES} \): 100k ohm resistor in the circuit.

Rearranging to solve for \( R_{NTC} \):
\[
R_{NTC} = R_{SERIES} \times \left( \frac{ADC_{MAX}}{ADC_{READ}} - 1 \right)
\]
Where:
- \( ADC_{MAX} \): Maximum ADC value (4095 for 12-bit ADC).
- \( ADC_{READ} \): ADC reading from the ESP32.

### Beta Equation for Temperature
The thermistor resistance is then converted to temperature using the Beta equation:
\[
T_{K} = \frac{1}{\frac{1}{T_{25}} + \frac{\ln{\left(\frac{R_{NTC}}{R_{25}}\right)}}{\beta}}
\]
Where:
- \( T_{K} \): Temperature in Kelvin.
- \( T_{25} \): 298.15K (25°C in Kelvin).
- \( R_{25} \): 100k ohms (thermistor resistance at 25°C).
- \( \beta \): Beta coefficient (3950 for this thermistor).

Finally, convert Kelvin to Celsius:
\[
T_{C} = T_{K} - 273.15
\]

---

## Noise Reduction in Temperature Readings

1. **Temperature Buffering:**
   - A minimum threshold (`temperatureThreshold`) is applied. Temperature updates only when the change exceeds this threshold.
   - Example: If the threshold is set to 1°C, only changes greater than 1°C are considered.

2. **Averaging (optional):**
   - If needed, averaging can be implemented by maintaining a rolling buffer of recent temperature values and averaging them.

---

## Code Overview

### 1. **Setup (`setup()`):**
   - Initializes the 7-segment display, buzzer, and touch sensitivity.
   - Sets ADC resolution to 12-bit for precise temperature readings.
   - Displays an initial temperature value on the 7-segment display.

### 2. **Main Loop (`loop()`):**
   - Reads touch inputs using the `processTouch()` function.
   - Updates the temperature readings from the NTC thermistor.
   - Provides feedback for touch events:
     - **Short Press:** Logs the touch value and triggers a short buzzer.
     - **Long Press:** Logs the touch value and triggers a long buzzer.
   - Displays the current temperature on the 7-segment display.

---

## Future Enhancements

1. Add Fahrenheit conversion and a toggle feature for Celsius/Fahrenheit.
2. Introduce Wi-Fi connectivity for remote monitoring and logging.
3. Expand touch controls for additional features (e.g., start/stop actions).
4. Implement real-time data visualization via BLE or MQTT.

---

## License

This project is open-source and available under the [MIT License](LICENSE).

---

## Contributions

Contributions are welcome! Create a pull request or open an issue for suggestions or bug reports.

---

## Author

Developed by sundeep goud malkapuram
