#include <Arduino.h>
#include <LittleFS.h>

// Konfiguration
const char *ANTAL_FIL = "/antal.txt";  // antal opstarter, overskrives hver gang
const char *LOG_FIL = "/log.txt";      // én linje pr. opstart, vokser

// Læser antal opstarter. Giver 0, hvis filen ikke findes endnu.
int laesAntal() {
  if (!LittleFS.exists(ANTAL_FIL)) {
    return 0;
  }
  File fil = LittleFS.open(ANTAL_FIL, FILE_READ);
  if (!fil) {
    return 0;
  }
  // Tallet er gemt med et linjeskift efter, så readStringUntil stopper der
  String tekst = fil.readStringUntil('\n');
  fil.close();
  return tekst.toInt();
}

// Overskriver filen med det nye tal
void skrivAntal(int antal) {
  File fil = LittleFS.open(ANTAL_FIL, FILE_WRITE);  // sletter det gamle indhold
  if (!fil) {
    Serial.println("Kunne ikke skrive antal");
    return;
  }
  fil.println(antal);
  fil.close();  // først nu er tallet sikkert gemt
}

// Tilføjer en linje til sidst i filen
void tilfoejLinje(const char *sti, const String &linje) {
  File fil = LittleFS.open(sti, FILE_APPEND);  // opretter filen, hvis den mangler
  if (!fil) {
    Serial.printf("Kunne ikke åbne %s\n", sti);
    return;
  }
  fil.println(linje);
  fil.close();
}

// Skriver hele filens indhold i Serial Monitor
void udskrivFil(const char *sti) {
  File fil = LittleFS.open(sti, FILE_READ);
  if (!fil) {
    Serial.printf("%s findes ikke\n", sti);
    return;
  }
  Serial.printf("--- %s (%u bytes) ---\n", sti, (unsigned)fil.size());
  while (fil.available()) {
    Serial.write(fil.read());
  }
  fil.close();
}

// Lister filerne i en mappe med deres størrelse
void listFiler(const char *mappe) {
  File rod = LittleFS.open(mappe);
  if (!rod || !rod.isDirectory()) {
    Serial.printf("%s er ikke en mappe\n", mappe);
    return;
  }
  Serial.printf("--- Filer i %s ---\n", mappe);
  File fil = rod.openNextFile();
  while (fil) {
    Serial.printf("%-20s %6u bytes\n", fil.name(), (unsigned)fil.size());
    fil = rod.openNextFile();
  }
}

// Viser, hvor meget af filsystemet der er brugt
void visPlads() {
  size_t total = LittleFS.totalBytes();
  size_t brugt = LittleFS.usedBytes();
  Serial.printf("Plads: %u bytes i alt, %u brugt, %u ledig\n",
                (unsigned)total, (unsigned)brugt, (unsigned)(total - brugt));
}

void setup() {
  Serial.begin(115200);

  // true: formatér, hvis partitionen ikke kan monteres (fx første gang)
  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS kunne ikke monteres");
    return;
  }

  int antal = laesAntal() + 1;
  skrivAntal(antal);
  tilfoejLinje(LOG_FIL, "Opstart nr. " + String(antal));

  udskrivFil(LOG_FIL);
  listFiler("/");
  visPlads();
}

void loop() {
}
