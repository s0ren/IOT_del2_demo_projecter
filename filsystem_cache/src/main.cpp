#include <Arduino.h>
#include <LittleFS.h>

// Konfiguration
const int NET_PIN = 33;                        // knap (eller ledning) til GND = "netværket er nede"
const unsigned long MAALE_INTERVAL_MS = 5000;  // kort interval, så man kan følge med
const char *CACHE_FIL = "/cache.jsonl";        // én JSON-linje pr. måling
const size_t MIN_LEDIG_BYTES = 16 * 1024;      // gem ikke mere, når pladsen er under dette

unsigned long sidsteMaaling = 0;

// Simuleret netværk. Skiftes ud med fx WiFi.status() == WL_CONNECTED.
bool netvaerkOk() {
  return digitalRead(NET_PIN) == HIGH;  // pull-up: HIGH, når knappen ikke er trykket
}

// Simuleret afsendelse. Skiftes ud med fx en MQTT-publish.
// Giver false, hvis beskeden ikke kom af sted.
bool sendMaaling(const String &linje) {
  if (!netvaerkOk()) {
    return false;
  }
  Serial.print("Sendt:  ");
  Serial.println(linje);
  return true;
}

// Simuleret sensor: 20,0-24,9 grader
float laesTemperatur() {
  return 20.0f + random(0, 50) / 10.0f;
}

// Bygger én måling som JSON, uden linjeskift
String lavJsonLinje(unsigned long tid, float temperatur) {
  char buffer[64];
  snprintf(buffer, sizeof(buffer), "{\"tid_ms\":%lu,\"temp\":%.1f}", tid, temperatur);
  return String(buffer);
}

// Tilføjer en måling til cachen, hvis der er plads
void gemICache(const String &linje) {
  size_t ledig = LittleFS.totalBytes() - LittleFS.usedBytes();
  if (ledig < MIN_LEDIG_BYTES) {
    Serial.println("Cachen er fuld: målingen kasseres");
    return;
  }
  File fil = LittleFS.open(CACHE_FIL, FILE_APPEND);
  if (!fil) {
    Serial.println("Kunne ikke åbne cachen");
    return;
  }
  fil.print(linje);
  fil.print('\n');  // \n er JSON Lines' linjeskift; println() ville også tilføje \r
  fil.close();      // først nu er målingen sikkert gemt
  Serial.print("Cachet: ");
  Serial.println(linje);
}

// Sender alle målinger i cachen og sletter filen, når alt er sendt
void toemCache() {
  if (!LittleFS.exists(CACHE_FIL)) {
    return;  // intet at sende
  }
  File fil = LittleFS.open(CACHE_FIL, FILE_READ);
  if (!fil) {
    return;
  }
  int antalSendt = 0;
  bool altSendt = true;
  while (fil.available()) {
    String linje = fil.readStringUntil('\n');
    linje.trim();  // fjerner evt. \r og mellemrum
    if (linje.length() == 0) {
      continue;    // tomme linjer springes over
    }
    if (!sendMaaling(linje)) {
      altSendt = false;  // netværket forsvandt undervejs
      break;
    }
    antalSendt++;
  }
  fil.close();

  if (altSendt) {
    LittleFS.remove(CACHE_FIL);
    Serial.printf("Cachen er tømt: %d målinger sendt\n", antalSendt);
  } else {
    // Filen beholdes. De linjer, der nåede at blive sendt, sendes igen næste gang.
    Serial.printf("Afbrudt efter %d målinger, cachen beholdes\n", antalSendt);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(NET_PIN, INPUT_PULLUP);

  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS kunne ikke monteres");
    return;
  }

  // Ligger der målinger fra før en genstart?
  if (LittleFS.exists(CACHE_FIL)) {
    File fil = LittleFS.open(CACHE_FIL, FILE_READ);
    Serial.printf("Cache fra før genstart: %u bytes\n", (unsigned)fil.size());
    fil.close();
  }
}

void loop() {
  if (millis() - sidsteMaaling >= MAALE_INTERVAL_MS) {
    sidsteMaaling = millis();
    String linje = lavJsonLinje(millis(), laesTemperatur());

    if (netvaerkOk()) {
      toemCache();  // gamle målinger først, så rækkefølgen bevares
    }
    if (!sendMaaling(linje)) {
      gemICache(linje);
    }
  }
}
