/*

  CO2_Ampel_OLed.ino


  Built for Teensy LC, but might be changed rather easy to also run on other Arduino-Compatible Boards with support for I2C and NeoPixel (at least one 5V IO-Line)
  
  - Uses an SCD30 Sensirion-Sensor for sensing CO2.
  - Uses an Adafruit 128x32-Pixel 0.9 something OLED-Display for showing the exact values
  - Uses an Neo-Pixel-Ring with 16 RGB-Leds, each showing 100ppm.
  - For brightness-setting it reads a BCD-rotary-Encoder with 0-9


  The OLED-Parts of the program are derived from HelloWorld.ino from
  --- snip ---
  Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)

  Copyright (c) 2016, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.
    
  * Redistributions in binary form must reproduce the above copyright notice, this 
    list of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  

  --- snap ---



 * Very simple schematics for the RGB-Neopixel: 
 * 
 * +5 V --------------------- LED +5v
 * GND  ----------------------LED GND
 * Pin 17 ------ 330 Ohm ---- LED Data In


*/

/* Features of the Hardware => Functionality in SW */
#define NEOPIXEL_STRIP_CONNECTED
// #define NEOSEGMENTS_CONNECTED
#define ADAFRUIT_OLED_CONNECTED
#define ROTARY_BCD_SWITCH_CONNECTED

#ifdef NEOPIXEL_STRIP_CONNECTED 
#ifdef NEOSEGMENTS_CONNECTED
#error "Either NEOPIXEL or NEOSEGMENTS, not both can be connected"
#endif /* NEOSEGMENTS_CONNECTED */
#endif /* NEOPIXEL_STRIP_CONNECTED  */

#ifdef NEOPIXEL_STRIP_CONNECTED 
#include <Adafruit_NeoPixel.h>
#endif /* NEOPIXEL_STRIP_CONNECTED */

#ifdef NEOSEGMENTS_CONNECTED
#include <Neosegment.h>
#error "not yet implemented in this sketch, please use first version of sketch in sibling directory"
#endif /* NEOSEGMENTS_CONNECTED */


#ifdef ADAFRUIT_OLED_CONNECTED
// when running on Teensy >= 3 or Teensy LC you need to set the define IN the Library
#include <U8g2lib.h>
#endif /* ADAFRUIT_OLED_CONNECTED */

// when running on Teensy >= 3 or Teensy LC you need to set the define IN the Library
#include "SparkFun_SCD30_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_SCD30


#define NEO_COLOR_RED    0xFF0000
#define NEO_COLOR_YELLOW 0xFFFF00
#define NEO_COLOR_GREEN  0x00FF00
#define NEO_COLOR_BLUE   0x0000FF

#define BRIGHTNESS_PER_DIGIT 15  // BCD-Rotary-Coder gives 0..9, Max Brightness is 255. 15 * 9 gives 135, which is really bright already


#define PIN 17
#define AMOUNT_RGB_LEDS  16

#ifdef NEOPIXEL_STRIP_CONNECTED 
Adafruit_NeoPixel strip = Adafruit_NeoPixel(AMOUNT_RGB_LEDS, PIN, NEO_GRB + NEO_KHZ800);
#endif /* NEOPIXEL_STRIP_CONNECTED */

#ifdef ADAFRUIT_OLED_CONNECTED
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2( U8G2_R0, /* reset=*/20 , /* clock=*/ SCL, /* data=*/ SDA);
#endif /* ADAFRUIT_OLED_CONNECTED */

SCD30 airSensor;


