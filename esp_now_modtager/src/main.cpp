#include <Arduino.h>
#include <WiFi.h>
#include <ESP32_NOW.h>

// Konfiguration
const uint8_t KANAL = 1;          // WiFi-kanal; skal være den samme på alle enheder
const int KOE_LAENGDE = 10;       // hvor mange beskeder der kan vente på loop()

// Beskedens format. Skal være defineret helt ens hos afsender og modtager.
struct Besked {
  uint32_t nummer;     // løbenummer, så vi kan se, om der mangler beskeder
  uint32_t oppetidMs;  // afsenderens millis(), da beskeden blev sendt
};

// Det, callback-funktionen lægger i køen til loop()
struct Modtaget {
  uint8_t afsender[6];  // afsenderens MAC-adresse
  bool broadcast;       // true, hvis beskeden var sendt til alle
  Besked besked;
};

QueueHandle_t koe;

// Kaldes af ESP_NOW-biblioteket for hver besked fra en afsender, der ikke er
// registreret som peer. Vi registrerer ingen peers, så alle beskeder kommer her.
// Kører i WiFi-opgaven: kopier data over i køen, og intet andet.
void modtagBesked(const esp_now_recv_info_t *info, const uint8_t *data, int len, void *arg) {
  if (len != sizeof(Besked)) {
    return;  // ikke vores format, fx fra en anden gruppe på samme kanal
  }
  Modtaget m;
  memcpy(m.afsender, info->src_addr, 6);
  m.broadcast = memcmp(info->des_addr, ESP_NOW.BROADCAST_ADDR, 6) == 0;
  memcpy(&m.besked, data, sizeof(Besked));
  xQueueSend(koe, &m, 0);  // 0 = vent ikke; er køen fuld, smides beskeden væk
}

void startWiFi() {
  WiFi.mode(WIFI_STA);    // station mode, men vi forbinder ikke til en router
  WiFi.setChannel(KANAL);
  while (!WiFi.STA.started()) {
    delay(100);  // OK her: setup() har ikke andet at lave endnu
  }
}

void skrivMac(const uint8_t *mac) {
  Serial.printf("%02X:%02X:%02X:%02X:%02X:%02X",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void setup() {
  Serial.begin(115200);
  koe = xQueueCreate(KOE_LAENGDE, sizeof(Modtaget));

  startWiFi();
  Serial.printf("Modtager. MAC-adresse: %s, kanal: %d\n",
                WiFi.macAddress().c_str(), (int)WiFi.channel());

  if (!ESP_NOW.begin()) {
    Serial.println("ESP-NOW kunne ikke starte. Genstarter om 5 s.");
    delay(5000);
    ESP.restart();
  }
  ESP_NOW.onNewPeer(modtagBesked, nullptr);
}

void loop() {
  Modtaget m;
  // Hent alle ventende beskeder; 0 = vent ikke, hvis køen er tom
  while (xQueueReceive(koe, &m, 0) == pdTRUE) {
    Serial.print("Fra ");
    skrivMac(m.afsender);
    Serial.printf(" %s: nr. %lu, afsenderens oppetid %lu ms\n",
                  m.broadcast ? "(broadcast)" : "(unicast)",
                  (unsigned long)m.besked.nummer,
                  (unsigned long)m.besked.oppetidMs);
  }

  // Her kan programmet lave andet, fx måle
}
