/*
 * SPI.h
 *
 * Created: 5-12-2018 10:32:18
 *  Author: joost
 */ 


#ifndef SPI_H_
#define SPI_H_

#include <avr/io.h>

class SPI
{
	public:
	SPI(volatile uint8_t *CS_state,volatile uint8_t *CS_PORT, uint8_t CS_portNumber, uint8_t speed);
	void init();
	void transmit(uint8_t b);
	void transmit16(uint16_t i);
	void transmit32(uint32_t l);
	void beginTransmission();
	void endTransmission();
	uint8_t transfer(uint8_t out);
	bool transferComplete();
	void setSpeed(uint8_t speed);
};

#endif /* SPI_H_ */