void setup(void) {

    /* The BCD rotary switch */
    RotaryBCD_Init();

    delay( 1000 );
    /* Debugging */
    Serial.begin(115200);
    Serial.println("CO2-Ampel");

#ifndef ADAFRUIT_OLED_CONNECTED     // This makes Wire.begin itself and two times calling is conflicting, why ever ...
    Wire.begin();
#endif /* not ADAFRUIT_OLED_CONNECTED */
    
    /* The OLED-Display */
#ifdef  ADAFRUIT_OLED_CONNECTED
    u8g2.begin();
    u8g2.clearDisplay();  
#endif /* ADAFRUIT_OLED_CONNECTED */

    /* The NEO-Pixel-Ring */
#ifdef NEOPIXEL_STRIP_CONNECTED 
    strip.begin();
    strip.setBrightness( RotaryBCD_GetVal() * BRIGHTNESS_PER_DIGIT );
    strip.show();
#endif /* NEOPIXEL_STRIP_CONNECTED */

    /* The SCD30-Air-Sensor */
    ShowSmallOnOLED("Sensor Initialise ...", 0 );
    if (airSensor.begin() == false) 
    {
        Serial.println("Air sensor not detected. Please check wiring. Freezing...");
        ShowBigOnOLED("Error 1");
        showErrorOnStrip();
        while (1)
            ;
    }
    ShowSmallOnOLED("Sensor Reading ...", 1 );
}

void loop(void) {
    static int color = 0;
    bool flashing = false; 
    static int mean_ppm = 0;
    
    if ( airSensor.dataAvailable()) {
        SerialDebugSensorVals( );
        int curr_ppm = airSensor.getCO2();  
        mean_ppm = CalcNewMeanVal( curr_ppm );
    }
    flashing = CalcFlashingAndColor( &color, mean_ppm) ;
    if ( mean_ppm != 8888 ){    
        String myString = String(mean_ppm) + " ppm";
        ShowBigOnOLEDWithFlashing( myString , flashing );
    }
    ShowValueAndFlashOnNeoStripesOrSegments( color, flashing, mean_ppm );
    delay(100);  
}


int last_mean_val = 0;
int last_millis;
void ShowValueAndFlashOnNeoStripesOrSegments( int color, bool flashing, int mean_val ){
    int max_led;
    max_led = mean_val / 100;

    if (last_mean_val != mean_val ){
        last_mean_val = mean_val;
        last_millis = millis();
        colorShow( color, 0, max_led); 
    }
    else if( flashing ){
        int curr_millis = millis();
        int delta_millis = curr_millis - last_millis;
        if( delta_millis > 1000 ){
            last_millis = millis();
            colorShow( color, 0, max_led); 
        }
        else if (delta_millis > 500 ){
            colorShow( 0, 0, max_led); 
        }    
    }
}


/**********************************/
/* Color and Flashing-Calculation */
/**********************************/
bool CalcFlashingAndColor( int *color, int ppm_in ){ 
    bool flashing = false;

    if( ppm_in == 8888 ){  // special handling when starting, value is 8888
        *color =  NEO_COLOR_BLUE;
        return false;
    }
    if( ppm_in  >= 1200 ){
        *color = NEO_COLOR_RED;
        flashing = true;
    }
    else if (ppm_in >= 1000 ){
        *color = NEO_COLOR_RED;
        flashing = false;
    }
    else if (ppm_in >= 800 ){
        *color = NEO_COLOR_YELLOW;
        flashing = false;
    }
    else { 
        *color = NEO_COLOR_GREEN; 
        flashing = false;
    }
    return flashing;
}

/**************************/
/* Mean-Value Calc        */
/**************************/
int  mean_val = 8888;
long new_mean_val = 0;
int  mean_count = 0;

int CalcNewMeanVal( int NewVal ){
    new_mean_val += NewVal;
    mean_count++;
    if( mean_count == 4 ){
        mean_val = new_mean_val / 4; 
        mean_count = 0;
        new_mean_val = 0;
    }
    return mean_val;
}


/**************************/
/* The Rotary BCD Encoder */
/**************************/
/* Init */
void RotaryBCD_Init( void ){
    pinMode(0, INPUT_PULLUP);
    pinMode(1, INPUT_PULLUP);
    pinMode(2, INPUT_PULLUP);
    pinMode(3, INPUT_PULLUP);

}

/* Get Value */
int RotaryBCD_GetVal( void ){
    int Value = (!digitalRead(3)) * 8  +
                (!digitalRead(2)) * 4 +
                (!digitalRead(1)) * 2 +
                (!digitalRead(0)) ;
    return Value;
}


