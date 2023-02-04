# CO2-Ampel am Raspberry-Pi

* Raspberry Pi
* USB-Ampel von Cleware
* CO2-Sensor SCD 30 
* Display tbd

## Software

* SCD30-I2C Python - SW : https://pypi.org/project/scd30-i2c
* USB-Ampel : https://github.com/jenszech/StatusLight
* bzw : 

## Other SCD30-Software

* Nutze inzwischen (co2-ampel-II.py) den Treiber-code von sensirion selbst

## Zweiter I2C-Bus

Moechte nun 2 (besser noch 3) SCD 30 Sensoren mal eine Woche an der frischen Luft laufen lassen um zu sehen, inwieweit sich die Messwerte angleichen.

Der Raspberry Pi hat zwei I2C-Busse eingebaut, der andere (I2C-Bus 0) muss jedoch erst per Config eingeschaltet werden.

Mehr Infos dazu : https://www.woodgears.ca/tech/i2c.html


