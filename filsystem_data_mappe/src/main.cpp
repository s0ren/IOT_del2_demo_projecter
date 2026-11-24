#include <Arduino.h>
#include <LittleFS.h>

// Konfiguration
const char *HILSEN_FIL = "/hilsen.txt";  // lægges op fra data/hilsen.txt

void setup() {
  Serial.begin(115200);

  // false: formatér IKKE, så vi kan se, hvis imaget ikke er uploadet rigtigt
  if (!LittleFS.begin(false)) {
    Serial.println("LittleFS kunne ikke monteres. Er Upload Filesystem Image kørt?");
    return;
  }

  File fil = LittleFS.open(HILSEN_FIL, FILE_READ);
  if (!fil) {
    Serial.println("hilsen.txt findes ikke");
    return;
  }
  while (fil.available()) {
    Serial.write(fil.read());
  }
  fil.close();
}

void loop() {
}
