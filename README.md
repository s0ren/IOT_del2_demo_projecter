# Demo er til faget IOT 2. del

## Eksempler fra undervisningssiderne

Hvert projekt er et eksempel fra en side på [fagets site](https://s0ren.github.io/FAG_17682_IoT_II/). Koden er den samme som på siden; retter du den ene, så ret også den anden.

Alle projekter bruger `platform = espressif32@8.0.0` og kortet ESP32-DEVKIT-V1. De kompilerer (24. nov. 2026), men er ikke alle testet på hardware endnu.

| Side | Projekt | Indhold |
|---|---|---|
| [Deep sleep](https://s0ren.github.io/FAG_17682_IoT_II/Features/deep_sleep.html) | [`deep_sleep_timer`](deep_sleep_timer) | vågn på timer |
|  | [`deep_sleep_knap_timer`](deep_sleep_knap_timer) | vågn på knap (GPIO 33) eller timer |
| [ESP-NOW og mesh](https://s0ren.github.io/FAG_17682_IoT_II/Features/esp_now_mesh.html) | [`esp_now_modtager`](esp_now_modtager) | ESP-NOW-modtager |
|  | [`esp_now_afsender`](esp_now_afsender) | ESP-NOW-afsender (brug to kort) |
| [Filsystem](https://s0ren.github.io/FAG_17682_IoT_II/Features/filsystem.html) | [`filsystem_laes_skriv`](filsystem_laes_skriv) | læs, skriv og tilføj med LittleFS |
|  | [`filsystem_data_mappe`](filsystem_data_mappe) | filer fra data/-mappen (Upload Filesystem Image) |
|  | [`filsystem_cache`](filsystem_cache) | cache af målinger, når netværket er nede |
| [Lokal MQTT](https://s0ren.github.io/FAG_17682_IoT_II/Features/lokal_mqtt.html) | [`lokal_mqtt_tls`](lokal_mqtt_tls) | MQTT over TLS til wilsons (port 8883); kopiér include/secrets_eksempel.h til secrets.h |
| [Intro til MQTT](https://s0ren.github.io/FAG_17682_IoT_II/Features/mqtt_intro.html) | [`mqtt_intro_potentiometer`](mqtt_intro_potentiometer) | sender potentiometerets måling over MQTT (port 1883); kopiér include/secrets_eksempel.h til secrets.h |
