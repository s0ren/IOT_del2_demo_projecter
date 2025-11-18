#include <Arduino.h>

const int BuildinledPin = 2; // GPIO2 is often connected to the onboard LED on ESP32 boards
const int ButtonPin = 32;    // GPIO0 is often used for a button on ESP32 boards
const int LedPin = 4; // GPIO33 for an external LED

void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  pinMode(LedPin, OUTPUT);
  digitalWrite(LedPin, HIGH); // Ensure LED is off at start
  delay(1000); // Wait for a second

  pinMode(ButtonPin, INPUT);


}

void loop() {
  // put your main code here, to run repeatedly:
  int buttonState = digitalRead(ButtonPin);
  Serial.println("buttonState: " + String(buttonState));
  if (buttonState == HIGH) {
    digitalWrite(LedPin, HIGH); // Turn the LED on
    Serial.println("LED is ON");
  } else {
    digitalWrite(LedPin, LOW); // Turn the LED off
    Serial.println("LED is OFF");
  }
}

