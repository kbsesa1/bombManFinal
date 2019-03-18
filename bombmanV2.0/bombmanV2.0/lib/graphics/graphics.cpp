#include "graphics.h"
//Colors
#define black 0x0000
#define white 0xffff
#define brown 0xB3CA
#define gold  0xFE41
#define grey  0x7BEF


#define BUFFPIXEL 5	


Adafruit_ILI9341 tft = Adafruit_ILI9341(10, 9);

uint8_t textSize = 1;
uint16_t textColor = white;

class Player;
class Map;


Graphics::Graphics(){
	
}
void Graphics::init(){
	Serial.println("graphics init");
	if (SD.begin(4))
	{
		Serial.println("SD ok!");
	}else{
		Serial.println("SD error!");
	}
	
	tft.begin();
	tft.setRotation(1);
	fillScreen(0x0000);
}

void Graphics::fillScreen(uint16_t color){
	tft.fillRect(0,0,ILI9341_TFTWIDTH,ILI9341_TFTHEIGHT,color);
}
void Graphics::drawSquare(uint16_t x,uint16_t y){
	tft.fillRect(x,y, 50,50,gold);
}

void Graphics::drawWall(uint8_t height,uint8_t width){
	//draw top wall
	for(int i = 0;i<width+2;i++) {
		drawBlock(i*20,0,1);
	}
	//draw sides
	for(int i = 0;i<height;i++) {
		drawBlock(0,(i*20)+20,1);
		drawBlock((width+1)*20,(i*20)+20,1);
	}
	//draw bottom wall
	for(int i = 0;i<width+2;i++) {
		drawBlock(i*20,(height+1)*20,1);
	}
}

void Graphics::drawMap(Map &m){
	for (uint8_t x = 0; x<GRIDWIDTH;x++)
	{
		for (uint8_t y = 0;y<GRIDHEIGHT;y++)
		{
			drawGridBlock(x,y,m.getMapData(x,y));
		}
	}
}

void Graphics::updateMap(Map &m){
	while(m.updateIndex > 0){
	drawGridBlock(m.updates[m.updateIndex-1][0],m.updates[m.updateIndex-1][1],m.updates[m.updateIndex-1][2]);
	m.setMapData(m.updates[m.updateIndex-1][0],m.updates[m.updateIndex-1][1],m.updates[m.updateIndex-1][2]);
	m.updateIndex --;	
	}
	
}
void Graphics::drawGridBlock(uint8_t x,uint8_t y,uint8_t state){
	drawBlock(getXfromGrid(x),getYfromGrid(y),state);
}

void Graphics::drawBlock(uint8_t x,uint8_t y,uint8_t state){
	switch (state){
		case 1:
		bmpDraw("map/Wall1.bmp",x,y);
		break;
		case 2:
		bmpDraw("map/Crate1.bmp",x,y);
		break;
		default:
		tft.fillRect(x,y,20,20,0x0000);
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

	if((x >= ILI9341_TFTWIDTH) || (y >= ILI9341_TFTHEIGHT)) return;

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
					if(x2 >= ILI9341_TFTWIDTH)  w = ILI9341_TFTWIDTH  - x; // Clip right
					if(y2 >= ILI9341_TFTHEIGHT) h = ILI9341_TFTHEIGHT - y; // Clip bottom
					
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

void  Graphics::setTextSize(uint8_t size){
	textSize = size;
}
void  Graphics::setTextColor(uint16_t color){
	textColor = color;
}
void  Graphics::print(int16_t x, int16_t y, unsigned char *str){
	uint8_t index = 0;
	while(str[index] != '\0'){
		drawChar(x+(textSize*6*index),y,str[index]);
		index++;
	}
}

void Graphics::drawChar(int16_t x, int16_t y, unsigned char c) {

File     fontFile;
uint8_t bitmap[5];
uint16_t charOffset = c*5;

if ((fontFile = SD.open("font.hex")) == NULL) {
	Serial.println("file not found");
	return;
}
fontFile.seek(charOffset);
	fontFile.read(bitmap,5);
fontFile.close();

		if((x >= 320)            || // Clip right
		(y >= 240)           || // Clip bottom
		((x + 6 * textSize - 1) < 0) || // Clip left
		((y + 8 * textSize - 1) < 0))   // Clip top
		return;

		
		tft.startWrite();
		for(int8_t i=0; i<5; i++ ) { // Char bitmap = 5 columns
			uint8_t line = bitmap[i];
			for(int8_t j=0; j<8; j++, line >>= 1) {
				if(line & 1) {
					if(textSize == 1)
					tft.writePixel(x+i, y+j, textColor);
					else
					tft.writeFillRect(x+i*textSize, y+j*textSize, textSize, textSize, textColor);
					
				}
			}
		}
		tft.endWrite();

	}

void Graphics::drawPlayer(Player &p){
	char file[24];
	if (p.isRedrawn())
	{
		drawBlock(p.getLastX(),p.getLastY(),OPENSPACE);
		drawBlock(p.getCurrentX(),p.getCurrentY(),OPENSPACE);
		if (p.getColor() == PLAYERONE)
		{
			strcpy(file,"p1/");
			switch(p.getDirection()){
				case DOWN:
				strcat(file,"down/");
				break;
				case UP:
				strcat(file,"up/");
				break;
				case LEFT:
				strcat(file,"left/");
				break;
				case RIGHT:
				strcat(file,"right/");
				break;
			}
			char number[3];
			sprintf(number,"%d",p.getStep());
			strcat(file,number);
			strcat(file,".bmp");
		}
		Serial.println(file);
		bmpDraw(file,p.getPositionX(),p.getPositionY());
		p.drawn();
	}
	
}

//Draw one of the menu's, switch-case
void Graphics::drawMenu(uint8_t menu){
	switch(menu){
		case 1 : drawHomescreen();
			break;
		case 2 : drawLobby();
			break;
	}
}

// Draws two sqaures to make a button  
void Graphics::drawButton(uint8_t xc, uint8_t yc, uint8_t xl, uint8_t yl){
	#define offset 5    //thickness of the brown border
	tft.fillRect(xc,yc,xl,yl,brown);
	tft.fillRect(xc+offset,yc+offset, xl-(2*offset), yl-(2*offset), gold);
}

// Draws the homescreen - switch 1
void Graphics::drawHomescreen(){
	fillScreen(grey);
	setTextSize(7);
	setTextColor(gold);
	//tft.setCursor(33, 15);
	print(33,15,"BOMBER");
	//tft.setCursor(95, 80);
	print(95,80,"MAN");
	drawButton(30,150, 120, 50);
	drawButton(170, 150, 120, 50);
	setTextSize(3);
	setTextColor(black);
	//tft.setCursor(50, 165);
	print(50,165,"Start");
	//tft.setCursor(195, 165);
	print(195,165,"Join");
}

// Draws the lobbyscreen - switch 2
void Graphics::drawLobby(){
	fillScreen(grey);
	setTextSize(5);
	setTextColor(gold);
	print(29,15,"BOMBERMAN");
	drawButton(20, 60, 90, 110);
	setTextColor(black);
	setTextSize(5);
	//tft.setCursor(38,95); positie zit bij print in
	print(38,95,"GO");
	drawButton(20, 180, 90, 45);
	setTextColor(black);
	setTextSize(3);
	//tft.setCursor(33,192);
	print(33,192,"back");
	drawButton(120, 60, 180,165);
	tft.fillRect(130,70,160,145,grey);
}

