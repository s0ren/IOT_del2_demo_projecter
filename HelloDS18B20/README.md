# Temperatur føler DS18B20

Baseret på DallasTemperature library. <https://registry.platformio.org/libraries/milesburton/DallasTemperature>

For diagram og forklaring, se <https://randomnerdtutorials.com/esp32-ds18b20-temperature-arduino-ide/>.  
Her kan man både se billeder af sensoren, diagram og demo program.
I denne udgave, **SKAL** der være en 4,7k ohms pullup modstand.

# Konklusion

**DS18B20** kommunikerer med _One-Wire_ protokol, som kun bruger _**en**_ kanal (altså en pin) til at kommunikere potentielt med flere enheder på One-Wire- _bussen_.

# Links

* <https://randomnerdtutorials.com/esp32-ds18b20-temperature-arduino-ide/>
* <https://ardustore.dk/produkt/ds18b20-temperatur-sensor-1-meter?gad_source=1&gclid=Cj0KCQiAgJa6BhCOARIsAMiL7V_quTqqtvX2ZxpK40P6j3jAWplhwcensZPt6Y-Zr5kBl3l8q5c5pKoaAvjcEALw_wcB>
* <https://www.milesburton.com/w/index.php/Dallas_Temperature_Control_Library>
* <https://randomnerdtutorials.com/esp32-multiple-ds18b20-temperature-sensors/>
