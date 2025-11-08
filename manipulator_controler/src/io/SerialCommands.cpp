#include "SerialCommands.h"
#include "../config/Constants.h"
#include "../config/ServoPositions.h"
#include "../control/ServoControl.h"

String receivedLine = "";
bool lineComplete = false;

static void printTargetAngles() {
  Serial.print("NEW TARGETS: ");
  for (int i = 0; i < 6; i++) {
    Serial.print(targetAngles[i]);
    Serial.print(" ");
  }
  Serial.println();
}

/**
 * @brief Handles parsing and executing serial commands from UART.
 */
void processSerialInput() {
  while (Serial.available() > 0) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') lineComplete = true;
    else if (inChar != '\r') receivedLine += inChar;
  }

  if (!lineComplete) return;
  receivedLine.trim();

  if (receivedLine.equalsIgnoreCase("default")) {
    Serial.println("COMMAND: Setting 'default' position.");
    for (int i = 0; i < 6; i++) targetAngles[i] = defaultAngles[i];
    printTargetAngles();
  } 
  else if (receivedLine.equalsIgnoreCase("pos1")) {
    Serial.println("COMMAND: Setting 'pos1' position.");
    for (int i = 0; i < 6; i++) targetAngles[i] = pos1Angles[i];
     printTargetAngles();
  }
  else if (receivedLine.equalsIgnoreCase("pos2")) {
    Serial.println("COMMAND: Setting 'pos2' position.");
    for (int i = 0; i < 6; i++) targetAngles[i] = pos2Angles[i];
     printTargetAngles();
  }
  else if (receivedLine.equalsIgnoreCase("pos3")) {
    Serial.println("COMMAND: Setting 'pos3' position.");
    for (int i = 0; i < 6; i++) targetAngles[i] = pos3Angles[i];
     printTargetAngles();
  }
  else if (receivedLine.equalsIgnoreCase("zero_pos")) {
    Serial.println("COMMAND: Setting 'zero_pos' position.");
    for (int i = 0; i < 6; i++) targetAngles[i] = zero_pos[i];
     printTargetAngles();
  }
  else {
    // Handle custom offset commands like "30 0 -10 0 0 0"
    float offsets[6];
    int count = 0;
    char buf[100];
    receivedLine.toCharArray(buf, sizeof(buf));

    char* part = strtok(buf, " ;");
    while (part != NULL && count < 6) {
      offsets[count++] = atof(part);
      part = strtok(NULL, " ;");
    }

    if (count == 6) {
      Serial.print("OFFSETS: ");
      for (int i = 0; i < 6; i++) Serial.printf("%.1f ", offsets[i]);
      Serial.println();

      for (int i = 0; i < 6; i++)
        targetAngles[i] = zero_pos[i] + (offsets[i] * offsetMultipliers[i]);

      targetAngles[0] = constrain(targetAngles[0], MIN_ANGLE_J0, MAX_ANGLE_J0);

      printTargetAngles();
    } else {
      Serial.println("ERROR: Invalid command. Expected 6 offsets or a keyword.");
    }
  }

  receivedLine = "";
  lineComplete = false;
}
