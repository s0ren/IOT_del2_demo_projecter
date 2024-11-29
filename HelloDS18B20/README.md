# Temperatur føler DS18B20

Baseret på DallasTemperature library. <https://registry.platformio.org/libraries/milesburton/DallasTemperature>

For diagram og forklaring, se <https://randomnerdtutorials.com/esp32-ds18b20-temperature-arduino-ide/>.  
Her kan man både se billeder af sensoren, diagram og demo program.
I denne udgave, **SKAL** der være en 4,7k ohms pullup modstand.

# Konklusion

**DS18B20** kommunikerer med _One-Wire_ protokol, som kun bruger _**en**_ kanal (altså en pin) til at kommunikere potentielt med flere enheder på One-Wire- _bussen_.

