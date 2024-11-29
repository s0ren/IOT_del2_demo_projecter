#include <Arduino.h>

#define LED BUILTIN_LED

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(1000);
  Serial.println("Hello, World!");

  pinMode(LED, OUTPUT);
  Serial.println("Bruger LED på pin: " + String(LED)); 
}

void loop() {
  // put your main code here, to run repeatedly:
  
  // Tænd LED
  digitalWrite(LED, 1);
  Serial.print("*");
  delay(500);

  // Sluk LED
  digitalWrite(LED, 0);
  Serial.print(".");
  delay(500);
}
