# CO2-Ampel

Ziel : 2 verschiedene Systeme für eine CO2-Ampel

Beide nutzen den CO2-Sensor von Sensirion, der via I2C-Bus folgende Daten ausgibt:

* CO2-Konzentration in ppm
* Temperatur in °C
* Luftfeuchte in %

Die beiden Ampeln nutzen ansonsten, was der Bastler-Haushalt so hergibt.


## System 1:

* Teensy-LC
* Mini-Ampel mit einzelnen LEDs 
* E-Paper-Shield 
* RTC-Breakout-Board 


## System 2:

* Raspberry Pi (mal schauen, welcher...)
* grosse USB-Ampel von Cleware
* 3.5 " LCD für Raspi
* evt. je nach Aufstellungs-Ort, auch noch eine RTC

  
