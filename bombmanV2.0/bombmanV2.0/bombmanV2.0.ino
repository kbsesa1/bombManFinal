
#define START 1000
#define STOP 800
#define HIGH 600
#define LOW 400
#define SPACE 200

uint16_t packet[] = {START,HIGH,LOW,HIGH,LOW,HIGH,LOW,HIGH,LOW,HIGH,STOP};
int main(void){
	init();
	DDRD |= (1<<PORTD3);
	initIRTimer(false);
	while(1){
		delay(100);
		sendTestMessage();	
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
	if (state)
	{
		//set timer output to toggle on 38Khz
		TCCR2A |= (1<<COM2B0);
		TCCR2A &= ~(1<<COM2B1);
	}
	else
	{
		//disconnect timer output
		TCCR2A &= ~(1<<COM2B0);
		TCCR2A &= ~(1<<COM2B1);
	}
	
}

void sendPulse(uint16_t micros){
	setIR(true);
	delayMicroseconds(micros);
	setIR(false);
	delayMicroseconds(SPACE);
}

void sendTestMessage(){
	for (int i = 0;i<sizeof(packet)/sizeof(int);i++)
	{
		sendPulse(packet[i]);
	}
}


