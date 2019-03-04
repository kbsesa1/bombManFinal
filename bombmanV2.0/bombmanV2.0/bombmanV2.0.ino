
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
//ArduinoNunchuk nunchuck = ArduinoNunchuk();

//Variables----------
//test




//Functions----------

//draw buttons

//move
void moveLeft();
void moveRight();
void moveUp();
void moveDown();



//Tasks----------


int main(){
	//initialisation process Arduino UNO
	init();
	
	//initialisation process task scheduler
	//game.init();
	//game.addTask(irCommunication);
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
		/*
		gfx.changeBlock(0,0,2);
		gfx.changeBlock(1,1,2);
		gfx.changeBlock(2,2,2);
		gfx.updateMap();
		delay(100);
		gfx.changeBlock(0,0,0);
		gfx.changeBlock(1,1,1);
		gfx.changeBlock(2,2,0);
		gfx.updateMap();
		delay(1000);
		*/
		game.execute();
	}
}

