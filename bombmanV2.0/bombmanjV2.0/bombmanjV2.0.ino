#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_STMPE610.h>
#include <ArduinoNunchuk.h>
#include <gfxfont.h>
#include <SD.h>
#include <SPI.h>

#define BUFFPIXEL 80
#define MAXPRESSURE 1000
#define MINPRESSURE 10
#define SD_CS 4
#define STMPE_CS 8        // The STMPE610 uses hardware SPI on the shield, and #8
#define TFT_DC 9
#define TFT_CS 10
#define TS_MINX 150       // This is calibration data for the raw touch data to the screen coordinates
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000

//defines voor kleuren
#define BLACK     0x0000
#define BLUE      0x001F
#define CYAN      0x07FF
#define GREEN     0x07E0
#define MAGENTA   0xF81F
#define RED       0xF800
#define YELLOW    0xFFE0

//aanmaken objecten voor gebruik dan de lirbary's
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
Adafruit_STMPE610 ts = Adafruit_STMPE610(STMPE_CS);
ArduinoNunchuk nunchuk = ArduinoNunchuk(); //initialize nunchuck

uint8_t         spi_save;

bool rdy_1 = 0;
bool rdy_2 = 1;
boolean live = true;
boolean go = false;
boolean boem = false;
boolean bombdown = false;


uint8_t x = 40;
uint8_t y = 30;

int bx;
int by;
int brow ;
int bcolumn;

uint8_t row = 1;
uint8_t column = 1;

unsigned long startMillis;
unsigned long bombtimer;


