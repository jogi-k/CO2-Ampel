# CO2-Ampel

Ziel : 3 verschiedene Systeme für eine CO2-Ampel

Alle nutzen den CO2-Sensor von Sensirion, der via I2C-Bus folgende Daten ausgibt:

* CO2-Konzentration in ppm
* Temperatur in °C
* Luftfeuchte in %

Die drei Ampeln nutzen ansonsten, was der Bastler-Haushalt so hergibt.


## System 1 (ist fertig) :

Siehe [hier im Unterverzeichnis Teensy/CO2_Ampel](Teensy/CO2_Ampel/README.md)

* Teensy-LC
* 4 * 7-Segment-Anzeigen mit Neo-Pixeln, ist also gleichzeitig die Ampel, da die Farben angezeigt werden


## System 2 

wird vermutlich so aussehen:

* Teensy LC 
* 3 Simple LEDs
* E-Paper-Shield für Teensy
* evt ein RTC-Breakout-Board für die Grafik-Anzeige mit Zeit ...

 


## System 3:

* Raspberry Pi (mal schauen, welcher...)
* grosse USB-Ampel von Cleware
* 3.5 " LCD für Raspi
* evt. je nach Aufstellungs-Ort, auch noch eine RTC

