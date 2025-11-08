/**
 * @file Constants.h
 * @brief Contains all hardware constants, pin definitions, and system-wide parameters.
 */

#pragma once
#include <Arduino.h>

// =========================
// SERVO LIMITS AND CONTROL
// =========================

constexpr int SERVO_STOP  = 92;
constexpr int SERVO_LEFT  = 84;
constexpr int SERVO_RIGHT = 100;

constexpr int MIN_ANGLE_J0 = -102;
constexpr int MAX_ANGLE_J0 = 122;

// =========================
// FEEDBACK & ANALOG CONFIG
// =========================

constexpr int   ANALOG_PIN          = 1;
constexpr int   ADC_RESOLUTION      = 4095;
constexpr float REFERENCE_VOLTAGE   = 2.500;
constexpr float VOLTAGE_TOLERANCE   = 0.05;
constexpr float MIN_VOLTAGE_J0      = 0.077;
constexpr float MAX_VOLTAGE_J0      = 2.200;

// =========================
// SERVO PINOUTS
// =========================

constexpr int servoPin0 = 21;
constexpr int servoPin1 = 47;
constexpr int servoPin2 = 48;
constexpr int servoPin3 = 35;
constexpr int servoPin4 = 36;
constexpr int servoPin5 = 37;

// =========================
// MISC CONTROL PARAMETERS
// =========================

constexpr unsigned long moveInterval = 0;
constexpr float offsetMultipliers[6] = {-1.0, 1.0, -1.0, 1.0, 1.0, 1.0};
