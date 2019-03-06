// bomb.h

#ifndef _BOMB_h
#define _BOMB_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class Bomb
{
public:
Bomb(uint8_t color);

protected:
private:
};

#endif

