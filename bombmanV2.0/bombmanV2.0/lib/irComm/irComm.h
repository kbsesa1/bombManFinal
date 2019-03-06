// irComm.h

#ifndef _IRCOMM_h
#define _IRCOMM_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
class IR
{
public:
IR(bool highFrequency);
void begin();
void run();
void send(uint32_t data);
uint32_t read();
protected:
private:
void initSend();
void initRecieve();
void setIR(bool state);
void sendPulse(uint16_t length);
void buildPacket(uint8_t *packetBuffer ,uint32_t data);
void sendPacket(uint8_t *packetBuffer);
void decode(uint8_t *packetBuffer);
uint32_t parse(uint8_t *packetBuffer);
};

#endif

