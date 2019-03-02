
//Libraries--------
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_STMPE610.h>
#include <ArduinoNunchuk.h>
#include <gfxfont.h>
#include <TaskScheduler.h>
#include <SPI.h>
//project specific
#include "lib/irComm/irComm.h"

//Defines----------
//TFT screen
#define tft_cs  10
#define tft_dc 9

//Touchscreen
#define stmpe_cs 8

//Colors
#define black 0x0000
#define brown 0xB3CA
#define gold  0xFE41
#define grey  0x7BEF

//Make objects----------
Scheduler game;
IR ir = IR(true);
Adafruit_ILI9341 tft = Adafruit_ILI9341(tft_cs, tft_dc);
Adafruit_STMPE610 ts = Adafruit_STMPE610(stmpe_cs);
//ArduinoNunchuk nunchuck = ArduinoNunchuk();

//Variables----------
//test
uint8_t knippertest = 0;
uint8_t xcord = 0;
uint32_t data = 1024;



//Functions----------
//Test
void driehoek();
void vierkant();

//draw buttons
void drawButton(uint8_t xl, uint8_t yl, uint8_t xc, uint8_t yc);
void drawLobby();
void drawText(uint8_t xl,uint8_t yl, uint8_t textsize, char * string);

//move
void moveLeft();
void moveRight();
void moveUp();
void moveDown();



//Tasks----------
Task tekenDriehoek(2000, -1, &driehoek);
Task tekenVierkant(333, -1, &vierkant);
Task irCommunication(100, -1, &irSend);
Task printIRData(200, -1, &irPrint);

int main(){
	//initialisation process Arduino UNO
	init();
	Serial.begin(115200);
	//initialisation process task scheduler
	game.init();
	game.addTask(irCommunication);
	game.addTask(printIRData);
	irCommunication.enable();
	printIRData.enable();
	//game.addTask(tekenDriehoek);
	//game.addTask(tekenVierkant);
	//tekenVierkant.enable();
	
	//initialisation process infrared communication
	ir.begin();
	
	/*
	//initialisation process tft screen
	tft.begin();
	tft.setRotation(1);

	//initialisation process touchscreen support
	ts.begin();

	//initialisation process Wii nunchuck
	DDRC |= (1<<PORTC2) | (1<<PORTC3);
	PORTC |= (1<<PORTC3);
	nunchuck.init();
*/

	while(1){
		game.execute();
	}
}

void driehoek(){
	tft.fillRect(xcord,20,50,50, 0xFE41 );
	xcord += 20;
	if(xcord == 200){
		xcord = 0;
	}
}

void vierkant(){
	if(knippertest == 0){
		tft.fillRect(180,180, 50,50,0xF921);
		knippertest = 1;
		} else {
		tft.fillRect(180,180, 50,50,0xF81F);
		knippertest = 0;
	}
}

// Draws two sqaures to make a button
void drawButton(uint8_t xc, uint8_t yc, uint8_t xl, uint8_t yl){
	#define offset 5    //thickness of the brown border
	tft.fillRect(xc,yc,xl,yl,brown);
	tft.fillRect(xc+offset,yc+offset, xl-(2*offset), yl-(2*offset), gold);
}

/*
// Draws the lobbyscreen
void drawLobby(){
	tft.fillScreen(grey);
	tft.setTextColor(gold);
	tft.setCur
	drawButton(20, 60, 90, 110);
	tft.setTextColor(black);
	tft.setTextSize(5);
	tft.setCursor(28,98);
	tft.print("GO!");
}

// Draws text
void drawText(uint8_t xl, uint8_t yl, uint8_t textsize, ..){
	tft.setCursor(xl, yl);
	tft.setTextSize(textsize);
	tft.println(text);
}
*/

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