// 
// 
// 

#include "graphics.h"



//Colors


#define BUFFPIXEL 5

#define GRIDWIDTH 11
#define GRIDHEIGHT 9

Adafruit_ILI9341 tft = Adafruit_ILI9341(10, 9);

uint8_t mapData[GRIDHEIGHT][GRIDWIDTH];
uint8_t updates[20][3];
uint8_t updateIndex = 0;

Graphics::Graphics(){
	
}
void Graphics::init(){
	SD.begin(4);
	tft.begin();
	tft.setRotation(1);
	tft.setCursor(0, 0);
	tft.fillScreen(0x0000);
}

uint8_t Graphics::getMapData(uint8_t x,uint8_t y){
	return mapData[y][x];
}

void Graphics::setMapData(uint8_t x,uint8_t y,uint8_t value){
	mapData[y][x] = value;
}


void Graphics::buildMap(uint8_t scenario){
switch(scenario){
	case 0:
	for (uint8_t x = 0;x<11;x++)
	{
		for (uint8_t y = 0;y<9;y++)
		{
			setMapData(x,y,CRATE);
		}
	}
	for (uint8_t x = 0;x<5;x++)
	{
		for (uint8_t y = 0;y<4;y++)
		{
			setMapData((x*2)+1,(y*2)+1,WALL);
		}
	}
	setMapData(0,0,OPENSPACE);
	setMapData(0,1,OPENSPACE);
	setMapData(1,0,OPENSPACE);
	setMapData(10,8,OPENSPACE);
	setMapData(9,8,OPENSPACE);
	setMapData(10,7,OPENSPACE);
	break;
}	
}

void Graphics::drawSquare(uint16_t x,uint16_t y){
	tft.fillRect(x,y, 50,50,gold);
}

void Graphics::drawWall(){
	//draw top wall
	for(int i = 0;i<GRIDWIDTH+2;i++) {
		bmpDraw("Wall1.bmp",i*20,0);
	}
	//draw sides
	for(int i = 0;i<GRIDHEIGHT;i++) {
		bmpDraw("Wall1.bmp",0,(i*20)+20);
		bmpDraw("Wall1.bmp",(GRIDWIDTH+1)*20,(i*20)+20);
	}
	//draw bottom wall
	for(int i = 0;i<GRIDWIDTH+2;i++) {
		bmpDraw("Wall1.bmp",i*20,(GRIDHEIGHT+1)*20);
	}
}

void Graphics::drawMap(){
	for (uint8_t x = 0; x<GRIDWIDTH;x++)
	{
		for (uint8_t y = 0;y<GRIDHEIGHT;y++)
		{
			drawBlock(x,y,getMapData(x,y));
		}
	}
}

void Graphics::updateMap(){
	for (uint8_t i = 0;i<updateIndex;i++)
	{
		mapData[updates[i][0]][updates[i][1]] = updates[i][2];
		drawBlock(updates[i][0],updates[i][1],updates[i][2]);
	}
	updateIndex = 0;
}

void Graphics::changeBlock(uint8_t x,uint8_t y,uint8_t state){
	if (mapData[x][y] != state)
	{
		updates[updateIndex][0] = x;
		updates[updateIndex][1] = y;
		updates[updateIndex][2] = state;
		updateIndex++;
	}
	
}

void Graphics::drawBlock(uint8_t x,uint8_t y,uint8_t state){
	switch (state){
		case 1:
		bmpDraw("Wall1.bmp",getXfromGrid(x),getYfromGrid(y));
		break;
		case 2:
		bmpDraw("Crate1.bmp",getXfromGrid(x),getYfromGrid(y));
		break;
		default:
		tft.fillRect(getXfromGrid(x),getYfromGrid(y),20,20,0x0000);
		break;
	}
}
uint16_t Graphics::getXfromGrid(uint8_t grid){
	return grid*20+20;
}
uint16_t Graphics::getYfromGrid(uint8_t grid){
	return grid*20+20;
}

