
#include "Arduino.h"


void InitTimer()
{
	const long frequency = 40000L;  // Hz
  TCCR2A = _BV (WGM20) | _BV (WGM21) | _BV (COM2B1); // fast PWM, clear OC2B on compare
  TCCR2B = _BV (WGM22) | _BV (CS21);         // fast PWM, prescaler of 8
  OCR2A =  ((F_CPU / 8) / frequency) - 1;    // zero relative 
  OCR2B = ((OCR2A + 1) / 2) - 1;             // 50% duty cycle
	
}


void reset_timer()
{
  TCNT2 = 0; 
  TCCR2A = _BV (WGM20) | _BV (WGM21) | _BV (COM2B1); // fast PWM, clear OC2B on compare 
  TCCR2B = _BV (WGM22) | _BV (CS21);
}

void disable_timer()
{
  TCCR2A = 0; 
}

void trigger()
{

long tdif, tbgn, tend, tl, th; 
int t = 0;

tbgn = micros();
reset_timer();
delayMicroseconds(160);//8x20 uS
disable_timer();

}
