#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "secrets.h"   // WIFI_SSID, WIFI_PASSWORD, MQTT_USER, MQTT_PASSWORD
#include "ca_cert.h"   // CA_CERT

// Konfiguration
const char* MQTT_HOST = "wilsons.local";       // skal stå i brokerens certifikat
const uint16_t MQTT_PORT = 8883;               // TLS + login
const unsigned long WIFI_TIMEOUT_MS  = 20000;  // max ventetid på WiFi i setup()
const unsigned long MQTT_FORSOEG_MS  = 5000;   // pause mellem forsøg på at forbinde
const unsigned long SEND_INTERVAL_MS = 10000;  // hvor tit der sendes en måling

WiFiClientSecure tlsKlient;      // krypteret forbindelse
PubSubClient mqtt(tlsKlient);    // MQTT oven på den krypterede forbindelse

String sendTopic;     // elever/<brugernavn>/status
String modtagTopic;   // elever/<brugernavn>/kommando
unsigned long sidsteForsoeg = 0;
unsigned long sidsteSend = 0;

// Som på WiFi-siden: vent højst timeoutMs
bool forbindWiFi(unsigned long timeoutMs) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.printf("Forbinder til WiFi %s", WIFI_SSID);
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < timeoutMs) {
    delay(500);  // OK her: setup() har ikke andet at lave endnu
    Serial.print(".");
  }
  Serial.println();
  return WiFi.status() == WL_CONNECTED;
}

// Kaldes af mqtt.loop(), når der kommer en besked på et topic, vi abonnerer på
void modtag(char* topic, byte* payload, unsigned int laengde) {
  Serial.printf("Modtaget på %s: ", topic);
  Serial.write(payload, laengde);  // payload er ikke en C-streng med \0 til sidst
  Serial.println();
}

bool forbindMqtt() {
  // Client-id skal være unikt på brokeren. De sidste tre bytes af MAC-adressen er forskellige fra kort til kort.
  String klientId = String(MQTT_USER) + "-" + String((uint32_t)(ESP.getEfuseMac() >> 24), HEX);

  Serial.printf("Forbinder til %s:%u ... ", MQTT_HOST, MQTT_PORT);
  if (mqtt.connect(klientId.c_str(), MQTT_USER, MQTT_PASSWORD)) {
    Serial.println("OK");
    // Abonnér efter hver forbindelse, ligesom on_connect i Python
    mqtt.subscribe(modtagTopic.c_str());
    return true;
  }

  // Skriv både MQTT-status og en eventuel TLS-fejl ud; det er de bedste spor
  char tlsTekst[100] = "";
  int tlsFejl = tlsKlient.lastError(tlsTekst, sizeof(tlsTekst));
  Serial.printf("fejl, rc=%d", mqtt.state());
  if (tlsFejl != 0) {
    Serial.printf(", TLS: %s", tlsTekst);
  }
  Serial.println();
  return false;
}

void sendMaaling() {
  char payload[100];
  snprintf(payload, sizeof(payload), "{\"uptime_s\":%lu,\"rssi\":%d}",
           millis() / 1000, WiFi.RSSI());
  bool ok = mqtt.publish(sendTopic.c_str(), payload);
  Serial.printf("%s %s %s\n", ok ? "Sendt" : "IKKE sendt", sendTopic.c_str(), payload);
}

void setup() {
  Serial.begin(115200);

  // Topics bygges af brugernavnet, så de altid passer med ACL'en
  sendTopic   = String("elever/") + MQTT_USER + "/status";
  modtagTopic = String("elever/") + MQTT_USER + "/kommando";

  if (!forbindWiFi(WIFI_TIMEOUT_MS)) {
    Serial.printf("Ingen WiFi (status %d). ESP32 prøver selv igen.\n", WiFi.status());
  }

  tlsKlient.setCACert(CA_CERT);          // tjek brokerens certifikat mod skolens CA
  mqtt.setServer(MQTT_HOST, MQTT_PORT);  // navnet bruges også til at tjekke certifikatet
  mqtt.setCallback(modtag);
}

void loop() {
  unsigned long nu = millis();

  if (mqtt.connected()) {
    mqtt.loop();  // modtager beskeder og holder forbindelsen i live

    if (nu - sidsteSend >= SEND_INTERVAL_MS) {
      sidsteSend = nu;
      sendMaaling();
    }
  } else if (WiFi.status() == WL_CONNECTED && nu - sidsteForsoeg >= MQTT_FORSOEG_MS) {
    // Forbind (igen) med fast interval, så loop() ikke hænger i en løkke
    sidsteForsoeg = nu;
    forbindMqtt();
  }

  // Her kan programmet læse sensorer og knapper
}
