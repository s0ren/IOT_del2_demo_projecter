#include <Arduino.h>

// put function declarations here:
int myFunction(int, int);

#define FC51 GPIO_NUM_33

void setup() {
  // put your setup code here, to run once:
  // int result = myFunction(2, 3);
  Serial.begin(115200);
  delay(500);
  Serial.println("Hej Søren");

  // setup pi til sensor
  
pinMode(FC51, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("-");
  delay(1000);
  if (digitalRead(FC51) == 1)
  {
    Serial.println("Der er noget!");
  }
  else
  {
    Serial.println("Der er IKKE noget...");
  }
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}