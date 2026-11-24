#include <Arduino.h>
#include "esp_sleep.h"

// Konfiguration
const uint64_t SOVETID_S = 10;              // sekunder mellem opvågninger
const uint64_t US_PR_SEKUND = 1000000ULL;   // timeren regner i mikrosekunder

// Ligger i RTC-hukommelsen, så værdien overlever deep sleep
RTC_DATA_ATTR int antalOpstarter = 0;

void skrivVaekkeaarsag() {
  esp_sleep_wakeup_cause_t aarsag = esp_sleep_get_wakeup_cause();

  switch (aarsag) {
    case ESP_SLEEP_WAKEUP_TIMER: Serial.println("Vækket af: timer"); break;
    case ESP_SLEEP_WAKEUP_EXT0:  Serial.println("Vækket af: knap (ext0)"); break;
    case ESP_SLEEP_WAKEUP_EXT1:  Serial.println("Vækket af: knap (ext1)"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD: Serial.println("Vækket af: touch"); break;
    default:
      // Første opstart, EN-knappen eller upload
      Serial.printf("Ikke vækket fra deep sleep (%d)\n", aarsag);
      break;
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);  // giver Serial Monitor tid til at følge med efter opvågning

  antalOpstarter++;
  Serial.printf("Opstart nr. %d\n", antalOpstarter);
  skrivVaekkeaarsag();

  // Her ville en rigtig sensor måle og sende data

  esp_sleep_enable_timer_wakeup(SOVETID_S * US_PR_SEKUND);
  Serial.printf("Sover i %llu s\n", SOVETID_S);
  Serial.flush();  // vent, til teksten er sendt, før UART slukkes
  esp_deep_sleep_start();
  // Herfra kommer programmet aldrig
}

void loop() {
  // Bliver aldrig kørt: ESP32 starter forfra i setup() efter hver opvågning
}
