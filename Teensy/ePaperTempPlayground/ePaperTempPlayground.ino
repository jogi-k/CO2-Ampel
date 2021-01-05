#include <Adafruit_GFX.h> // include the base Adafruit Graphics library
#include <EPD215.h>       // include the epaper display driver library
#include <Wire.h>         // include the temperature sensor protocol driver library

#define TMP102_I2C_ADDRESS 0x49 // I2C address for our temperature chip

// The pyhsical screen size in pixel

#define MAX_WIDTH    208
#define MAX_HEIGHT   112


// The values for the graph, the real values of the physical world

#define GRAPH_TIME_SECS 48 // what is the overall time which is visible on the graph? time in seconds 
#define TIME_SCALE_MARKS   6  // how many scale-marks do we want to have on the time-axis
#define AMOUNT_VALUES   10 // how many values do we want to have in the graph overall ?
#define YVAL_AT_ORIGIN 10  // ° C
#define YVAL_MAX 30  // ° C
#define YVAL_SCALE_MARKS   4   // how many scale-marks do we want to have on the Y-Axis

// The values for the graph-coordinates on the LCD / ePaper
 
const int origin_X_coord = 25;
const int origin_Y_coord = 90;
const int size_X_pixel = 180;
const int size_Y_pixel = 80;
const int delay_time = ( GRAPH_TIME_SECS * 1000 ) / AMOUNT_VALUES;


int x_vals[size_X_pixel]; // we will keep a buffer for the max amount of pixels that can be shown, we also will store ints instead of floats
int y_vals[size_X_pixel];  

float temperature;


int start_x = 0;   // this value will float with the time passing by ...

// initialize epaper with pin numbers for cs, dc, rs, bs for hardware SPI
EPD215 epaper( 17, 16, 15, 14 );



float getTemperature() { //get temperature from temperature sensor
  Wire.beginTransmission( TMP102_I2C_ADDRESS ); // for more details on how to read the temperature from this sensor
  Wire.write( 0x00 );                           // please refer to the device datasheet: 
  Wire.endTransmission();                       // http://www.ti.com.cn/cn/lit/ds/symlink/tmp102.pdf
  Wire.requestFrom( TMP102_I2C_ADDRESS, 2 );
  Wire.endTransmission();
  int val = Wire.read() << 4;
  val |= Wire.read() >> 4;
  val = ( val * 0.0625 ) ; // Celsius
  return val;

}

String createTemperatureString( float t ) { // convert temperature value to String
  return String( t, 1 );
}


float map_float(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


void drawAxes()
{
  
  // draw axis iself 
  epaper.drawLine( origin_X_coord, origin_Y_coord, (origin_X_coord + size_X_pixel), origin_Y_coord, 1 );
  epaper.drawLine((origin_X_coord + size_X_pixel), origin_Y_coord, (origin_X_coord + size_X_pixel - 8), origin_Y_coord - 3,  1 );
  epaper.drawLine((origin_X_coord + size_X_pixel), origin_Y_coord, (origin_X_coord + size_X_pixel - 8), origin_Y_coord + 3,  1 );
  
  epaper.drawLine( origin_X_coord, origin_Y_coord, origin_X_coord, (origin_Y_coord - size_Y_pixel), 1 );
  epaper.drawLine( origin_X_coord, (origin_Y_coord - size_Y_pixel), origin_X_coord - 3, (origin_Y_coord - size_Y_pixel + 8), 1 );
  epaper.drawLine( origin_X_coord, (origin_Y_coord - size_Y_pixel), origin_X_coord + 3, (origin_Y_coord - size_Y_pixel + 8), 1 );

  // draw xlables
  for(int i = 0; i < TIME_SCALE_MARKS; i++)
  {
    int x_coord = origin_X_coord + i * size_X_pixel / TIME_SCALE_MARKS ;
    int x_val =  start_x + i * GRAPH_TIME_SECS / TIME_SCALE_MARKS ;
    
    epaper.drawLine( x_coord, origin_Y_coord-1, x_coord, origin_Y_coord + 5, 1 );
    epaper.setCursor( x_coord - 5 , origin_Y_coord + 9 );
    epaper.setTextSize( 1 );
    epaper.setTextWrap( false );
    epaper.println( x_val );
  }

  // draw ylabels
  for(int i = 0; i < YVAL_SCALE_MARKS; i++)
  {
    int y_coord = origin_Y_coord - i * size_Y_pixel / YVAL_SCALE_MARKS ;
    int y_val = YVAL_AT_ORIGIN + i * ( YVAL_MAX - YVAL_AT_ORIGIN ) / YVAL_SCALE_MARKS;
      
    epaper.drawLine( origin_X_coord + 1, y_coord, origin_X_coord -5 , y_coord, 1);
    epaper.setCursor( 2 , y_coord - 4 );
    epaper.setTextSize( 1 );
    epaper.setTextWrap( false );
    epaper.println( y_val );
  }
}



void setup() {                   // setup function runs once on startup
  Wire.begin();                  // start the I2C driver
  epaper.initScreen();           // initialize the epaper hardware
  epaper.setRotation( 1 );
  epaper.updateScreen();         // update the screen with whatever is in the buffer
  temperature = getTemperature();              // get the latest temperature
}

void loop() {                    // loop function runs forever until power is removed
  temperature = getTemperature();
  epaper.clearScreen();          // clear the screen buffer
  epaper.setCursor( 110, 0 );      // reset the cursor position
  String temp = "T: "  + createTemperatureString( temperature ) + "C" ; // write the current temperature to the epaper screen buffer   
  epaper.setTextSize(2 );
  epaper.println( temp ) ;
  fillGraph( temperature );
  drawAxes();
  drawGraph();
  epaper.updateScreen( temperature );                    // update the epaper screen with the buffer contents, passing the current temperature to the epaper screen
    
  delay( delay_time );                // wait the calculated amount of time 
}

int overall_counter = 0; 
int write_curr_ptr = 0;
int read_curr_ptr = 0;


void drawGraph( void )
{
  int temp_read_counter = read_curr_ptr + 1;
  if ( overall_counter < 2 )
    return;
  while(  temp_read_counter <  write_curr_ptr ){
    epaper.drawLine( x_vals[temp_read_counter -1] + origin_X_coord, y_vals[temp_read_counter -1],  x_vals[temp_read_counter ] + origin_X_coord, y_vals[temp_read_counter ] , 1);
    temp_read_counter++;
    
  }
  
    
}


void fillGraph( float currtemp)
{
  int y_val;
  int temp_val;
  x_vals[write_curr_ptr] = overall_counter;
  
  y_val = map_float( currtemp, YVAL_AT_ORIGIN, YVAL_MAX, origin_Y_coord , origin_Y_coord -  size_Y_pixel );
  y_vals[write_curr_ptr] = y_val;
  write_curr_ptr++ ;
  overall_counter++;  
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
