// graphics.h

#ifndef _GRAPHICS_h
#define _GRAPHICS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"	
#else
	#include "WProgram.h"
#endif

#include <Adafruit_ILI9341.h>
#include <SPI.h>
#include <SD.h>
#include "../entities/player.h"
#include "../entities/map.h"



#define black 0x0000
#define brown 0xB3CA
#define gold  0xFE41
#define grey  0x7BEF



class Graphics
{
	public:
	Graphics();
	void init();
	void drawWall(uint8_t height,uint8_t width);
	void drawMap(Map &m);
	void updateMap(Map &m);
	void drawPlayer(Player &p);
	void drawMenu(uint8_t menu);
	void drawHomescreen();
	void drawLobby();
	void drawButton(uint8_t xc, uint8_t yc, uint8_t xl, uint8_t yl);
	void setTextSize(uint8_t size);
	void setTextColor(uint16_t color);	
	void print(int16_t x, int16_t y, unsigned char *str);
	void drawChar(int16_t x, int16_t y, unsigned char c);
	void fillScreen(uint16_t color);
	void bmpDraw(char *filename, int16_t x, int16_t y);
	void drawBlock(uint8_t x,uint8_t y,uint8_t state);
	void drawGridBlock(uint8_t x,uint8_t y,uint8_t state);
	protected:
	private:	
	uint16_t getXfromGrid(uint8_t grid);
	uint16_t getYfromGrid(uint8_t grid);
	uint16_t read16(File &f);
	uint32_t read32(File &f);
	uint16_t color565(uint8_t red, uint8_t green, uint8_t blue);
	
	void drawSquare(uint16_t x,uint16_t y);
	
};


#endif