void Graphics::bmpDraw(char *filename, int16_t x, int16_t y) {

	File     bmpFile;
	int      bmpWidth, bmpHeight;   // W+H in pixels
	uint8_t  bmpDepth;              // Bit depth (currently must be 24)
	uint32_t bmpImageoffset;        // Start of image data in file
	uint32_t rowSize;               // Not always = bmpWidth; may have padding
	uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
	uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
	boolean  goodBmp = false;       // Set to true on valid header parse
	boolean  flip    = true;        // BMP is stored bottom-to-top
	int      w, h, row, col, x2, y2, bx1, by1;
	uint8_t  r, g, b;
	uint32_t pos = 0;

	if((x >= tft.width()) || (y >= tft.height())) return;

	// Open requested file on SD card
	if ((bmpFile = SD.open(filename)) == NULL) {
		Serial.println("file not found");
		return;
	}

	// Parse BMP header
	if(read16(bmpFile) == 0x4D42) { // BMP signature
		(void)read32(bmpFile);//read & ignore file size
		(void)read32(bmpFile); // Read & ignore creator bytes
		bmpImageoffset = read32(bmpFile); // Start of image data
		(void)read32(bmpFile);//read & ignore header size
		bmpWidth  = read32(bmpFile);
		bmpHeight = read32(bmpFile);
		if(read16(bmpFile) == 1) { // # planes -- must be '1'
			bmpDepth = read16(bmpFile); // bits per pixel
			if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

				goodBmp = true; // Supported BMP format -- proceed!

				// BMP rows are padded (if needed) to 4-byte boundary
				rowSize = (bmpWidth * 3 + 3) & ~3;

				// If bmpHeight is negative, image is in top-down order.
				// This is not canon but has been observed in the wild.
				if(bmpHeight < 0) {
					bmpHeight = -bmpHeight;
					flip      = false;
				}

				// Crop area to be loaded
				x2 = x + bmpWidth  - 1; // Lower-right corner
				y2 = y + bmpHeight - 1;
				if((x2 >= 0) && (y2 >= 0)) { // On screen?
					w = bmpWidth; // Width/height of section to load/display
					h = bmpHeight;
					bx1 = by1 = 0; // UL coordinate in BMP file
					if(x < 0) { // Clip left
						bx1 = -x;
						x   = 0;
						w   = x2 + 1;
					}
					if(y < 0) { // Clip top
						by1 = -y;
						y   = 0;
						h   = y2 + 1;
					}
					if(x2 >= tft.width())  w = tft.width()  - x; // Clip right
					if(y2 >= tft.height()) h = tft.height() - y; // Clip bottom
					
					// Set TFT address window to clipped image bounds
					tft.startWrite(); // Requires start/end transaction now
					tft.setAddrWindow(x, y, w, h);
					
					for (row=0; row<h; row++) { // For each scanline...
						
						// Seek to start of scan line.  It might seem labor-
						// intensive to be doing this on every line, but this
						// method covers a lot of gritty details like cropping
						// and scanline padding.  Also, the seek only takes
						// place if the file position actually needs to change
						// (avoids a lot of cluster math in SD library).
						if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
						pos = bmpImageoffset + (bmpHeight - 1 - (row + by1)) * rowSize;
						else     // Bitmap is stored top-to-bottom
						pos = bmpImageoffset + (row + by1) * rowSize;
						pos += bx1 * 3; // Factor in starting column (bx1)
						if(bmpFile.position() != pos) { // Need seek?
							tft.endWrite(); // End TFT transaction
							bmpFile.seek(pos);
							buffidx = sizeof(sdbuffer); // Force buffer reload
							tft.startWrite(); // Start new TFT transaction
						}
						for (col=0; col<w; col++) { // For each pixel...
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
							tft.writePixel(tft.color565(r,g,b));
						} // end pixel
					} // end scanline
					tft.endWrite(); // End last TFT transaction
				} // end onscreen
			} // end goodBmp
		}
	}
if (!goodBmp)
{
	Serial.println("bitmap error!");
}
	bmpFile.close();
}

uint16_t Graphics::read16(File &f) {
	uint16_t result;
	((uint8_t *)&result)[0] = f.read(); // LSB
	((uint8_t *)&result)[1] = f.read(); // MSB
	return result;
}

uint32_t Graphics::read32(File &f) {
	uint32_t result;
	((uint8_t *)&result)[0] = f.read(); // LSB
	((uint8_t *)&result)[1] = f.read();
	((uint8_t *)&result)[2] = f.read();
	((uint8_t *)&result)[3] = f.read(); // MSB
	return result;
}

void Graphics::drawPlayer(Player p){
	if (p.isRedrawn())
	{
		drawBlock(p.getLastX(),p.getLastY(),OPENSPACE);
		drawBlock(p.getCurrentX(),p.getCurrentY(),OPENSPACE);
		bmpDraw("bman1.bmp",getXfromGrid(p.getCurrentX()),getYfromGrid(p.getCurrentY()));
		p.drawn();
	}
	
}