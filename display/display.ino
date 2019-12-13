/*
 * Max Knutson, Patrick Hultquist, Ajay Jayaraman
 * 12-13-19
 * Microcontroller Applications
 * Semester Project: Counting display Arduino
 * 
 * Run this code using an Adafruit display and connect
 * the Arduino to the controlling "Master" Arduino to
 * display the amount of Skittles counted in each color
 * by the machine
 */


// Seesaw test includes/globals ///////////////////////////////
#include <SPI.h>
#include <SD.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <Adafruit_seesaw.h>
#include <Adafruit_TFTShield18.h>
Adafruit_TFTShield18 ss;
// TFT display and SD card will share the hardware SPI interface.
// Hardware SPI pins are specific to the Arduino board type and
// cannot be remapped to alternate pins.  For Arduino Uno,
// Duemilanove, etc., pin 11 = MOSI, pin 12 = MISO, pin 13 = SCK.
#define SD_CS    4  // Chip select line for SD card on Shield
#define TFT_CS  10  // Chip select line for TFT display on Shield
#define TFT_DC   8  // Data/command line for TFT on Shield
#define TFT_RST  -1  // Reset line for TFT is handled by seesaw!
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
////////////////////////////////////////////////////////////////

// Wire include
#include<Wire.h>

// Specific includes
int redCount = 0;
int orangeCount = 0;
int yellowCount = 0;
int greenCount = 0;
int purpleCount = 0;

// Code for current color
int color;

void setup() {
  // Wire initialization
  Wire.begin(9);
  Wire.onReceive(receiveEvent);
  Serial.begin(9600);

  // Adafruit initialization, from Seesaw test code ////////////////////////////////
  // start by disabling both SD and TFT
  pinMode(TFT_CS, OUTPUT);
  digitalWrite(TFT_CS, HIGH);
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);

  // Start seesaw helper chip
  if (!ss.begin()){
    Serial.println("seesaw could not be initialized!");
    while(1);
  }
  Serial.println("seesaw started");
  Serial.print("Version: "); Serial.println(ss.getVersion(), HEX);

  // Start set the backlight off
  ss.setBacklight(TFTSHIELD_BACKLIGHT_OFF);
  // Reset the TFT
  ss.tftReset();

  // Initialize 1.8" TFT
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab

  Serial.println("TFT OK!");
  tft.fillScreen(ST77XX_CYAN);

  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("failed!");
  } else {
    Serial.println("OK!");
    File root = SD.open("/");

    printDirectory(root, 0);
    root.close();
  }

  // Set backlight on fully
  // ss.setBacklight(TFTSHIELD_BACKLIGHT_ON);
  // Or you can set the backlight one third on
  // ss.setBacklight(TFTSHIELD_BACKLIGHT_ON / 3);
  // Or dim it up
  for (int32_t i=TFTSHIELD_BACKLIGHT_OFF; i<TFTSHIELD_BACKLIGHT_ON; i+=100) {
    ss.setBacklight(i);
    delay(1);
  }
  //////////////////////////////////////////////////////////////////////////

  // Initializing display
  tft.fillScreen(ST77XX_BLACK);
  drawLabels();
  drawCount("+");
}

/* 
 * receiveEvent replaces the loop function and runs each time the display recieves
 * a signal from the controlling Arduino. It updates count numbers and 
 * writes them to the display.
 */
void receiveEvent(int bytes) 
{
  // Reads
  color = Wire.read();
  switch (color) 
  {
  /*  Colors are sent as codes by the signalling "Master" Arduino:
   *  Red: 1
   *  Orange: 2
   *  Yellow: 3
   *  Green: 4
   *  Purple (magenta): 5
   *  
   *  Commented out fillScreens can be used for testing
   */
    case 1:
      //tft.fillScreen(ST77XX_RED);
      redCount++;
      break;
    case 2:
      //tft.fillScreen(ST77XX_ORANGE);
      orangeCount++;
      break;
    case 3:
      //tft.fillScreen(ST77XX_YELLOW);
      yellowCount++;
      break;
    case 4:
      //tft.fillScreen(ST77XX_GREEN);
      greenCount++;
      break;
    case 5:
      //tft.fillScreen(ST77XX_MAGENTA);
      purpleCount++;
      break;
    // Default case represents if there is no Skittle in the machine
    default:
      break;
  }

  // Refreshing the screen
  tft.fillScreen(ST77XX_BLACK);
  drawLabels();
  drawCount("+");
  
  /* Testing
  Serial.print("Red: ");
  Serial.println(redCount);
  Serial.print("Orange: ");
  Serial.println(orangeCount);
  Serial.print("Yellow: ");
  Serial.println(yellowCount);
  Serial.print("Green: ");
  Serial.println(greenCount);
  Serial.print("Purple: ");
  Serial.println(purpleCount);
  Serial.println();
  delay(40); 
  */
  
}

// No code is run from the loop function, as reciveEvent continously
// updates the display when the Arduino recieves a signal. 
void loop() 
{
  
}

// From seesaw test code
void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}

// Draws the Red, Orn, Ylw etc. labels in their specific color
void drawLabels()
{
  tft.setTextSize(3);
  tft.setTextColor(ST77XX_RED);
  tft.setCursor(0, 10);
  tft.print("Red:");
  tft.setTextColor(ST77XX_ORANGE);
  tft.setCursor(0, 40);
  tft.print("Orn:");
  tft.setTextColor(ST77XX_YELLOW);
  tft.setCursor(0, 70);
  tft.print("Ylw:");
  tft.setTextColor(ST77XX_GREEN);
  tft.setCursor(0, 100);
  tft.print("Grn:");
  tft.setTextColor(ST77XX_MAGENTA);
  tft.setCursor(0, 130);
  tft.print("Prp:");
}

/* Draws the quantity of each color of Skittle on the screen
 * indicator: String representing the indication you would like
 * to use to show the most recently added to color 
*/
void drawCount(String indicator)
{
  // Initializing number text
  tft.setTextSize(3);
  tft.setTextColor(ST77XX_WHITE);

  // Red Skittle amount
  tft.setCursor(75, 10);
  if (redCount <= 99) tft.print(redCount);
  else tft.print("99");
  if (color == 1) tft.print(indicator);

  // Orange Skittle amount
  tft.setCursor(75 , 40);
  if (orangeCount <= 99) tft.print(orangeCount);
  else tft.print("99");
  if (color == 2) tft.print(indicator);

  // Yellow Skittle amount
  tft.setCursor(75 , 70);
  if (yellowCount <= 99) tft.print(yellowCount);
  else tft.print("99");
  if (color == 3) tft.print(indicator);

  // Green Skittle amount
  tft.setCursor(75 , 100);
  if (greenCount <= 99) tft.print(greenCount);
  else tft.print("99");
  if (color == 4) tft.print(indicator);

  // Purple Skittle amount
  tft.setCursor(75, 130);
  if (purpleCount <= 99) tft.print(purpleCount);
  else tft.print("99");
  if (color == 5) tft.print(indicator);
}
