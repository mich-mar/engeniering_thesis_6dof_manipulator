/**
 * @file main.cpp
 * @brief Advanced hybrid control system for a 6-axis robot arm on ESP32-S3.
 * @version 2.3
 * * This program implements:
 * - Closed-loop control for one joint (joint_0) using a potentiometer as an encoder.
 * - Smooth, interpolated control for five standard servo motors.
 * - Receiving and parsing commands via the serial port.
 * - Predefined positions: "default", "pos1", "pos2", "pos3", "zero_pos".
 * - Relative offsets from zero_pos (e.g., "-30 40 0 0 10 0").
 * - Clamping for joint_0 based on new empirical calibration.
 */

#include <Arduino.h>
#include <ESP32Servo.h>

// =================================================================
// ==              ROBOT PREDEFINED POSITIONS                   ==
// =================================================================

const float defaultAngles[6] = {0.0, 150.0, 240.0, 80.0, 180.0, 90.0};
const float pos1Angles[6] = {0.0, 120.0, 150.0, 90.0, 90.0, 180.0}; 
const float pos2Angles[6] = {90.0, 150.0, 220.0, 90.0, 180.0, 90.0};
const float pos3Angles[6] = {0.0, 200.0, 50.0, 180.0, 0.0, 90.0}; 
const float zero_pos[6] = {0.0, 130.0, 130.0, 80.0, 80.0, 90.0};


// =================================================================
// ==   JOINT_0 CONFIGURATION (FEEDBACK SERVO)                  ==
// =================================================================

const int ANALOG_PIN = 1;      // Potentiometer feedback pin.

// Control parameters for joint_0 (continuous/modified servo).
const int SERVO_STOP = 92;     // PWM value to stop the servo.
const int SERVO_LEFT = 84;     // PWM value for counter-clockwise motion.
const int SERVO_RIGHT = 100;   // PWM value for clockwise motion.

// *** NOWA KALIBRACJA (oparta na logach z 27.10.2025) ***
// Kąty obliczone na podstawie fizycznych granic napięcia.
const int MIN_ANGLE_J0 = -102;  // Kąt odpowiadający 0.077V
const int MAX_ANGLE_J0 = 122;   // Kąt odpowiadający 2.200V
// Zmierzone fizyczne granice napięcia.
const float MIN_VOLTAGE_J0 = 0.077; // Zmierzone minimum
const float MAX_VOLTAGE_J0 = 2.200; // Zmierzone maksimum

// Analog to Digital Converter (ADC) parameters.
const int ADC_RESOLUTION = 4095;    // 12-bit ADC resolution (0-4095).
// Napięcie odniesienia dla ADC_11db (ok. 2.5V). Jest POPRAWNE.
const float REFERENCE_VOLTAGE = 2.500;
const float VOLTAGE_TOLERANCE = 0.05; // Dead zone (in Volts) to prevent oscillation.

// =================================================================
// ==            GENERAL MANIPULATOR CONFIGURATION            ==
// =================================================================

const int servoPin0 = 21;
const int servoPin1 = 47; 
const int servoPin2 = 48; 
const int servoPin3 = 35;
const int servoPin4 = 36;
const int servoPin5 = 37;

Servo servo0, servo1, servo2, servo3, servo4, servo5;

// Arrays for storing the manipulator's state.
float currentAngles[6] = {0.0, 150.0, 240.0, 80.0, 180.0, 90.0}; 
float targetAngles[6]  = {0.0, 150.0, 240.0, 80.0, 180.0, 90.0};

// Serial communication variables.
String receivedLine = "";
boolean lineComplete = false;

// Smooth motion (interpolation) variables.
unsigned long lastMoveTime = 0;
const unsigned long moveInterval = 10; // Milliseconds between interpolation steps.

// Timer do debugowania napięcia
unsigned long lastVoltagePrintTime = 0;
const unsigned long voltagePrintInterval = 250;

// =================================================================
// ==               HELPER AND CONTROL FUNCTIONS              ==
// =================================================================

