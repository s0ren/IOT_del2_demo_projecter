#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "secrets.h"   // WIFI_SSID og WIFI_PASSWORD (se WiFi-siden)

// Konfiguration
const char* MQTT_HOST = "wilsons.local";         // brokeren; virker navnet ikke, så "192.168.0.33"
const uint16_t MQTT_PORT = 1883;                 // uden TLS; se Lokal MQTT server for 8883
const char* BRUGERNAVN = "anna";                 // dit eget brugernavn
const int MAALE_PIN = 32;                      // potentiometer, ADC1
const unsigned long SEND_INTERVAL_MS = 5000;     // hvor tit der sendes en måling
const unsigned long MQTT_FORSOEG_MS = 5000;      // ventetid mellem forsøg på at forbinde
const unsigned long WIFI_TIMEOUT_MS = 20000;     // max ventetid på WiFi i setup()

WiFiClient netKlient;            // TCP-forbindelsen
PubSubClient mqtt(netKlient);    // MQTT oven på TCP-forbindelsen

char klientId[24];               // fx "esp32-a1b2c3d4e5f6"
char topic[64];                  // fx "elever/anna/esp32/maaling"
unsigned long sidsteSend = 0;
unsigned long sidsteForsoeg = 0;
unsigned long antalSendt = 0;

// Client-id ud fra MAC-adressen, så to ESP32'er aldrig har samme id.
// getEfuseMac() har MAC'ens første byte nederst, så bytes står i omvendt
// rækkefølge i forhold til WiFi.macAddress(). Id'et er stadig unikt.
void lavKlientId() {
  uint64_t mac = ESP.getEfuseMac();
  snprintf(klientId, sizeof(klientId), "esp32-%04x%08x",
           (unsigned int)(uint16_t)(mac >> 32), (unsigned int)(uint32_t)mac);
}

// Som på WiFi-siden: vent højst timeoutMs, så programmet ikke hænger
bool forbindWiFi(unsigned long timeoutMs) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.printf("Forbinder til %s", WIFI_SSID);
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < timeoutMs) {
    delay(500);  // OK her: setup() har ikke andet at lave endnu
    Serial.print(".");
  }
  Serial.println();
  return WiFi.status() == WL_CONNECTED;
}

// Ét forsøg på at forbinde til brokeren. Ingen while-løkke: loop() prøver igen senere.
bool forbindMqtt() {
  Serial.printf("Forbinder til MQTT %s:%u som %s ... ", MQTT_HOST, MQTT_PORT, klientId);
  if (mqtt.connect(klientId)) {
    Serial.println("OK");
    return true;
  }
  Serial.printf("fejl, state=%d\n", mqtt.state());
  return false;
}

void sendMaaling() {
  int millivolt = analogReadMilliVolts(MAALE_PIN);
  antalSendt++;

  char payload[128];
  snprintf(payload, sizeof(payload),
           "{\"device\":\"%s\",\"nr\":%lu,\"spaending_mv\":%d,\"rssi\":%d,\"uptime_ms\":%lu}",
           klientId, antalSendt, millivolt, WiFi.RSSI(), millis());

  // publish() giver false, hvis vi ikke er forbundet, eller beskeden er for stor
  bool ok = mqtt.publish(topic, payload);
  Serial.printf("%s %s %s\n", ok ? "Sendt" : "IKKE sendt", topic, payload);
}

void setup() {
  Serial.begin(115200);
  lavKlientId();
  snprintf(topic, sizeof(topic), "elever/%s/esp32/maaling", BRUGERNAVN);

  if (!forbindWiFi(WIFI_TIMEOUT_MS)) {
    Serial.printf("Ingen WiFi endnu (status %d). Prøver videre i baggrunden.\n", WiFi.status());
  }

  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  if (WiFi.status() == WL_CONNECTED) {
    forbindMqtt();
  }
  sidsteForsoeg = millis();
}

void loop() {
  unsigned long nu = millis();

  if (mqtt.connected()) {
    mqtt.loop();  // sender keep-alive-ping og modtager beskeder; skal kaldes ofte
  } else if (WiFi.status() == WL_CONNECTED && nu - sidsteForsoeg >= MQTT_FORSOEG_MS) {
    // Prøv igen med fast interval i stedet for en while-løkke med delay()
    sidsteForsoeg = nu;
    forbindMqtt();
  }

  if (mqtt.connected() && nu - sidsteSend >= SEND_INTERVAL_MS) {
    sidsteSend = nu;
    sendMaaling();
  }

  // Her kan programmet læse knapper og sensorer, også mens forbindelsen er nede
}
