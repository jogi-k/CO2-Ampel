#include <Adafruit_GFX.h> // include the base Adafruit Graphics library
#include <EPD215.h>       // include the epaper display driver library
#include <Wire.h>         // include the temperature sensor protocol driver library

#define TMP102_I2C_ADDRESS 0x49 // I2C address for our temperature chip

#define MAX_WIDTH    208
#define MAX_HEIGHT   112

#define MAX_XVAL 60 // min
#define X_SEGM   6
#define MIN_YVAL 400  // ° C
#define MAX_YVAL 2000  // ° C
#define Y_SEGM   4


// initialize epaper with pin numbers for cs, dc, rs, bs for hardware SPI
EPD215 epaper( 17, 16, 15, 14 );

float temperature;
 
const int originX = 30;
const int originY = 90;
const int sizeX = 180;
const int sizeY = 80;

int start_x = 0;
 
void getTemperature() { //get temperature from temperature sensor
  Wire.beginTransmission( TMP102_I2C_ADDRESS ); // for more details on how to read the temperature from this sensor
  Wire.write( 0x00 );                           // please refer to the device datasheet: 
  Wire.endTransmission();                       // http://www.ti.com.cn/cn/lit/ds/symlink/tmp102.pdf
  Wire.requestFrom( TMP102_I2C_ADDRESS, 2 );
  Wire.endTransmission();
  int val = Wire.read() << 4;
  val |= Wire.read() >> 4;
  temperature = ( val * 0.0625 ) - 1; // Celsius

}

String createTemperatureString( float t ) { // convert temperature value to String
  String s;
  if ( t > 100 ) s = String( t / 100 );
  s += String(int( t / 10 ) % 10 );
  s += String(int( t ) % 10 );
  s += ".";
  s += String(int( t * 10 ) % 10 );
  return s;
}

void drawAxes()
{
  
  // draw axis iself 
  epaper.drawLine( originX, originY, (originX + sizeX), originY, 1 );
  epaper.drawLine((originX + sizeX), originY, (originX + sizeX - 8), originY - 3,  1 );
  epaper.drawLine((originX + sizeX), originY, (originX + sizeX - 8), originY + 3,  1 );
  
  epaper.drawLine( originX, originY, originX, (originY - sizeY), 1 );
  epaper.drawLine( originX, (originY - sizeY), originX - 3, (originY - sizeY + 8), 1 );
  epaper.drawLine( originX, (originY - sizeY), originX + 3, (originY - sizeY + 8), 1 );

  // draw xlables
  for(int i = 0; i < X_SEGM; i++)
  {
    int x_coord = originX + i * sizeX / X_SEGM ;
    int x_val =  start_x + i * MAX_XVAL / X_SEGM ;
    
    epaper.drawLine( x_coord, originY-1, x_coord, originY + 5, 1 );
    epaper.setCursor( x_coord - 5 , originY + 9 );
    epaper.setTextSize( 1 );
    epaper.setTextWrap( false );
    epaper.println( x_val );
  }

  // draw ylabels
  for(int i = 0; i < Y_SEGM; i++)
  {
    int y_coord = originY - i * sizeY / Y_SEGM ;
    int y_val = MIN_YVAL + i * ( MAX_YVAL - MIN_YVAL ) / Y_SEGM;
      
    epaper.drawLine( originX + 1, y_coord, originX -5 , y_coord, 1);
    epaper.setCursor( 2 , y_coord - 4 );
    epaper.setTextSize( 1 );
    epaper.setTextWrap( false );
    epaper.println( y_val );
  }

# ifdef HUGO
  // draw number values
  for(int i = 0; i < 6; i++)
  {
    tft.setCursor((minorSizeX - 30), (number[i] + numberSize));
    tft.setTextColor(graphColor);
    tft.setTextSize(1);
    tft.println(val[i]);
  }
#endif

}



void setup() {                   // setup function runs once on startup
  Wire.begin();                  // start the I2C driver
  epaper.initScreen();           // initialize the epaper hardware
  epaper.setRotation( 1 );
  epaper.updateScreen();         // update the screen with whatever is in the buffer
  getTemperature();              // get the latest temperature
}

void loop() {                    // loop function runs forever until power is removed
  getTemperature();
  epaper.clearScreen();          // clear the screen buffer
  epaper.setCursor( 110, 0 );      // reset the cursor position
  String temp = "T: "  + createTemperatureString( temperature ) + "C" ; // write the current temperature to the epaper screen buffer   
  epaper.setTextSize(2 );
  epaper.println( temp ) ;
  drawAxes();
  epaper.updateScreen( temperature );                    // update the epaper screen with the buffer contents, passing the current temperature to the epaper screen
    
  delay( 5000 );                // wait 30 seconds before taking the next measurement
}