/**
 * @brief Reads the current voltage from the joint_0 potentiometer.
 * @return Voltage in Volts.
 */
float getCurrentVoltage_J0() {
  return ((float)analogRead(ANALOG_PIN) / ADC_RESOLUTION) * REFERENCE_VOLTAGE;
}

/**
 * @brief Converts a target angle (degrees) to its corresponding voltage.
 * @param angle The target angle for joint_0.
 * @return The calculated target voltage.
 */
float angleToVoltage_J0(float angle) {
  // map() używa teraz nowych, precyzyjnych granic
  long mappedValue = map(angle, MIN_ANGLE_J0, MAX_ANGLE_J0, (long)(MIN_VOLTAGE_J0 * 1000), (long)(MAX_VOLTAGE_J0 * 1000));
  return (float)mappedValue / 1000.0;
}

/**
 * @brief Control loop for joint_0. Compares current and target voltage to minimize the error.
 */
void updateJoint0_feedback() {
  // Cel (targetAngles[0]) jest teraz ograniczony (constrained) w pętli loop()
  float targetVoltage = angleToVoltage_J0(targetAngles[0]);
  float currentVoltage = getCurrentVoltage_J0();
  float error = targetVoltage - currentVoltage;

  if (abs(error) <= VOLTAGE_TOLERANCE) {
    servo0.write(SERVO_STOP);
  } else if (error > 0) {
    servo0.write(SERVO_LEFT);
  } else {
    servo0.write(SERVO_RIGHT);
  }
}

/**
 * @brief Applies the current angle values to the standard servos (1-5).
 */
void applyStandardServoPositions() {
  servo3.write(int(constrain(currentAngles[3], 0.0, 180.0)));
  servo4.write(int(constrain(currentAngles[4], 0.0, 180.0)));
  servo5.write(int(constrain(currentAngles[5], 0.0, 180.0)));

  // Servos 1 & 2 have a 270° range, map it to the library's 0-180° standard.
  servo1.write(int(constrain(currentAngles[1], 0.0, 270.0) * 180.0 / 270.0));
  servo2.write(int(constrain(currentAngles[2], 0.0, 270.0) * 180.0 / 270.0));
}

/**
 * @brief Handles smooth interpolation for standard servos (1-5).
 */
void updateStandardServosSmooth() {
  if (millis() - lastMoveTime >= moveInterval) {
    lastMoveTime = millis();
    bool moved = false;
    const float step = 0.1; // Krok interpolacji

    for (int i = 1; i < 6; i++) {
      float diff = targetAngles[i] - currentAngles[i];

      // Poprawka logiki: Zapewnia, że cel zostanie dokładnie osiągnięty
      if (fabs(diff) < step) {
        if (currentAngles[i] != targetAngles[i]) {
          currentAngles[i] = targetAngles[i];
          moved = true;
        }
      } else if (diff != 0) {
        moved = true;
        if (diff > 0) currentAngles[i] += step;
        else currentAngles[i] -= step;
      }
    }
    if (moved) applyStandardServoPositions();
  }
}

// =================================================================
// ==                      MAIN PROGRAM FUNCTIONS                   ==
// =================================================================

/**
 * @brief Setup function, runs once after microcontroller starts.
 */
void setup() {
  Serial.begin(115200);
  delay(100);

  // Configure analog input for joint_0.
  analogReadResolution(12);
  analogSetPinAttenuation(ANALOG_PIN, ADC_11db);
  
  Serial.println("HYBRID SERVO CONTROL READY (v2.3 - CALIBRATED)");

  // Allocate PWM timers for the ESP32Servo library.
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  // Attach servo objects to physical GPIO pins.
  servo0.attach(servoPin0);
  servo1.attach(servoPin1);
  servo2.attach(servoPin2);
  servo3.attach(servoPin3);
  servo4.attach(servoPin4);
  servo5.attach(servoPin5);

  // Set servos to their initial positions.
  applyStandardServoPositions();
  servo0.write(SERVO_STOP);
}

