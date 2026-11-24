#include <Arduino.h>
#include <WiFi.h>
#include <ESP32_NOW.h>

// Konfiguration
const uint8_t KANAL = 1;  // skal være den samme som modtagerens
const uint8_t MODTAGER_MAC[6] = {0x24, 0x6F, 0x28, 0xA1, 0xB2, 0xC3};  // ret til jeres modtager
const unsigned long SEND_INTERVAL_MS = 2000;

// Beskedens format. Skal være defineret helt ens hos afsender og modtager.
struct Besked {
  uint32_t nummer;
  uint32_t oppetidMs;
};

// Sættes af onSent() i WiFi-opgaven og læses i loop()
volatile bool kvitteringKlar = false;
volatile bool sidsteKvitteret = false;

// ESP_NOW_Peer kan ikke bruges direkte: konstruktøren, add() og send() er
// protected. Derfor en lille klasse, der arver og gør dem tilgængelige.
class ModtagerPeer : public ESP_NOW_Peer {
public:
  ModtagerPeer(const uint8_t *mac, uint8_t kanal)
      : ESP_NOW_Peer(mac, kanal, WIFI_IF_STA, nullptr) {}  // nullptr = ingen kryptering

  bool tilfoej() {
    return add();
  }

  bool sendBesked(const Besked &besked) {
    // send() giver antal byte, der er sat i kø til afsendelse, eller 0 ved fejl.
    // Færre byte end sizeof(besked) betyder, at beskeden er klippet over.
    return send((const uint8_t *)&besked, sizeof(besked)) == sizeof(besked);
  }

  // Kaldes af WiFi-opgaven, når modtagerens radio har kvitteret, eller ikke.
  // Hold den kort: sæt kun flag, og skriv ud i loop().
  void onSent(bool success) override {
    sidsteKvitteret = success;
    kvitteringKlar = true;
  }
};

ModtagerPeer modtager(MODTAGER_MAC, KANAL);
uint32_t naesteNummer = 0;
unsigned long sidsteSend = 0;

void startWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.setChannel(KANAL);
  while (!WiFi.STA.started()) {
    delay(100);  // OK her: setup() har ikke andet at lave endnu
  }
}

void setup() {
  Serial.begin(115200);

  startWiFi();
  Serial.printf("Afsender. MAC-adresse: %s, kanal: %d\n",
                WiFi.macAddress().c_str(), (int)WiFi.channel());

  // ESP-NOW skal startes, før peeren kan tilføjes
  if (!ESP_NOW.begin() || !modtager.tilfoej()) {
    Serial.println("ESP-NOW eller peer kunne ikke startes. Genstarter om 5 s.");
    delay(5000);
    ESP.restart();
  }
}

void loop() {
  unsigned long nu = millis();

  if (nu - sidsteSend >= SEND_INTERVAL_MS) {
    sidsteSend = nu;
    Besked besked = {naesteNummer, (uint32_t)nu};
    if (modtager.sendBesked(besked)) {
      Serial.printf("Sendt nr. %lu\n", (unsigned long)besked.nummer);
    } else {
      Serial.println("send() fejlede");
    }
    naesteNummer++;
  }

  if (kvitteringKlar) {
    kvitteringKlar = false;
    if (memcmp(modtager.addr(), ESP_NOW.BROADCAST_ADDR, 6) == 0) {
      // Ved broadcast giver onSent() altid success, så kvitteringen siger intet
      Serial.println("  sendt som broadcast (ingen kvittering)");
    } else {
      Serial.println(sidsteKvitteret ? "  kvitteret af modtageren" : "  IKKE kvitteret");
    }
  }
}
