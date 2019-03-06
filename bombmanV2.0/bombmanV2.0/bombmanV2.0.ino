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
#define STMPE_CS 8

//data voor het scherm voor touchscreen functionaliteit
#define TS_MINX 30
#define TS_MINY 30
#define TS_MAXX 4000
#define TS_MAXY 4000
#define MINPRESSURE 50
#define MAXPRESSURE 1000

enum scherm{Home, Start, Join, Winner, Death};
typedef enum scherm scherm_t;
scherm_t scherm;


//Make objects----------
Scheduler game;
IR ir = IR(true);
Graphics gfx;
Adafruit_STMPE610 ts = Adafruit_STMPE610(8);
Player p1 = Player(PLAYERONE);
ArduinoNunchuk nunchuck = ArduinoNunchuk();

//Variables----------
uint8_t huidig_scherm_menu = 0;
uint8_t keuze_touch_menu = 1;

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
void onMenu(uint8_t menuCase);
//Tasks----------
Task mapDraw (50, -1, &onMapDraw);
Task gameTest (200, -1, &onGameTest);
Task menu (1, -1, &onMenu);

int main(){
	//initialisation process Arduino UNO
	init();
	Serial.begin(115200);
	
	//initialisation process task scheduler
	game.init();
	game.addTask(mapDraw);
	game.addTask(gameTest);
	game.addTask(menu);
	menu.enable();

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
	if (stepsIndex >= 12){
		stepsIndex = 0;
		directionIndex ++;
		if (directionIndex >= 4){
			directionIndex = 0;
		}
	}
}

void onMenu(){
	if (ts.bufferEmpty()) {
		return;
	}
	
	TS_Point p = ts.getPoint();
	

	if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {

		// Scale from ~0->4000 to tft.width using the calibration #'s
		p.x = map(p.x, TS_MINX, TS_MAXX, 0, 240);
		p.y = map(p.y, TS_MINY, TS_MAXY, 0, 320);

		//Display received X an Y value from touch
		Serial.print("p.x="); Serial.print(p.x);
		Serial.print(", p.y="); Serial.print(p.y);
		Serial.print(", p.z="); Serial.println(p.z);
	}
	
	if (huidig_scherm_menu != keuze_touch_menu){
		gfx.drawMenu(keuze_touch_menu);
		huidig_scherm_menu = keuze_touch_menu;
	}
	
}