/**
 * @brief Main program loop, runs indefinitely.
 */
void loop() {
  // Step 1: Receive data from the serial port.
  while (Serial.available() > 0) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      lineComplete = true;
    } else if (inChar != '\r') {
      receivedLine += inChar;
    }
  }

  // Step 2: Process the received command.
  if (lineComplete) {
    receivedLine.trim();

    // --- OBSŁUGA KOMEND TEKSTOWYCH ---
    if (receivedLine.equalsIgnoreCase("default")) {
      Serial.println("COMMAND: Setting 'default' position.");
      for (int i = 0; i < 6; i++) {
        targetAngles[i] = defaultAngles[i];
      }
    } 
    else if (receivedLine.equalsIgnoreCase("pos1")) {
      Serial.println("COMMAND: Setting 'pos1' position.");
      for (int i = 0; i < 6; i++) {
        targetAngles[i] = pos1Angles[i];
      }
    }
    else if (receivedLine.equalsIgnoreCase("pos2")) {
      Serial.println("COMMAND: Setting 'pos2' position.");
      for (int i = 0; i < 6; i++) {
        targetAngles[i] = pos2Angles[i];
      }
    }
    else if (receivedLine.equalsIgnoreCase("pos3")) {
      Serial.println("COMMAND: Setting 'pos3' position.");
      for (int i = 0; i < 6; i++) {
        targetAngles[i] = pos3Angles[i];
      }
    }
    else if (receivedLine.equalsIgnoreCase("zero_pos")) {
      Serial.println("COMMAND: Setting 'zero_pos' position.");
      for (int i = 0; i < 6; i++) {
        targetAngles[i] = zero_pos[i];
      }
    }
    // --- OBSŁUGA KOMEND Z ODCHYŁKAMI ---
    else {
      float offsets[6];
      int valueIndex = 0;
      
      char lineBuffer[100]; 
      receivedLine.toCharArray(lineBuffer, sizeof(lineBuffer));

      // Dzielimy tekst po spacji LUB średniku
      char* part = strtok(lineBuffer, " ;");
      while(part != NULL && valueIndex < 6) {
        offsets[valueIndex++] = atof(part);
        part = strtok(NULL, " ;");
      }

      if (valueIndex == 6) {
        Serial.print("OFFSETS: ");
        for(int i=0; i<6; i++) Serial.printf("%.1f ", offsets[i]);
        Serial.println();

        // Obliczamy nowy cel jako: pozycja_zerowa + odchyłka
        for (int i = 0; i < 6; i++) {
          targetAngles[i] = zero_pos[i] + offsets[i];
        }
        
        // *** ZMIANA: Ponownie włączamy ograniczanie (constrain) ***
        // Używa teraz nowych, precyzyjnych granic: -102 do 122
        targetAngles[0] = constrain(targetAngles[0], MIN_ANGLE_J0, MAX_ANGLE_J0);
        
        Serial.printf("NEW TARGETS (J0 constrained): %.1f %.1f %.1f %.1f %.1f %.1f\n",
                      targetAngles[0], targetAngles[1], targetAngles[2],
                      targetAngles[3], targetAngles[4], targetAngles[5]);
      } else {
        Serial.println("ERROR: Invalid command. Expected 6 offset numbers (e.g. '30 0 -10 ...') or a keyword ('default', 'pos1', etc.).");
      }
    }

    // Reset the buffer for the next command.
    receivedLine = "";
    lineComplete = false;
  }

  // Step 3: Update all servo positions in each iteration.
  updateJoint0_feedback();
  updateStandardServosSmooth();

  // Step 4: Report current voltage for debugging
  // if (millis() - lastVoltagePrintTime >= voltagePrintInterval) {
  //   lastVoltagePrintTime = millis();
    
  //   float currentVoltage = getCurrentVoltage_J0(); 
  //   Serial.printf("DEBUG J0 Voltage: %.3f V\n", currentVoltage);
  // }
}