// |rows  >column
uint8_t grid[ 11  ][ 15 ] = {
  // [11][15] zijn de max bytes maar bij het aanroepen begin je wel bij 0
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},	//0=air
  //1=wall
  {1, 0, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 0, 1},	//2=crate
  {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},	//3=bomb
  {1, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 1},	//4=explosion
  {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
  {1, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 1},
  {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
  {1, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 1},
  {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
  {1, 0, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 0, 1},
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};


enum scherm {Home, Start, Join, Map };					//benoemen verschillende schermen
typedef enum scherm scherm_t;
scherm_t scherm;



//functie declaraties
void bmpDraw(char *filename, int16_t x, int16_t y);
void map();
uint16_t read16(File &f);
uint32_t read32(File &f);

int main() {
  Serial.begin(9600);
  tft.begin();

  yield();

  //Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    //Serial.println("failed!");
  }
  //Serial.println("OK!");


  tft.setRotation(1);

  if (!ts.begin()) {
    //		  Serial.println("Couldn't start touchscreen controller");
    while (1);
  }
  //	  Serial.println("Touchscreen started");



  //power for nunchuck
  DDRC |= (1 << PORTC2); //set pin A2 as output
  DDRC |= (1 << PORTC3); //set pin A3 as output
  PORTC |= (1 << PORTC3); //set pin A3 high
  delay(100); //wait for signal to stabalize
  nunchuk.init(); //init the nunschuck


  //bmpDraw("/map.bmp",							//map tekenen
  //(tft.width() - 320 ),
  //(tft.height() - 220 ));

  ///* Het tekenen van de crate's */
  //for (int i = 0; i < 11; i++ ) {
  //for (int j = 0; j < 15; j++ ) {
  //if(grid[i][j] ==1){
  //
  //tft.fillRect((j*20)+10,(i*20)+20, 20, 20, RED);
  //}
  //if(grid[i][j] ==2){
  ////fillRect("/crate1.bmp",
  ////(tft.width() - ((j*20)+30) ),
  ////(tft.height() - ((i*20)+20) ));
  //tft.fillRect((j*20)+10,(i*20)+20, 20, 20, CYAN);
  //}
  //}
  //}

  //bmpDraw("/bman1.bmp",				//man
  //(tft.width() ) - (50 ),
  //(tft.height()- 40 ));


  if (!ts.begin()) {
    //		  Serial.println("Couldn't start touchscreen controller");
    while (1);
  }
  scherm = Map;




  void links() {
    nunchuk.update();
    if (grid[row][column - 1] == 0 || grid[row][column - 1] == 4)
    {
      //bmpDraw("/bman3.bmp",         //links
      //(tft.width() ) - (x ),
      //(tft.height()- y ));
  
      tft.fillRect(y - 20, x, 20, 20, GREEN);
  
  
      //bmpDraw("/bman2.bmp",         //rechts, volgende blokje
      //(tft.width() ) - (x + 20 ),
      //(tft.height()- y ));
  
  
      //bmpDraw("/bman1.bmp",         //mid
      //(tft.width() ) - (x + 20 ),
      //(tft.height()- y ));
  
      if (grid[row][column] != 3)
      {
        tft.fillRect(y, x, 20, 20, BLACK);
      }
  
      //bmpDraw("/black.bmp",         //oude model weghalen
      //(tft.width() ) - (x ),
      //(tft.height()- y ));
      y = y - 20;
      column = column - 1;
    }
  }
  
  void rechts() {
    nunchuk.update();
    if (grid[row][column + 1] == 0 || grid[row][column + 1] == 4)
    {
      //bmpDraw("/bman2.bmp",       //rechts
      //(tft.width() ) - (x ),
      //(tft.height()- y ));
  
      //bmpDraw("/bman3.bmp",       //links, volgende blokje
      //(tft.width() ) - (x - 20 ),
      //(tft.height()- y ));
      tft.fillRect(y + 20, x, 20, 20, GREEN);
  
  
      //bmpDraw("/bman1.bmp",       //mid
      //(tft.width() ) - (x - 20 ),
      //(tft.height()- y ));
      //
      //bmpDraw("/black.bmp",       //oude model weghalen
      //(tft.width() ) - (x ),
      //(tft.height()- y ));
      if (grid[row][column] != 3)
      {
        tft.fillRect(y, x, 20, 20, BLACK);
      }
  
      y = y + 20;
      column = column + 1;
    }
  }
  
  void omhoog() {
    nunchuk.update();
    if (grid[row - 1][column] == 0 || grid[row - 1][column] == 4 ) {
      //bmpDraw("/bman1.bmp",       //nieuwe sprite plaatsen
      //(tft.width() ) - (x ),
      //(tft.height()- y-20 ));
      //bmpDraw("/bman1.bmp",       //nieuwe sprite plaatsen
      //(tft.width() ) - (x ),
      //(tft.height()- y-20 ));
  
      tft.fillRect(y, x - 20, 20, 20, GREEN);
  
      if (grid[row][column] != 3)
      {
        tft.fillRect(y, x, 20, 20, BLACK);
      }
  
      //bmpDraw("/black.bmp",       //oude model weghalen
      //(tft.width() ) - (x ),
      //(tft.height()- y ));
      x = x - 20;
      row = row - 1;
    }
  }
  
  void omlaag() {
    nunchuk.update();
    if (grid[row + 1][column] == 0 || grid[row + 1][column] == 4 )
    {
      //bmpDraw("/bman1.bmp",       //nieuwe sprite plaatsen
      //(tft.width() ) - (x ),
      //(tft.height()- y+20 ));
  
      tft.fillRect(y, x + 20, 20, 20, GREEN);
  
      if (grid[row][column] != 3)
      {
        tft.fillRect(y, x, 20, 20, BLACK);
      }
  
      //bmpDraw("/black.bmp",       //oude model weghalen
      //(tft.width() ) - (x ),
      //(tft.height()- y ));
      x = x + 20;
      row = row + 1;
    }
  }
  
  void bomb() {
  
  
    //bmpDraw("/bomb1.bmp",       //bomb stage 1 tekenen
    //(tft.width() ) - (x ),
    //(tft.height()- y));
  
    //bmpDraw("/bomb2.bmp",       //bomb stage 2 tekenen
    //(tft.width() ) - (x ),
    //(tft.height()- y));
    //delay("500");
  
    //bmpDraw("/bomb3.bmp",       //bomb stage 3 tekenen
    //(tft.width() ) - (x ),
    //(tft.height()- y));
  
    //delay("100");
    //bmpDraw("/expl1.bmp",       //oude model weghalen
    //(tft.width() ) - (x ),
    //(tft.height()- y));
  
  
    if (grid[brow - 1][bcolumn] != 1)
    {
      //Serial.print(grid[row-1][column]);
      //bmpDraw("/expl1.bmp",       //als er geen muur boven de bomb staat teken een explosie
      //(tft.width() ) - (x ),
      //(tft.height()- y)-20);
      tft.fillRect(by, bx - 20, 20, 20, YELLOW);
      grid[brow - 1][bcolumn] = 4;
  
    }
  
  
    if (grid[brow][(bcolumn + 1)] != 1)
    {
      //bmpDraw("/expl1.bmp",       //als er geen muur rechts van de bomb staat teken een explosie
      //(tft.width() ) - (x -20 ),
      //(tft.height()- y));
      tft.fillRect(by + 20, bx, 20, 20, YELLOW);
      grid[brow][bcolumn + 1] = 4;
  
    }
  
    if (grid[(brow + 1)][bcolumn] != 1)
    {
      //bmpDraw("/exp.bmp",       //als er geen muur onder de bomb staat teken een explosie
      //(tft.width() ) - (x),
      //(tft.height()- y+20));
      tft.fillRect(by, bx + 20, 20, 20, YELLOW);
      grid[brow + 1][bcolumn] = 4;
  
    }
  
    if (grid[brow][(bcolumn - 1)] != 1)
    {
      //bmpDraw("/expl1.bmp",       //als er geen muur links van de bomb staat teken een explosie
      //(tft.width() ) - (x  +20 ),
      //(tft.height()- y));
      //Serial.print("explosion links");
      tft.fillRect(by - 20, bx, 20, 20, YELLOW);
      grid[brow][bcolumn - 1] = 4;
    }
    boem = false;
    bombtimer = millis();
  }
  
  void game() {
  
    tft.fillRect(y, x, 20, 20, GREEN);
  
    /* Het tekenen van de crate's */
    for (int i = 0; i < 11; i++ ) {
      for (int j = 0; j < 15; j++ ) {
        if (grid[i][j] == 1) {
  
          tft.fillRect((j * 20) + 10, (i * 20) + 20, 20, 20, RED);
        }
        if (grid[i][j] == 2) {
          //fillRect("/crate1.bmp",
          //(tft.width() - ((j*20)+30) ),
          //(tft.height() - ((i*20)+20) ));
          tft.fillRect((j * 20) + 10, (i * 20) + 20, 20, 20, CYAN);
        }
      }
    }


  while (live = true) {
    delay(100);
    int direction = 4;
    nunchuk.update();

    if ((startMillis + 3000  <= millis())  && boem == true )
    {
      Serial.print("start");
      bomb();
    }


    if ((bombtimer + 3000)  <= millis() && boem == false)
    {
      for (int i = 0; i < 11; i++ ) {
        for (int j = 0; j < 15; j++ ) {
          if (grid[i][j] == 3 || grid[i][j] == 4 ) {
            grid[i][j] = 0;
            tft.fillRect((j * 20) + 10, (i * 20) + 20, 20, 20, BLACK);
          }
        }
      }
      bombdown = false;
    }


    //Witte echte controllers
    if (nunchuk.analogY > 180 && nunchuk.analogX > 100 && nunchuk.analogX < 180 ) direction = 0;
    else if (nunchuk.analogY < 65 && nunchuk.analogX > 100 && nunchuk.analogX < 180 ) direction = 2;
    else if (nunchuk.analogX >  180 && nunchuk.analogY > 100 && nunchuk.analogY < 180 ) direction = 1;
    else if (nunchuk.analogX < 65 && nunchuk.analogY > 100 && nunchuk.analogY < 180 ) direction = 3 ;


    if (direction == 0) omhoog();
    if (direction == 2) omlaag();
    if (direction == 1) rechts();
    if (direction == 3) links();


    if (nunchuk.zButton == 1 && bombdown == false) {
      grid[row][column] = 3;
      tft.fillRect(y, x, 20, 20, YELLOW);
      startMillis = millis();
      boem = true;
      brow = row;
      bcolumn = column;
      bx = x;
      by = y;
      bombdown = true;
    }

  }
}

  while (1) {
    init();
    // See if there's any  touch data for us
    if (ts.bufferEmpty()) {
      return;
    }
  
    TS_Point p = ts.getPoint();
  
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
  
      // Scale from ~0->4000 to tft.width using the calibration #'s
      p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
      p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());
  
      //Display received X an Y value from touch
      Serial.print("p.x="); Serial.print(p.x);
      Serial.print(", p.y="); Serial.println(p.y);
    }
  
    switch (scherm) {
      case Home :																	//home scherm
        Serial.print("home");
        //bmpDraw("/home.bmp",													//beginscherm tekenen
        //(tft.width() - 320 ),
        //(tft.height() - 240 ));
  
        if (p.x >= 140 && p.x <= 185 && p.y >= 50  && p.y <= 110 ) {
          //bmpDraw("/s_lobby1.bmp",
          //(tft.width() - 320 ),
          //(tft.height() - 240 ));												//scherm tekenen bij *Start lobby
          Serial.println("  NEXT STATE = Start");
          scherm = Start;
        }
        if (p.x >= 140 && p.x <= 185 && p.y >= 120 && p.y <= 180 ) {
          //bmpDraw("/j_lobby.bmp",
          //(tft.width() - 320 ),
          //(tft.height() - 240 ));												//scherm tekenen bij *Join lobby
          Serial.println("  NEXT STATE = Join");
          scherm = Join;
        }
        break;
      case Start :
        Serial.println("Start");												//*Start lobby
        if (p.x >= 90 && p.x <= 200 && p.y >= 15  && p.y <= 60 ) {
          //bmpDraw("/ready.bmp",
          //(tft.width() - 198 ),
          //(tft.height() - 110 ));												//Vinkje tekenen bij *Go
          int rdy_1 = 1;
          if (rdy_1 == 1 && rdy_2 == 1) {
            rdy_1 = 0;
            //rdy_2 = 0;
            scherm = Map;													//Naar de game als allebei de spelers RDY zijn
          }
  
        }
        if (p.x >= 45 && p.x <= 70 && p.y >= 15 && p.y <= 60 ) {
          //bmpDraw("/home.bmp",
          //(tft.width() - 320 ),
          //(tft.height() - 240 ));												//Home tekenen bij *Back
          rdy_1 = 0;
          rdy_2 = 0;
          Serial.println("  NEXT STATE = Home");
          scherm = Home;
        }
        break;
      case Join :
        Serial.println("Join");													//*Join lobby
        if (p.x >= 90 && p.x <= 200 && p.y >= 15  && p.y <= 60 ) {
          //bmpDraw("/ready.bmp",
          //(tft.width() - 100 ),
          //(tft.height() - 110 ));												//Vinkje tekenen bij *Go
          int rdy_2 = 1;
          if (rdy_1 == 1 && rdy_2 == 1) {
            rdy_1 = 0;
            rdy_2 = 0;
            scherm = Map;													//Naar de game als allebei de spelers RDY zijn
          }
        }
        if (p.x >= 45 && p.x <= 70 && p.y >= 15 && p.y <= 60 ) {
          //bmpDraw("/home.bmp",
          //(tft.width() - 320 ),
          //(tft.height() - 240 ));												//scherm tekenen bij *Back
          rdy_1 = 0;
          rdy_2 = 0;
          Serial.println("  NEXT STATE = Home");
          scherm = Home;
        }
  
      case Map:
        tft.fillScreen(BLACK);
        map();														//*Beginnen spel
    }
  }