#ifdef HUGO
  int valueBlock[500];
  int timeBlock[500];
  int locationBlock[500];
  int valuePos;
  int blockPos;
  int temp;
  int chk;
  
// Editable Variables
  bool proDebug = 0;

  uint16_t graphColor = BLUE;
  uint16_t pointColor = BLACK;
  uint16_t lineColor = GREEN;

  String graphName = "Time Graph";

  int graphRange = 50;
  int markSize = 3;
  
// Calculate Values
  const int numberOfMarks = 8;
  const int originX = 45;
  const int originY = 200;
  const int sizeX = 270;
  const int sizeY = 150;
  const int deviation = 30;
  
  int boxSize = (sizeX / numberOfMarks);
  int mark[] = {(boxSize + deviation), ((boxSize * 2) + deviation), ((boxSize * 3) + deviation), ((boxSize * 4) + deviation), ((boxSize * 5) + deviation), ((boxSize * 6) + deviation), ((boxSize * 7) + deviation), ((boxSize * 8) + deviation)};

  const int minorSizeY = (originY + 10);
  const int minorSizeX = (originX - 10);

  int numberSize = (sizeY / 6);
  int number[] = {numberSize, (numberSize * 2), (numberSize * 3), (numberSize * 4), (numberSize * 5), (numberSize * 6)};

  int numberValue = (graphRange / 6);
  int val[] = {graphRange, (numberValue * 5), (numberValue * 4), (numberValue * 3), (numberValue * 2), numberValue};



void graph()
{
  chk = DHT.read11(22);
  temp = (DHT.temperature);
  timeBlock[valuePos] = ((millis() - 4500) / 1000);

  valueBlock[valuePos] = temp;
  
  if(proDebug)
  {
    Serial.println(timeBlock[valuePos]);
  }
  
  if(blockPos < 8)
  {
    // print the time
    tft.setCursor((mark[valuePos] - 5), (originY + 16));
    tft.setTextColor(graphColor, WHITE);
    tft.setTextSize(1);
    tft.println(timeBlock[valuePos]);
    
    // map the value
    locationBlock[valuePos] = map(temp, 0, graphRange, originY, (originY - sizeY));

    // draw point
    tft.fillRect((mark[valuePos] - 1), (locationBlock[valuePos] - 1), markSize, markSize, pointColor);

    // try connecting to previous point
    if(valuePos != 0)
    {
      tft.drawLine(mark[valuePos], locationBlock[valuePos], mark[(valuePos - 1)], locationBlock[(valuePos - 1)], lineColor);
    }

    blockPos++;
  }
  else
  {
    // clear the graph's canvas
    tft.fillRect((originX + 2), (originY - sizeY), sizeX, sizeY, WHITE);

    // map the value - current point
    locationBlock[valuePos] = map(temp, 0, graphRange, originY, (originY - sizeY));

    // draw point - current point
    tft.fillRect((mark[7]), (locationBlock[valuePos] - 1), markSize, markSize, pointColor);

    // draw all points
    for(int i = 0; i < 8; i++)
    {
      tft.fillRect((mark[(blockPos - (i + 1))] - 1), (locationBlock[(valuePos - i)] - 1), markSize, markSize, pointColor);
    }

    // draw all the lines
    for(int i = 0; i < 7; i++)
    {
      tft.drawLine(mark[blockPos - (i + 1)], locationBlock[valuePos - i], mark[blockPos - (i + 2)], locationBlock[valuePos - (i + 1)], lineColor);
    }
    
    // change time lables
    for(int i = 0; i <= 7; i++)
    {
      tft.setCursor((mark[(7 - i)] - 5), (originY + 16));
      tft.setTextColor(graphColor, WHITE);
      tft.setTextSize(1);
      tft.println(timeBlock[valuePos - i]);
    }
  }

  valuePos++;
}

void setup()
{
  if(proDebug)
  {
    Serial.begin(9600);
    while(!Serial) {};
  }
  
  tft.reset();
  delay(500);
  uint16_t identifier = tft.readID();
  identifier=0x9341;

  tft.begin(identifier);
  tft.setRotation(1);

  drawHome();
  drawGraph();
}

void loop()
{
  graph();
  delay(6000);
}




#endif
