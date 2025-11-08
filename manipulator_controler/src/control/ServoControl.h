/**
 * @file ServoControl.h
 * @brief Defines servo control logic, smooth motion, and feedback loop.
 */

#pragma once
#include <Arduino.h>
#include <ESP32Servo.h>
#include "../config/Constants.h"
#include "../config/ServoPositions.h"

extern Servo servo0, servo1, servo2, servo3, servo4, servo5;
extern float currentAngles[6];
extern float targetAngles[6];

void initServos();
void updateJoint0_feedback();
void updateStandardServosSmooth();
void applyStandardServoPositions();

float getCurrentVoltage_J0();
float angleToVoltage_J0(float angle);
