
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
uint8_t mapData[9][11] = {
	{0,0,0,0,0,0,0,0,0,0,0},
		{0,1,2,1,0,1,0,1,0,1,0},
			{0,0,0,0,0,0,0,0,0,0,0},
				{0,1,2,1,0,1,0,1,0,1,0},
					{0,0,0,0,0,2,0,0,0,0,0},
						{0,1,0,1,0,1,2,1,0,1,0},
							{0,0,0,0,0,0,0,0,0,0,0},
								{0,1,0,1,0,1,0,1,0,1,0},
									{0,0,2,0,0,0,0,0,0,0,0},
	};



//Functions----------

//draw buttons

//move
void moveLeft();
void moveRight();
void moveUp();
void moveDown();



//Tasks----------
Task irCommunication(100, -1, &irSend);
Task printIRData(200, -1, &irPrint);

int main(){
	//initialisation process Arduino UNO
	init();
	
	Serial.begin(115200);
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
gfx.drawWall(11,9);
gfx.drawMap(mapData);

	while(1){
		game.execute();
	}
}



void irSend(){
	ir.run();
	ir.send(data);
	data ++;
}
void irPrint(){
	Serial.print("i got: ");
	uint32_t value = ir.read();
	Serial.println(value);
}
