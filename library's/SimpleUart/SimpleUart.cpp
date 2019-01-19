/*
 * SimpleUart.cpp
 *
 * Created: 28-11-2018 13:34:47
 *  Author: joost
 */ 
/*
 * SimpleUart.c
 *
 * Created: 25-11-2018 22:16:00
 *  Author: joost
 */ 

#include "SimpleUart.h"

UART::UART(){
	
}

void UART::Init( unsigned long f_cpu,unsigned long baud)
{
	/*calculate baud register*/
unsigned int ubrr = f_cpu/16/baud-1;
ubrr = 8;
	/*Set baud rate */
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	/*Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	/* Set frame format: 8data, 2stop bit */
	UCSR0C = (3<<UCSZ00);
}
void UART::Transmit( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) )
	;
	/* Put data into buffer, sends the data */
	UDR0 = data;
}
unsigned char UART::Receive()
{
	/* Wait for data to be received */
	while ( !(UCSR0A & (1<<RXC0)) )
	;
	/* Get and return received data from buffer */
	return UDR0;
}
void UART::PutCString(const char* str){
	
	char message[strlen(str)+1];
	strcpy (message,str);
	message[sizeof(message)-1] = '\0';
	
	for (uint32_t i = 0;i<sizeof(message)-1;i++)
	{
		Transmit(message[i]);
	}
}
void UART::PutInt(int32_t input){
	char str[20];
	sprintf(str, "%ld", input);
	PutCString(str);
}
void UART::PutUInt(uint32_t input){
	char str[20];
	sprintf(str, "%lu", input);
	PutCString(str);
}
void UART::PutHex(uint8_t data){
	uint8_t char1 = (data >> 4) & 0x0F;
	uint8_t char2 = (data & 0x0F);
	if (char1 >= 0x0A) char1 += 55;
	else char1 += 48;
	if (char2 >= 0x0A) char2 += 55;
	else char2 += 48;
	Transmit(char1);
	Transmit(char2);
	
}
 