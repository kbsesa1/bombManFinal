/*
* IRcomm.h
*
* Created: 28-11-2018 13:34:10
*  Author: joost
*/


#ifndef IRCOMM_H_
#define IRCOMM_H_

#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdlib.h>
#include "SimpleUart.h"

class IR
{
	public:
	IR();
	void Init(int,int);
	char Recieve();
	int Available();
	bool rawAvailable();
	void Transmit(char);
	char* printRaw();

};






#endif /* IRCOMM_H_ */