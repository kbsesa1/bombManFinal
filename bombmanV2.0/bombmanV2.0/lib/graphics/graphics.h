// graphics.h

#ifndef _GRAPHICS_h
#define _GRAPHICS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>
#include <SD.h>


class Graphics
{
	public:
	Graphics();
	void init();
	void drawWall(uint8_t gridWidth,uint8_t gridHeight);
	void drawMap(uint8_t map[][11]);
	protected:
	private:
	uint16_t getXfromGrid(uint8_t grid);
	uint16_t getYfromGrid(uint8_t grid);
	uint16_t read16(File &f);
	uint32_t read32(File &f);
	uint16_t color565(uint8_t red, uint8_t green, uint8_t blue);
	void bmpDraw(char *filename, int16_t x, int16_t y);
	void drawSquare(uint16_t x,uint16_t y);
};


#endif

