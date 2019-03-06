//Bomberman opdracht voor KBS ESA1 - door Bart, Jarom, Joost en Rik
//Libraries--------
#include <Adafruit_STMPE610.h>
#include <ArduinoNunchuk.h>
#include <TaskScheduler.h>
//project specific
#include "lib/irComm/irComm.h"
#include "lib/graphics/graphics.h"
#include "lib/controls/controls.h"

//Defines----------
#define GRIDWIDTH
#define GRIDHEIGHT

//Make objects----------
Scheduler game;
IR ir = IR(true);
Graphics gfx;
Adafruit_STMPE610 ts = Adafruit_STMPE610(8);
Player p1 = Player(PLAYERONE);
ArduinoNunchuk nunchuck = ArduinoNunchuk();

//Variables----------
//test
uint8_t gameTestIndex;
uint8_t stepsIndex;
uint8_t directionIndex;


//Functions----------

//move
void moveLeft();
void moveRight();
void moveUp();
void moveDown();


void onMapDraw();
void onGameTest();
//Tasks----------
Task mapDraw (50, -1, &onMapDraw);
Task gameTest (200, -1, &onGameTest);
Task lobby (1, 1, &onLobby);
Task homescreen (1, 1, &onHomescreen);

int main(){
	//initialisation process Arduino UNO
	init();
	Serial.begin(115200);
	
	//initialisation process task scheduler
	game.init();
	game.addTask(mapDraw);
	game.addTask(gameTest);
	game.addTask(lobby);
	game.addTask(homescreen);
	
	homescreen.enable();

	//mapDraw.enable();
	//gameTest.enable();
	//game.addTask(printIRData);
	//irCommunication.enable();
	//printIRData.enable();
	
	//initialisation process infrared communication
	//ir.begin();
	
	
	//initialisation process tft screen
	gfx.init();
	gfx.drawWall();
	gfx.buildMap(0);
	gfx.drawMap();
	
	
	//initialisation process touchscreen support
	ts.begin();

/*
	//initialisation process Wii nunchuck
	DDRC |= (1<<PORTC2) | (1<<PORTC3);
	PORTC |= (1<<PORTC3);
	nunchuck.init();
*/
	

	while(1){
		game.execute();	
	}
}

void onMapDraw(){
	gfx.updateMap();
	gfx.drawPlayer(p1);
}

void onGameTest(){
	p1.walk(directionIndex,gfx);
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

void onLobby(){
	gfx.drawLobby();
}

void onHomescreen(){
	gfx.drawHomescreen();
}