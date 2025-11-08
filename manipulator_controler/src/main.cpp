#include <Arduino.h>
#include "config/Constants.h"
#include "control/ServoControl.h"
#include "io/SerialCommands.h"

/**
 * @brief System initialization and main control loop.
 */
void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  analogSetPinAttenuation(ANALOG_PIN, ADC_11db);
  Serial.println("HYBRID SERVO CONTROL READY (Modular v3.0)");
  initServos();
}

/**
 * @brief Main control loop — handles serial commands and servo updates.
 */
void loop() {
  processSerialInput();
  updateJoint0_feedback();
  updateStandardServosSmooth();
}