/**************************/
/* The OLED-Display       */
/**************************/

/* Show small strings on the OLED-Display */
void ShowSmallOnOLED( String What, int Line ){
#ifdef ADAFRUIT_OLED_CONNECTED
    char myArray[40];
    if( Line == 0 )
        u8g2.clearBuffer();                    // clear the internal memory
    u8g2.setFont(u8g2_font_5x8_tf);   // choose a suitable font
    What.toCharArray(myArray, 40);
    if( Line == 0 )
        u8g2.drawStr(5,10,myArray );    // write something to the internal memory
    else if (Line == 1)
        u8g2.drawStr(5,20,myArray );    // write something to the internal memory
    else 
        u8g2.drawStr(5,30,myArray );    // write something to the internal memory
    u8g2.sendBuffer();                  // transfer internal memory to the display
#endif /* ADAFRUIT_OLED_CONNECTED */

}

void ShowBigOnOLED( String What ){
#ifdef ADAFRUIT_OLED_CONNECTED
    char myArray[20];
    u8g2.clearBuffer();                    // clear the internal memory
    u8g2.setFont(u8g2_font_fub25_tf);   // choose a suitable font
    What.toCharArray(myArray, 20);
    u8g2.drawStr(1,26,myArray );    // write something to the internal memory
    u8g2.sendBuffer();                  // transfer internal memory to the display
#endif /* ADAFRUIT_OLED_CONNECTED */

}




int last_millis_oled=0;
/* Show one big string on the OLED-Display */
void ShowBigOnOLEDWithFlashing( String What , bool flashing ){
   
    if( flashing ){
        int curr_millis_oled = millis();
        int akt_delta = curr_millis_oled  - last_millis_oled;
        if ( akt_delta > 1000 ){
            last_millis_oled = millis();
            ShowBigOnOLED( What );
        }
        else if ( akt_delta > 500 ){
            ShowBigOnOLED( "          " );       
        }
    }
    else {
        ShowBigOnOLED( What );
    }
}


/**************************/
/* The NEO-Pixel-Stripe   */
/**************************/

/* Shows in the given color, up to given LED, can animate LED by LED with the given delay  */ 
void colorShow(uint32_t c, uint8_t wait, uint32_t max_pixel) {
#ifdef NEOPIXEL_STRIP_CONNECTED 
    if( max_pixel > strip.numPixels() ){
        max_pixel = strip.numPixels();
    } 
    strip.setBrightness( RotaryBCD_GetVal() * BRIGHTNESS_PER_DIGIT );         
    for(uint16_t i=0; i<max_pixel ; i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
    }
    for(uint16_t i=max_pixel; i<strip.numPixels(); i++){
        strip.setPixelColor(i, 0);
        strip.show();
    }
#endif /* NEOPIXEL_STRIP_CONNECTED */

}

/* Shows in the given color, all LEDs, can animate LED by LED with the given delay */ 
void colorWipe(uint32_t c, uint8_t wait) {
#ifdef NEOPIXEL_STRIP_CONNECTED 
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
#endif /* NEOPIXEL_STRIP_CONNECTED */
}

/* Shows every second LED in RED to indicate an error */ 
void showErrorOnStrip( void ) {
#ifdef NEOPIXEL_STRIP_CONNECTED 
    for(uint16_t i=0; i<strip.numPixels(); i++) {
        if( i % 2)
            strip.setPixelColor(i, NEO_COLOR_RED );
        else
            strip.setPixelColor(i, 0 );
        strip.show();
  }
#endif /* NEOPIXEL_STRIP_CONNECTED */
}


/**************************/
/* Serial Debugging       */
/**************************/
/* Prints the values of the AIR-Sensor via serial  */ 
void SerialDebugSensorVals( void ){
    Serial.print("co2(ppm):");
    Serial.print( airSensor.getCO2());

    Serial.print(" temp(C):");
    Serial.print(airSensor.getTemperature(), 1);

    Serial.print(" humidity(%):");
    Serial.print(airSensor.getHumidity(), 1);

    Serial.println();
}
