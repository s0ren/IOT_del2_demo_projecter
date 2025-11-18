#include <Arduino.h>
#include "OneButton.h"
#include <ezLED.h> 

#define BuildinledPin 2; // GPIO2 is often connected to the onboard LED on ESP32 boards
#define ButtonPin 32    // GPIO0 is often used for a button on ESP32 boards
#define RedLedPin 4 // GPIO33 for an external LED
#define BlueLedPin 5

enum LedStates {
  Off = 1,
  On = 2,
  Blinking = 4
};

enum LedStates redLedState = Off;
enum LedStates blueLedState = On;

// int ledState1 = LOW; // LED is off when pin is HIGH
// int ledState2 = HIGH; // LED2 is off when pin is HIGH
// int lastButtonState = LOW; // Previous state of the button
int debounceDelay = 50; // Debounce time in milliseconds
int lastTime = 0; // Last time the button state changed

OneButton button;

ezLED redLed(RedLedPin);
ezLED blueLed(BlueLedPin); 

void Click(); // function prototype for button click event

void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:

  pinMode(RedLedPin, OUTPUT);
  pinMode(BlueLedPin, OUTPUT);
  // digitalWrite(LedPin, HIGH); // Ensure LED is off at start
  // digitalWrite(LedPin2, LOW); // Ensure LED2 is off at start
  redLed.turnON();
  blueLed.turnOFF();
  

  delay(1000); // Wait for a second

  pinMode(ButtonPin, INPUT);

  // setup OneButton
  button.setup(ButtonPin, INPUT, false);

  // link the click function to be called on a doubleclick event.
  button.attachClick(Click);
}

void Click() {
  Serial.println("Button clicked!");
  // ledState1 = !ledState1; // Toggle LED state
  // ledState2 = !ledState2; // Toggle LED state
  
  // digitalWrite(LedPin, ledState1); // Update LED based on ledState
  // digitalWrite(LedPin2, ledState2); // Update LED2 based on ledState
}

void loop() {
  button.tick(); // check the button state and trigger events
}

