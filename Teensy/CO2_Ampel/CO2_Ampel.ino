/*
  C02-Ampel for Teensy LC,
  By Jogi Kuenstner
  Date : 05-Jan-2021
  License : MIT 

  Based upon: SCD30 Reading Example 
  By: Nathan Seidle
  SparkFun Electronics
  Date: May 22nd, 2018
  
  
  Hardware : Teensy LC, Sensirion SC30 Board, 2 pcs Neosegment allowing for 4 numbers 
  
*/

#include <Neosegment.h>
#include <i2c_t3.h>

/* attention: the library was patched to use i2c_t3 the i2c-Library for Teensy 3 / LC instead of wire , see also the provided patch */
#include "SparkFun_SCD30_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_SCD30

#define nDigits 4          // 4 Digits build from 2 Neosegments, allowing up to 9999 
#define PIN 17             // Teensy LC has 5V especially for Neopixel ONLY on pin 17 
#define BRIGHTNESS 100

#define NEO_COLOR_RED    0xFF0000
#define NEO_COLOR_YELLOW 0xFFFF00
#define NEO_COLOR_GREEN  0x00FF00


// instantiate the air-sensor
SCD30 airSensor;

// instantiate the Neosegments
Neosegment neosegment(nDigits, PIN, BRIGHTNESS);


void setup()
{
  delay( 1000 );

  Serial.begin(115200);
  Serial.println("CO2-Ampel");

  neosegment.begin();
  neosegment.clearAll();

  Wire.begin();
 
  if (airSensor.begin() == false)
  {

    neosegment.setDigit(0, 'e', NEO_COLOR_RED);
    neosegment.setDigit(1, 'r', NEO_COLOR_RED);
    neosegment.setDigit(2, 'r', NEO_COLOR_RED);
    Serial.println("Air sensor not detected. Please check wiring. Freezing...");
    while (1)
      ;
  }
}


int mean_array[4];
int mean_count = 0;
int mean_co2;

void ShowValueOnNeosegmentDisplay( int value, uint32_t color, bool showLeadingzeros )
{
  int digitBuffer;
  int digitIndex;

  if ( !showLeadingzeros ){
    // If value is below 1000, make sure the 0-th digit is off
    if(value  < 1000) {
      neosegment.clearDigit(0);
    }
    // If value is below 100, make sure also the 1-st digits is off
    if(value   < 100) {
      neosegment.clearDigit(1);
    }
    // If value is below 10, make sure also 2-nd digit ia off
    if(value     < 10) {
      neosegment.clearDigit(2);
    }
  }
  digitIndex = nDigits - 1;
  digitBuffer = value;    // Start with the complete number
      
  while (digitBuffer > 0)
  {
    int digit = digitBuffer % 10;
    // Write digit to Neosegment display in color that corresponds to the sensor reading
    neosegment.setDigit( digitIndex, digit,  color );

    digitBuffer /= 10;
    digitIndex--;
  }
}

void loop()
{
  
  uint16_t co2_data;
  int color;
  
  bool flashing = false;

  if (airSensor.dataAvailable())
  {
    Serial.print("co2(ppm):");
    co2_data = airSensor.getCO2();
    Serial.print( co2_data  );

    Serial.print(" temp(C):");
    Serial.print(airSensor.getTemperature(), 1);

    Serial.print(" humidity(%):");
    Serial.print(airSensor.getHumidity(), 1);

    Serial.println();

    mean_array[mean_count] = co2_data;
    mean_count++;
    if( mean_count == 4 )
    {
      mean_count = 0;
      mean_co2 = 0; 
      for ( int i = 0; i < 4; ++i )
      {
        mean_co2 += mean_array[i];
      }
      mean_co2 /= 4;
      Serial.print(" New mean value co2 :");
      Serial.println( mean_co2 );
      
    }

    flashing = false;
    
    if (mean_co2 >= 1200 ){
      color = 0xFF0000;
      flashing = true;
    }
    else if (mean_co2 >= 1000 )
      color = 0xFF0000;
    else if (mean_co2 >= 800 )
      color = 0xFFFF00;
    else 
      color = 0x00FF00; 
    // Display every digit from the sensor reading on appropriate Neosegment Digit

    ShowValueOnNeosegmentDisplay( mean_co2, color, false );
  }
  else
  {
    Serial.println("Waiting for new data");
    Wire.resetBus();
  }
  delay(1000);
  if( flashing == true )
  {
      neosegment.clearDigit(0);
      neosegment.clearDigit(1);
      neosegment.clearDigit(2);;
      neosegment.clearDigit(3);;
  }
  delay( 1000 );
}
