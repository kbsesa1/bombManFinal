// 
// 
// 

#include "player.h"

uint8_t color;
uint8_t lastX = 0;
uint8_t lastY = 0;
uint8_t currentX = 0;
uint8_t currentY = 0;
uint16_t positionX = 0;
uint16_t positionY = 0;
uint8_t direction = DOWN;
uint8_t step = 0;
uint8_t needsRedraw = 1;


Player::Player(uint8_t c){
	color = c;
	positionX = 20;
	positionY = 20;
}
uint8_t Player::getColor(){
	return color;
}
void Player::setPosition(uint8_t x,uint8_t y){
	lastX = currentX;
	lastY = currentY;
	currentX = x;
	currentY = y;
	needsRedraw = 1;
}
uint8_t Player::getCurrentX(){
	 return currentX;
	 }
uint8_t Player::getCurrentY(){
	 return currentY;
}
uint8_t Player::getLastX(){
	 return lastX;
}
uint8_t Player::getLastY(){
	 return lastY;
}
uint16_t Player::getPositionX(){
	return positionX;
}
uint16_t Player::getPositionY(){
	return positionY;
}
uint8_t Player::getDirection(){
	return direction;
}
uint8_t Player::getStep(){
	return step;
}
uint8_t Player::isRedrawn(){
	return needsRedraw;
	
}
void Player::drawn(){
	needsRedraw = 0;
}
void Player::walk(uint8_t Direction,Graphics &gfx){
	direction = Direction;
	switch(direction){
		case UP:
		positionY -= 5;
		break;
		case DOWN:
		positionY += 5;
		break;
		case LEFT:
		positionX -= 5;
		break;
		case RIGHT:
		positionX += 5;
		break;
	}
	step++;
	if (step >= 3)
	{
		step = 0;
	}
	needsRedraw = 1;
}