#include <Arduino.h>
#include <IRremote.h>

// Define the GPIO pin connected to the IR LED
#define IR_SEND_PIN 19 // Change this to match your GPIO setup

void setup() {
  // Initialize the IR sender
  IrSender.begin(IR_SEND_PIN);
}

void loop() {
  // Send a command using the NEC protocol
  unsigned long command = 0x6897F300; // Example: NEC command for "Power" button
  IrSender.sendNEC(command, 32, 1);      // Send NEC signal (32 bits)
  Serial.println("command, HEX");
  delay(5000); // Wait 5 seconds before sending the next signal
}
