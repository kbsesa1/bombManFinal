/*
 * AVRmillis.h
 *
 * Created: 28-11-2018 17:55:33
 *  Author: joost
 */ 


#ifndef AVRMILLIS_H_
#define AVRMILLIS_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

class MILLIS
{
	public:
	void init(unsigned long);
	unsigned long get ();
};

extern MILLIS millis;
#endif /* AVRMILLIS_H_ */