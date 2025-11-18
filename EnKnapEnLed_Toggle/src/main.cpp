#include <Arduino.h>

const int BuildinledPin = 2; // GPIO2 is often connected to the onboard LED on ESP32 boards
const int ButtonPin = 32;    // GPIO0 is often used for a button on ESP32 boards
const int LedPin = 4; // GPIO33 for an external LED

int ledState = LOW; // LED is off when pin is HIGH
int lastButtonState = LOW; // Previous state of the button
int debounceDelay = 50; // Debounce time in milliseconds
int lastTime = 0; // Last time the button state changed

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
  // Serial.println("buttonState: " + String(buttonState));
  Serial.println(String(lastTime) + ' ' + String(millis()));
  if (buttonState == HIGH && lastButtonState == LOW && (millis() - lastTime) > debounceDelay) 
  {
    ledState = !ledState; // Toggle LED state
    digitalWrite(LedPin, ledState); // Update LED based on ledState
    lastTime = millis();
  } else {
  }
  lastButtonState = buttonState;
  
}

