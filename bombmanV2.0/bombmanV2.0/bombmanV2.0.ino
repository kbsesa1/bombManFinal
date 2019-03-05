

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



//Functions----------

//draw buttons

//move
void moveLeft();
void moveRight();
void moveUp();
void moveDown();



//Tasks----------
Task mapDraw (100, -1, &onMapDraw);
Task gameTest (1000, -1, &onGameTest);

void onMapDraw(){
	gfx.updateMap();
	gfx.drawPlayer(p1);
}

void onGameTest(){
	switch (gameTestIndex){
		case 0:
		gfx.changeBlock(2,0,0);
		gfx.changeBlock(8,8,0);
		break;
		case 1:
		gfx.changeBlock(3,0,0);
		gfx.changeBlock(7,8,0);
		break;
		case 2:
		gfx.changeBlock(4,0,0);
		gfx.changeBlock(6,8,0);
		break;
		case 3:
		gfx.changeBlock(4,0,2);
		gfx.changeBlock(6,8,2);
		break;
		case 4:
		gfx.changeBlock(3,0,2);
		gfx.changeBlock(7,8,2);
		break;
		case 5:
		gfx.changeBlock(2,0,2);
		gfx.changeBlock(8,8,2);
		break;
	}
	gameTestIndex++;
	if (gameTestIndex >= 6)
	{
		gameTestIndex = 0;
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
	//gameTest.enable();
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

