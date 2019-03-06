// player.h

#ifndef _PLAYER_h
#define _PLAYER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define PLAYERONE 1
#define PLAYERTWO 2
#define DOWN 0
#define UP 1
#define LEFT 2
#define RIGHT 3

class Graphics;
class Player{
	public:
	Player(uint8_t c);
	uint8_t getColor();
	void setPosition(uint8_t x,uint8_t y);
	uint8_t getCurrentX();
	uint8_t getCurrentY();
	uint8_t getLastX();
	uint8_t getLastY();
	uint16_t getPositionX();
	uint16_t getPositionY();
	uint8_t getDirection();
	uint8_t getStep();
	void walk(uint8_t Direction,Graphics &gfx);
	uint8_t isRedrawn();
	void drawn();
	private:
	};

#endif