return 0;
}



// This function opens a Windows Bitmap (BMP) file and
// displays it at the given coordinates.  It's sped up
// by reading many pixels worth of data at a time
// (rather than pixel by pixel).  Increasing the buffer
// size takes more of the Arduino's precious RAM but
// makes loading a little faster.  20 pixels seems a
// good balance.


void bmpDraw(char *filename, int16_t x, int16_t y) {

  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3 * BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col, x2, y2, bx1, by1;
  uint8_t  r, g, b;
  uint32_t pos = 0;

  if ((x >= tft.width()) || (y >= tft.height())) return;

  Serial.println();
  Serial.print(F("Loading image '"));
  Serial.print(filename);
  Serial.println('\'');

  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
    //Serial.print(F("File not found"));
    return;
  }

  // Parse BMP header
  if (read16(bmpFile) == 0x4D42) { // BMP signature
    Serial.print(F("File size: "));
    Serial.println(read32(bmpFile));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    Serial.print(F("Image Offset: ")); Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    Serial.print(F("Header size: "));
    Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if (read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      Serial.print(F("Bit Depth: "));
      Serial.println(bmpDepth);
      if ((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        Serial.print(F("Image size: "));
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if (bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        x2 = x + bmpWidth  - 1; // Lower-right corner
        y2 = y + bmpHeight - 1;
        if ((x2 >= 0) && (y2 >= 0)) { // On screen?
          w = bmpWidth; // Width/height of section to load/display
          h = bmpHeight;
          bx1 = by1 = 0; // UL coordinate in BMP file
          if (x < 0) { // Clip left
            bx1 = -x;
            x   = 0;
            w   = x2 + 1;
          }
          if (y < 0) { // Clip top
            by1 = -y;
            y   = 0;
            h   = y2 + 1;
          }
          if (x2 >= tft.width())  w = tft.width()  - x; // Clip right
          if (y2 >= tft.height()) h = tft.height() - y; // Clip bottom

          // Set TFT address window to clipped image bounds
          tft.startWrite(); // Requires start/end transaction now
          tft.setAddrWindow(x, y, w, h);

          for (row = 0; row < h; row++) { // For each scanline...

            // Seek to start of scan line.  It might seem labor-
            // intensive to be doing this on every line, but this
            // method covers a lot of gritty details like cropping
            // and scanline padding.  Also, the seek only takes
            // place if the file position actually needs to change
            // (avoids a lot of cluster math in SD library).
            if (flip) // Bitmap is stored bottom-to-top order (normal BMP)
              pos = bmpImageoffset + (bmpHeight - 1 - (row + by1)) * rowSize;
            else     // Bitmap is stored top-to-bottom
              pos = bmpImageoffset + (row + by1) * rowSize;
            pos += bx1 * 3; // Factor in starting column (bx1)
            if (bmpFile.position() != pos) { // Need seek?
              tft.endWrite(); // End TFT transaction
              bmpFile.seek(pos);
              buffidx = sizeof(sdbuffer); // Force buffer reload
              tft.startWrite(); // Start new TFT transaction
            }
            for (col = 0; col < w; col++) { // For each pixel...
              // Time to read more pixel data?
              if (buffidx >= sizeof(sdbuffer)) { // Indeed
                tft.endWrite(); // End TFT transaction
                bmpFile.read(sdbuffer, sizeof(sdbuffer));
                buffidx = 0; // Set index to beginning
                tft.startWrite(); // Start new TFT transaction
              }
              // Convert pixel from BMP to TFT format, push to display
              b = sdbuffer[buffidx++];
              g = sdbuffer[buffidx++];
              r = sdbuffer[buffidx++];
              tft.writePixel(tft.color565(r, g, b));
            } // end pixel
          } // end scanline
          tft.endWrite(); // End last TFT transaction
        } // end onscreen
      } // end goodBmp
    }
  }

  bmpFile.close();
  if (!goodBmp)
    Serial.println(F("BMP format not recognized."));
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.


void map() {
  game();
  delay(100);
  nunchuk.update();
}

uint16_t read16(File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}
