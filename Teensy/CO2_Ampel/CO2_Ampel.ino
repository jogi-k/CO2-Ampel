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
#define NEO_COLOR_BLUE   0x0000FF


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

// Attention, the NeoSegment starts with the highest digit, 
// meaning (in our case with 4 digits):
// neosegement.setDigit( 0 ...) adresses the 1000s
// neosegement.setDigit( 1 ...) adresses the 100s
// neosegement.setDigit( 2 ...) adresses the 10s
// and so on
// therefore we start with the highest neo-digit and write to it the modulo-10-result which is the lowest digit...

void ShowValueOnNeosegmentDisplay( int value, uint32_t color, bool showLeadingzeros )
{
  int digitBuffer;
  int digitIndex;

  digitIndex = nDigits ;
  digitBuffer = value;    // Start with the complete number
      
  while (digitBuffer > 0)  {
    int digit = digitBuffer % 10;
    // Write digit to Neosegment display in given color 
    neosegment.setDigit( digitIndex -1, digit,  color );

    digitBuffer /= 10;
    digitIndex--;
  }
  while ( digitIndex ) {
    if ( showLeadingzeros ){
      neosegment.setDigit( digitIndex -1, 0,  color );
    }
    else{
      neosegment.clearDigit(digitIndex -1 );
    }
    digitIndex--;
  }
}


int color = NEO_COLOR_BLUE ;
bool flashing = false;
int mean_co2 = 8888;
long new_mean_val = 0;
int mean_count = 0;



void loop()
{
  
  uint16_t co2_data;
  

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

    new_mean_val += co2_data;
    mean_count++;
    if( mean_count == 4 )
    {
      mean_co2 = new_mean_val / 4; 
      mean_count = 0;
      new_mean_val = 0;
      Serial.print(" New mean value co2 :");
      Serial.println( mean_co2 );
      
      flashing = false;
    
      if (mean_co2 >= 1200 ){
        color = NEO_COLOR_RED;
        flashing = true;
      }
      else if (mean_co2 >= 1000 )
        color = NEO_COLOR_RED;
      else if (mean_co2 >= 800 )
        color = NEO_COLOR_YELLOW;
      else 
        color = NEO_COLOR_GREEN; 
     }
  }
  else
  {
    Serial.println("Waiting for new data");
    Wire.resetBus();
  }
  ShowValueOnNeosegmentDisplay( mean_co2, color, false );
  if (flashing )
  {
    delay(500);
    neosegment.clearAll();  
    delay(500);
    ShowValueOnNeosegmentDisplay( mean_co2, color, false );
    delay(500);
    neosegment.clearAll();  
    delay(500);
  }
  else
  {
    delay(2000);
  }
}
