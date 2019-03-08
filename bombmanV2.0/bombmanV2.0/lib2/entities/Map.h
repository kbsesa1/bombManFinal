// Map.h

#ifndef _MAP_h
#define _MAP_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define GRIDWIDTH 11
#define GRIDHEIGHT 9

#define OPENSPACE 0
#define WALL 1
#define CRATE 2

class Map{
	public:
	uint8_t mapData[GRIDHEIGHT][GRIDWIDTH];
	uint8_t updates[20][3];
	uint8_t updateIndex = 0;
	uint8_t getMapData(uint8_t x,uint8_t y);
	void setMapData(uint8_t x,uint8_t y,uint8_t value);
	void buildMap(uint8_t scenario);
	void changeBlock(uint8_t x,uint8_t y,uint8_t state);
	};

#endif

