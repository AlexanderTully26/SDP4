#include <Servo.h>

#define NUM_SERVOS 8

// Create an array of Servo objects
Servo servos[NUM_SERVOS];

// Define the pins for each servo
const uint8_t servoPins[NUM_SERVOS] = {2, 3, 4, 5, 6, 7, 8, 9};

// Current percentage values (0–100) for each servo
int servoPercents[NUM_SERVOS] = {0, 25, 50, 75, 100, 50, 25, 0}; // Example initial values

void setup() {
  for (int i = 0; i < NUM_SERVOS; i++) {
    servos[i].attach(servoPins[i]);
  }

  Serial.begin(9600);
  Serial.println("Servo Controller Ready. Send 8 percentages separated by commas:");
}

void loop() {
  // Example: Receiving values via Serial (optional)
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    parseAndSetPercents(input);
  }

  // Update servos based on percentage values
  for (int i = 0; i < NUM_SERVOS; i++) {
    int angle = map(servoPercents[i], 0, 100, 0, 180);
    servos[i].write(angle);
  }

  delay(50); // Slight delay to prevent jitter
}

// Parse comma-separated percentage values from Serial
void parseAndSetPercents(String input) {
  int index = 0;
  int lastComma = -1;

  for (int i = 0; i <= input.length(); i++) {
    if (input[i] == ',' || i == input.length()) {
      String valStr = input.substring(lastComma + 1, i);
      valStr.trim();
      if (index < NUM_SERVOS) {
        int percent = valStr.toInt();
        servoPercents[index] = constrain(percent, 0, 100);
        index++;
      }
      lastComma = i;
    }
  }
}
