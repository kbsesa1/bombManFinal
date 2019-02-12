
#define START 1000
#define STOP 800
#define HIGH 600
#define LOW 400
#define SPACE 200

uint16_t outPacket[34];
uint32_t sendBuffer = 0;
int main(void){
	init();
	Serial.begin(115200);
	initIRTimer(false);
	sendBuffer = 255;
	while(1){
		delay(50);
		buildPacket(outPacket,sendBuffer);
		printPacket(outPacket);
		sendPacket(outPacket);
		sendBuffer++;
	}
	return 0;
}

void initIRTimer(bool highspeed){
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
	if (highspeed)
	{
		OCR2B = 125;
		
	}
	else
	{
		OCR2B = 192;
	}
	//enable output compare interrupt
	TIMSK2 |= (1 << OCIE2B);
	sei();
	setIR(false);
	
}
ISR(TIMER2_COMPB_vect){
	TCNT2 = 0;
}
void setIR(bool state){
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
void sendPulse(uint16_t length){
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
void buildPacket(uint16_t *packetBuffer ,uint32_t data){
	Serial.println("building packet!");
	uint8_t packetIndex = 0;
	packetBuffer[packetIndex] = START;
	packetIndex ++;
	for(int i = 0;i<32;i++){
		uint32_t masked = 0;
		masked = (data>>i) & 1;
		if (masked)	packetBuffer[packetIndex] = HIGH;
		else packetBuffer[packetIndex] = LOW;
		packetIndex++;
	}
	packetBuffer[packetIndex] = STOP;
}
void printPacket(uint16_t *packetBuffer){
	Serial.println("packet contents:");
	Serial.print("(");
	for (int i = 0;i<34;i++){
		switch(packetBuffer[i]){
			case START:
			Serial.print("ST,");
			break;
			case HIGH:
			Serial.print("H,");
			break;
			case LOW:
			Serial.print("L,");
			break;
			case STOP:
			Serial.print("STP");
			break;
		}
	}
	Serial.println(")");
}
void sendPacket(uint16_t *packetBuffer){
	for (int i = 0;i<34;i++)
	{
		sendPulse(packetBuffer[i]);
	}
}
void sendTestMessage(){
	for (int i = 0;i<sizeof(outPacket)/sizeof(int);i++)
	{
		sendPulse(outPacket[i]);
	}
}



