/*

  CO2_Ampel_OLed.ino


  Built for Teensy LC, but might be changed rather easy to also run on other Arduino-Compatible Boards with support for I2C and NeoPixel (at least one 5V IO-Line)
  
  - Uses an SCD30 Sensirion-Sensor for sensing CO2.
  - Uses an Adafruit 128x32-Pixel 0.9 something OLED-Display for showing the exact values
  - Uses an Neo-Pixel-Ring with 16 RGB-Leds, each showing 100ppm.

  



  The OLED-Parts of the programm are derived from HelloWorld.ino from
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

#define U8X8_HAVE_HW_I2C
#include <Arduino.h>
#include <U8g2lib.h>
#include <Adafruit_NeoPixel.h>

#include <Wire.h>

#define PIN 17
#define AMOUNT_RGB_LEDS  16

Adafruit_NeoPixel strip = Adafruit_NeoPixel(AMOUNT_RGB_LEDS, PIN, NEO_GRB + NEO_KHZ800);


U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2( U8G2_R0, /* reset=*/20 , /* clock=*/ SCL, /* data=*/ SDA);


void setup(void) {
  u8g2.begin();
  u8g2.clearDisplay();    
  strip.begin();
  strip.setBrightness( 70 );
  strip.show();
}

int ppm = 0;
void loop(void) {
    static int color = 0;
    static int flash_counter = 0;
    bool flashing = false; 
    char myArray[20];
    String myString ;
    int max_led;  
    u8g2.clearBuffer();					// clear the internal memory
    u8g2.setFont(u8g2_font_fub25_tf);	// choose a suitable font
    myString = String(ppm) + " ppm";
    myString.toCharArray(myArray, 20);
    u8g2.drawStr(1,26,myArray );	// write something to the internal memory
    u8g2.sendBuffer();					// transfer internal memory to the display
    ppm += 100;
    if (ppm > 3000)
        ppm = 0;

    max_led = ppm / 100;
    flashing = false;
    if ( max_led >= 12 ){
        color = strip.Color(255, 0, 0) ; // Red
        flashing = true;
    }
    else if ( max_led >= 10 )
        color = strip.Color(255, 0, 0) ; // Red
    else if ( max_led >= 8 ) 
        color = strip.Color(255, 255, 0); // Yellow
    else 
        color = strip.Color(0, 255, 0) ; // green
        
    //strip.show();    
    //colorWipe( strip.Color(0, 0, 0), 0 ); 
    if( flashing ){
        flash_counter++;
        if( flash_counter == 2 )
            flash_counter = 0;
        if( flash_counter == 0 ){
            colorShow( color, 0, max_led); 
            delay(500);  
        }
        else{
            colorShow( 0, 0, max_led); 
            delay(500);  
        }
    }
    else {
        colorShow( color, 0, max_led); 
        delay(1000);  

    }
}


void colorShow(uint32_t c, uint8_t wait, uint32_t max_pixel) {
    if( max_pixel > strip.numPixels() ){
        max_pixel = strip.numPixels();
    } 
    for(uint16_t i=0; i<max_pixel ; i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
    }
    for(uint16_t i=max_pixel; i<strip.numPixels(); i++){
        strip.setPixelColor(i, 0);
        strip.show();
    }
}

void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}
