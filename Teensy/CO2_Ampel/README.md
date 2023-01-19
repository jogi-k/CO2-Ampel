# Teensy LC USB-Ampel mit Neosegments

* Der Sensirion SCD30 CO2-Sensor wird mit I2C angesprochen
* dafür gibt es eine Library, SparkFun_SCD30_Arduino_Library 
* diese wurde von mir gepatcht, um mit anstatt mit der wire-I2C-library die besser für Teensy 3 / Teensy LC geeignete i2c_t3 - Library zu verwenden
* Der Patch ist inzwischen auf dem Master-Branch von Sparkfun
* Muss jedoch explizit im Headerfile eingeschaltet werden, siehe  // #define USE_TEENSY3_I2C_LIB  
* Die Anzeige wurde über die (leider nicht mehr verfügbaren) Neo-Segments von crowdsupply realisiert : [https://www.crowdsupply.com/maksmakes/neosegment](https://www.crowdsupply.com/maksmakes/neosegment)

## Das Pinout des SCD30


![](SCD30_Pinout.png)


## Layout von Fritzing


### Breadboard

Die Neosegments gibt es leider nicht in Fritzing.   
Da die elektrische Verbindung genau gleich wie bei klassischen Neopixel-Sticks ist,   
wurden diese ersatzweise für den Stromlauf/Layout verwendet.

![](CO2_Ampel_Teensy_Neosegment_bb.png)

### Schematics

![](CO2_Ampel_Teensy_Neosegment_schem.png)


## Fliegender Aufbau

### Noch im grünen Bereich

![](ampel2.jpg)

### Grösser 800 ppm : Gelb

![](ampel1.jpg)

## Implementierungs-Details

* Es Wird alle 2 sek gemessen
* Vier Werte werden aufsummiert und daraus der Mittelwert gebildet
* Alle 8 Sekunden liegt ein neuer Mittelwert vor, der dann 8 sek lang angezeigt wird
* bis 799 ppm : Grün
* bis 999 ppm : Gelb
* bis 1199 ppm : Rot
* Grössr/gleich 1200 ppm : Rot blinkend
* Beim Einschalten liegt noch kein Wert vor: es wird 8888 in blau angezeigt

 

## Finaler Zusammenbau in Makerbox 

![](ampel3.jpg)

![](ampel4.jpg)

![](ampel5.jpg)

![](ampel6.jpg)

![](ampel7.jpg)
