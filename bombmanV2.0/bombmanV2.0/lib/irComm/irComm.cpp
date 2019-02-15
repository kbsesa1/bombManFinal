// 
// 
// 

#include "irComm.h"

//defining the sending times
#define START 2000
#define STOP 1600
#define HIGH 1200
#define LOW 800
#define SPACE 400

#define BUFFERLENGTH 36

const uint16_t decodeTimes38[] = {45,75,120,135,180};
const uint16_t decodeTimes56[] = {60,120,160,200,250};
uint16_t minLength;
uint16_t maxLength;
bool recieveSpeed = false;

volatile uint16_t decodeTicks = 0;
volatile uint16_t times[BUFFERLENGTH];
volatile uint16_t timeIndex = 0;

uint8_t outPacket[34];
uint8_t inPacket[34];

ISR(TIMER2_COMPB_vect){
	TCNT2 = 0;
	decodeTicks++;
	if (decodeTicks >= 500)
	{
		decodeTicks = 500;
	}
}
ISR(INT0_vect) //interrupt ISR
{
	uint16_t pulseLength = decodeTicks;
	decodeTicks = 0;
	if (pulseLength >= minLength)
	{
		times[timeIndex] = pulseLength;
		timeIndex++;
	}
	if (pulseLength >= maxLength){
		timeIndex = 0;
	}
}

IR::IR(bool highFrequency = false){
	recieveSpeed = highFrequency;
	if (recieveSpeed)
	{
		minLength = decodeTimes56[0];
		maxLength = decodeTimes56[4];
		}else{
		minLength = decodeTimes38[0];
		maxLength = decodeTimes38[4];
	}
}
void IR::initSend(){
	//set ir led port to output
	DDRD |= (1 << PORTD3);
	
	//set waveform generation to normal mode
	TCCR2A &= ~(1 << WGM20);
	TCCR2A &= ~(1 << WGM21);
	TCCR2B &= ~(1 << WGM22);
	
	
	//set prescaler to 1
	TCCR2B |= (1 << CS20);
	TCCR2B &= ~(1 << CS21);
	TCCR2B &= ~(1 << CS22);
	//set output compare register to 209 for 38KHz
	if (recieveSpeed)
	{
		OCR2B = 118;
		
	}
	else
	{
		OCR2B = 182;
	}
	//enable output compare interrupt
	TIMSK2 |= (1 << OCIE2B);
	sei();
	setIR(false);
	
}
void IR::initRecieve(){
	
	DDRD &= ~(1<<PORTD2);
	PORTD &= ~(1<<PORTD2);
	EIMSK |= (1 << INT0);    /* enable INT0 */
	EICRA |= (1<<ISC00);    /*falling edge interrupt*/
	sei();  //enable interrupts
}
void IR::setIR(bool state){
	if (state){
		//set timer output to toggle on 38Khz
		TCCR2A |= (1<<COM2B0);
		TCCR2A &= ~(1<<COM2B1);
	}
	else{
		//disconnect timer output
		TCCR2A &= ~(1<<COM2B0);
		TCCR2A &= ~(1<<COM2B1);
	}
}
void IR::sendPulse(uint16_t length){
	unsigned long lastPulse = micros();
	setIR(true);
	while(micros() < lastPulse+length){
		
	}
	lastPulse = micros();
	setIR(false);
	while(micros() < lastPulse+SPACE){
		
	}
	lastPulse = micros();
}
void IR::buildPacket(uint8_t *packetBuffer ,uint32_t data){
	uint8_t packetIndex = 0;
	packetBuffer[packetIndex] = 4;
	packetIndex ++;
	for(int i = 0;i<32;i++){
		uint32_t masked = 0;
		masked = (data>>i) & 1;
		if (masked)	packetBuffer[packetIndex] = 2;
		else packetBuffer[packetIndex] = 1;
		packetIndex++;
	}
	packetBuffer[packetIndex] = 3;
}
void IR::sendPacket(uint8_t *packetBuffer){
	for (int i = 0;i<34;i++)
	{
		switch(packetBuffer[i]){
			case 1:
			sendPulse(LOW);
			break;
			case 2:
			sendPulse(HIGH);
			break;
			case 3:
			sendPulse(STOP);
			break;
			case 4:
			sendPulse(START);
			break;
		}
	}
}
void IR::decode(uint8_t *packetBuffer){
	uint8_t packetIndex = 0;
	for (int i = 0;i<BUFFERLENGTH;i++)
	{
		uint16_t pulseType = 0;
		for (int j = 4;j>0;j--)
		{
			if (recieveSpeed)
			{
				if (times[i] > decodeTimes56[j-1]&&times[i]< decodeTimes56[j])
				{
					pulseType = j;
				}
			}
			else{
				if (times[i] > decodeTimes38[j-1]&&times[i]< decodeTimes38[j])
				{
					pulseType = j;
				}
			}
			
		}
		if (pulseType != 0)
		{
			packetBuffer[packetIndex] = pulseType;
			packetIndex++;
			if (packetIndex >= 34)
			{
				return;
			}
		}
		
		
		
	}
}
uint32_t IR::parse(uint8_t *packetBuffer){
	uint32_t output=0;
	for (int i = 1;i<32;i++)
	{
		output |= (packetBuffer[i]-1)<<(i-1);
	}
	return output;
}

void IR::begin(){
	initRecieve();
	initSend();
}
void IR::run(){
	sendPacket(outPacket);
	decode(inPacket);
}
void IR::send(uint32_t data){
	buildPacket(outPacket,data);
}
uint32_t IR::read(){
	return parse(inPacket);
}