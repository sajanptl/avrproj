#ifndef F_CPU
#define F_CPU 16000000UL 
#endif

#include <avr/io.h>
#include <util/delay.h>

int main()
{
	DDRD = 0x40;
	while (1)
  	{
    	PORTD = 0x40;
    	_delay_ms(50);

    	PORTD = 0x00;
    	_delay_ms(50);
  	}
}
