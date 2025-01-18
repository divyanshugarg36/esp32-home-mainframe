 /**********************************************************************************
 *  TITLE: Get the Hex code from any IR remote.
 * 
 *  Download the Libraries:
 *  IRremote Library v4.4.1: https://github.com/Arduino-IRremote/Arduino-IRremote
 **********************************************************************************/

// CONNECTIONS:
// Pin S - GPIO PIN
// Middle - VCC 3.3v
// Negative - GND

#include <IRremote.hpp>
#define IR_RECEIVE_PIN 32

void setup()
{
  Serial.begin(115200); // // Establish serial communication
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK); // Start the receiver
}

void loop() {
  if (IrReceiver.decode()) {
      Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX); // Print "old" raw data
      IrReceiver.printIRResultShort(&Serial); // Print complete received data in one line
      IrReceiver.printIRSendUsage(&Serial);   // Print the statement required to send this data
      IrReceiver.resume(); // Enable receiving of the next value
  }
}
