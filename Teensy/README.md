# Teensy LC USB-Ampel

## 1. Teensy LC mit Neosegments

* Sensor Sensirion SCD30 für CO2-Messung
* 2 Doppelsegmente Neosegments, 7-Segment-Anzeige mit Farbe
* Das geht und ist im Einsatz in der Schule, Details siehe [hier](CO2_Ampel/README.md) 

## 2. Teensy LC mit Neopixel-Ring und kleinem OLED-Display

* Sensor Sensirion SCD30 für CO2-Messung
* Neopixel-Ring mit 16 RGB-Leds, je LED für 100 ppm
* Die LEDs wechseln natürlich auch die Farbe
* zusätzlich ein Mini-OLED-Display, das die ppm-Werte anzeigt
* Das ist gerade im Aufbau, Einzel-SW-Teile gehen schon, siehe [hier](CO2_Ampel_OLed/README.md)

## 3. Teensy LC mit ePaper-Shield 

* Epaper-shield von Crowd-Supply
* Software von, mit Patch von mir: 
* Graph-Routines von https://create.arduino.cc/projecthub/andreiflorian/tft-graphing-live-history-graphs-744f3b
* Ist noch stark im Aufbau, die Epaper-Shield-SW braucht noch nen weiteren Patch* Die Graph-Routines dienen als Inspiration, sind selbst aber etwas buggy und können so nicht verwendet werden.

