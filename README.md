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


## System 2 (ist fertig)

und sieht so aus:

* Teensy LC 
* Neo-Pixel-Ring mit 16 LEDs, pro 100 ppm 1 LED bis max 1699 ppm unterscheidbar, farbkodierte Wertebereiche
* OLED-Display 0.91" , 128 x 32, für Ausgabe der genauen CO2-Konzentration 
* Ein BCD-Dreh-Encoder für die Helligkeit der Neopixel-LEDs e Grafik-Anzeige mit Zeit ...

Siehe [hier im Unterverzeichnis Teensy/CO2_Ampel_OLed](Teensy/CO2_Ampel_OLed/README.md)

 


## System 3:

* Raspberry Pi (mal schauen, welcher...)
* grosse USB-Ampel von Cleware
* 3.5 " LCD für Raspi
* evt. je nach Aufstellungs-Ort, auch noch eine RTC

