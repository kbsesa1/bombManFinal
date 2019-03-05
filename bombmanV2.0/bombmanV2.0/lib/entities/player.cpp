// 
// 
// 

#include "player.h"

uint8_t color;
uint8_t lastX = 0;
uint8_t lastY = 0;
uint8_t currentX = 0;
uint8_t currentY = 0;
uint8_t needsRedraw = 1;

Player::Player(uint8_t c){
	color = c;
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

uint8_t Player::isRedrawn(){
	return needsRedraw;
	
}
void Player::drawn(){
	needsRedraw = 0;
}
void Player::walk(uint8_t Direction){
	needsRedraw = 1;
}