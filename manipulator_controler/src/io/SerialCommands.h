#pragma once
#include <Arduino.h>

/**
 * @brief Reads and processes commands from the serial port.
 * 
 * Supports:
 * - Predefined poses: default, pos1, pos2, pos3, zero_pos
 * - Offset commands: "30 0 -10 0 0 0"
 */
void processSerialInput();
