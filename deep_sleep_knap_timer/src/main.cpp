#include <Arduino.h>
#include "esp_sleep.h"
#include "driver/rtc_io.h"

// Konfiguration
const gpio_num_t KNAP_PIN = GPIO_NUM_33;   // knap mellem GPIO 33 og GND (RTC-GPIO)
const int LED_PIN = 2;                     // indbygget lysdiode
const uint64_t SOVETID_S = 30;
const uint64_t US_PR_SEKUND = 1000000ULL;
const int BLINK_MS = 200;

RTC_DATA_ATTR int antalKnapTryk = 0;       // overlever deep sleep

void blink() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  delay(BLINK_MS);  // kort pause i setup(), før vi alligevel går i dvale
  digitalWrite(LED_PIN, LOW);
}

void haandterOpvaagning() {
  esp_sleep_wakeup_cause_t aarsag = esp_sleep_get_wakeup_cause();

  if (aarsag == ESP_SLEEP_WAKEUP_EXT0) {
    antalKnapTryk++;
    Serial.printf("Knap trykket (%d gange i alt)\n", antalKnapTryk);
    blink();
  } else if (aarsag == ESP_SLEEP_WAKEUP_TIMER) {
    Serial.println("Timer: tid til en måling");
  } else {
    Serial.println("Første opstart");
  }
}

void gaaISeng() {
  // Vågn, når knappen trykker benet LOW
  esp_sleep_enable_ext0_wakeup(KNAP_PIN, 0);
  // INPUT_PULLUP virker ikke i deep sleep; RTC-delen skal selv holde benet HIGH
  rtc_gpio_pullup_en(KNAP_PIN);
  rtc_gpio_pulldown_dis(KNAP_PIN);

  // Vågn også efter et stykke tid, selv om ingen trykker
  esp_sleep_enable_timer_wakeup(SOVETID_S * US_PR_SEKUND);

  Serial.println("Sover ...");
  Serial.flush();
  esp_deep_sleep_start();
}

void setup() {
  Serial.begin(115200);
  delay(500);

  haandterOpvaagning();
  gaaISeng();
}

void loop() {
  // Bliver aldrig kørt
}
