#include "ServoControl.h"

Servo servo0, servo1, servo2, servo3, servo4, servo5;
float currentAngles[6] = {0.0, 130.0, 130.0, 80.0, 80.0, 90.0};
float targetAngles[6]  = {0.0, 130.0, 130.0, 80.0, 80.0, 90.0};

unsigned long lastMoveTime = 0;

/**
 * @brief Initialize all servo objects and timers.
 */
void initServos() {
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  servo0.attach(servoPin0);
  servo1.attach(servoPin1);
  servo2.attach(servoPin2);
  servo3.attach(servoPin3);
  servo4.attach(servoPin4);
  servo5.attach(servoPin5);

  applyStandardServoPositions();
  servo0.write(SERVO_STOP);
}

/**
 * @brief Reads the analog voltage corresponding to joint 0 feedback.
 */
float getCurrentVoltage_J0() {
  return ((float)analogRead(ANALOG_PIN) / ADC_RESOLUTION) * REFERENCE_VOLTAGE;
}

/**
 * @brief Maps desired angle to equivalent voltage.
 */
float angleToVoltage_J0(float angle) {
  long mappedValue = map(angle, MIN_ANGLE_J0, MAX_ANGLE_J0,
                         (long)(MIN_VOLTAGE_J0 * 1000),
                         (long)(MAX_VOLTAGE_J0 * 1000));
  return (float)mappedValue / 1000.0;
}

/**
 * @brief Closed-loop feedback control for joint 0.
 */
void updateJoint0_feedback() {
  float targetVoltage = angleToVoltage_J0(targetAngles[0]);
  float currentVoltage = getCurrentVoltage_J0();
  float error = targetVoltage - currentVoltage;

  if (abs(error) <= VOLTAGE_TOLERANCE)      servo0.write(SERVO_STOP);
  else if (error > 0)                       servo0.write(SERVO_LEFT);
  else                                      servo0.write(SERVO_RIGHT);
}

/**
 * @brief Writes constrained servo positions for all joints (1–5).
 */
void applyStandardServoPositions() {
  servo3.write(int(constrain(currentAngles[3], 0.0, 180.0)));
  servo4.write(int(constrain(currentAngles[4], 0.0, 180.0)));
  servo5.write(int(constrain(currentAngles[5], 0.0, 180.0)));

  servo1.write(int(constrain(currentAngles[1], 0.0, 270.0) * 180.0 / 270.0));
  servo2.write(int(constrain(currentAngles[2], 0.0, 270.0) * 180.0 / 270.0));
}

/**
 * @brief Smooth motion controller for standard servos (J1–J5).
 */
void updateStandardServosSmooth() {
  if (millis() - lastMoveTime >= moveInterval) {
    lastMoveTime = millis();
    const float step = 0.1;
    bool moved = false;

    for (int i = 1; i < 6; i++) {
      float diff = targetAngles[i] - currentAngles[i];
      if (fabs(diff) < step) {
        currentAngles[i] = targetAngles[i];
      } else {
        currentAngles[i] += (diff > 0 ? step : -step);
      }
      moved = true;
    }

    if (moved) applyStandardServoPositions();
  }
}
