

//Libraries--------

#include <Adafruit_STMPE610.h>
#include <ArduinoNunchuk.h>
#include <TaskScheduler.h>


//project specific
#include "lib/irComm/irComm.h"
#include "lib/graphics/graphics.h"
#include "lib/controls/controls.h"
#include "lib/entities/player.h"

//Defines----------
#define GRIDWIDTH
#define GRIDHEIGHT

//Make objects----------
Scheduler game;
IR ir = IR(true);
Graphics gfx;
Adafruit_STMPE610 ts = Adafruit_STMPE610(8);
Player p1 = Player(PLAYERONE);
//ArduinoNunchuk nunchuck = ArduinoNunchuk();

//Variables----------
//test
uint8_t gameTestIndex;
uint8_t stepsIndex;
uint8_t directionIndex;


//Functions----------

//draw buttons

//move
void moveLeft();
void moveRight();
void moveUp();
void moveDown();



//Tasks----------
Task mapDraw (50, -1, &onMapDraw);
Task gameTest (200, -1, &onGameTest);

void onMapDraw(){
	gfx.updateMap();
	gfx.drawPlayer(p1);
}

void onGameTest(){
	p1.walk(directionIndex);
	stepsIndex++;
	if (stepsIndex >= 12)
	{
		stepsIndex = 0;
		directionIndex ++;
		if (directionIndex >= 4)
		{
			directionIndex = 0;
		}
	}
}

int main(){
	//initialisation process Arduino UNO
	init();
	Serial.begin(115200);
	
	//initialisation process task scheduler
	game.init();
	game.addTask(mapDraw);
	game.addTask(gameTest);
	
	mapDraw.enable();
	gameTest.enable();
	//game.addTask(printIRData);
	//irCommunication.enable();
	//printIRData.enable();
	//game.addTask(tekenDriehoek);
	//game.addTask(tekenVierkant);
	//tekenVierkant.enable();
	
	//initialisation process infrared communication
	//ir.begin();
	
	
	//initialisation process tft screen
	
	
	
	//initialisation process touchscreen support
	ts.begin();

/*
	//initialisation process Wii nunchuck
	DDRC |= (1<<PORTC2) | (1<<PORTC3);
	PORTC |= (1<<PORTC3);
	nunchuck.init();
*/
gfx.init();
gfx.drawWall();
gfx.buildMap(0);
gfx.drawMap();


 

	while(1){
		game.execute();	
	}
}

