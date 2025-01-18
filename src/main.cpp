#include <Arduino.h>

bool isOn = HIGH;
int INPUT_PIN = 21;
int OUTPUT_PIN = 19;

void setup()
{
  Serial.begin(115200);

  // // pinMode(11, OUTPUT); X

  // pinMode(10, OUTPUT);
  
  // // pinMode(9, OUTPUT); X
  
  // pinMode(13, OUTPUT);
  // pinMode(12, OUTPUT);
  // pinMode(14, OUTPUT);
  // pinMode(27, OUTPUT);
  // pinMode(26, OUTPUT);
  // pinMode(25, OUTPUT);
  // pinMode(33, OUTPUT);
  // pinMode(32, OUTPUT);

  // pinMode(35, INPUT); // I-Only
  // pinMode(34, INPUT); // I-Only
  // pinMode(39, INPUT); // I-Only
  // pinMode(36, INPUT); // I-Only

  // // pinMode(6, OUTPUT); X
  // // pinMode(7, OUTPUT); X
  // // pinMode(8, OUTPUT); X
  
  // pinMode(15, OUTPUT);
  // pinMode(2, OUTPUT);
  // pinMode(0, OUTPUT);
  // pinMode(4, OUTPUT);
  // pinMode(16, OUTPUT);
  // pinMode(17, OUTPUT);
  // pinMode(5, OUTPUT);
  // pinMode(18, OUTPUT);
  // pinMode(19, OUTPUT);
  // pinMode(21, OUTPUT);
  // pinMode(3, OUTPUT);
  // pinMode(1, OUTPUT);
  // pinMode(22, OUTPUT);
  // pinMode(23, OUTPUT);

  pinMode(OUTPUT_PIN, OUTPUT);
  pinMode(INPUT_PIN, INPUT_PULLUP);
  digitalWrite(OUTPUT_PIN, isOn);
}

void loop()
{
  delay(500);
  if (digitalRead(INPUT_PIN) == 0) {
    if(isOn) {
      isOn = LOW;
    } else {
      isOn = HIGH;
    }
    digitalWrite(OUTPUT_PIN, isOn);
    delay(1000);
  }

  // // digitalWrite(11, HIGH); X
  
  // digitalWrite(10, HIGH);
  
  // // digitalWrite(9, HIGH); X
  
  // digitalWrite(13, HIGH);
  // digitalWrite(12, HIGH);
  // digitalWrite(14, HIGH);
  // digitalWrite(27, HIGH);
  // digitalWrite(26, HIGH);
  // digitalWrite(25, HIGH);
  // digitalWrite(33, HIGH);
  // digitalWrite(32, HIGH);

  Serial.println("35-"+String(digitalRead(35))); // I-Only
  Serial.println("34-"+String(digitalRead(34))); // I-Only
  Serial.println("39-"+String(digitalRead(39))); // I-Only
  Serial.println("36-"+String(digitalRead(36))); // I-Only

  // // digitalWrite(6, HIGH); X
  // // digitalWrite(7, HIGH); X
  // // digitalWrite(8, HIGH); X
  
  // digitalWrite(15, HIGH);
  // digitalWrite(2, HIGH);
  // digitalWrite(0, HIGH);
  // digitalWrite(4, HIGH);
  // digitalWrite(16, HIGH);
  // digitalWrite(17, HIGH);
  // digitalWrite(5, HIGH);
  // digitalWrite(18, HIGH);
  // digitalWrite(19, HIGH);
  // digitalWrite(21, HIGH);
  // digitalWrite(3, HIGH);
  // digitalWrite(1, HIGH);
  // digitalWrite(22, HIGH);
  // digitalWrite(23, HIGH);

  // delay(500);

  // Serial.println("ON");
}