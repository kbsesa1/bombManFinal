/*
 * CPPFile1.cpp
 *
 * Created: 28-11-2018 17:55:00
 *  Author: joost
 */ 
#include "AVRmillis.h"

MILLIS millis;

volatile unsigned long timer1_millis;

ISR (TIMER1_COMPA_vect)
{
	timer1_millis++;
}

void MILLIS::init(unsigned long F_CPU){
	unsigned int CTC_MATCH_OVERFLOW = ((F_CPU / 1000) / 8);
	// CTC mode, Clock/8
	TCCR1B |= (1 << WGM12) | (1 << CS11);
	
	// Load the high byte, then the low byte
	// into the output compare
	OCR1AH = (CTC_MATCH_OVERFLOW >> 8);
	OCR1AL = CTC_MATCH_OVERFLOW;
	
	// Enable the compare match interrupt
	TIMSK1 |= (1 << OCIE1A);
	

	// Now enable global interrupts
	sei();
}

unsigned long MILLIS::get ()
{
	unsigned long millis_return;

	// Ensure this cannot be disrupted
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		millis_return = timer1_millis;
	}
	
	return millis_return;
}