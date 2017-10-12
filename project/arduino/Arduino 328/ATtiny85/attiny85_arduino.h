#define HIGH				1
#define LOW					0
#define true				1
#define false				0

#define digitalHigh(a)  	PORTB = PORTB | 1 <<a
#define digitalLow(a)   	PORTB = PORTB & ~(1<<a)

#define pinOutput(a)    	DDRB = DDRB | 1 << a
#define pinInput(a)     	DDRB = DDRB & ~(1<<a)
#define pinInputPullup(a)   pinInput(a); digitalHigh(a)

#define digitalRead(bit)    ( ( (PINB & (1UL << (bit) ) ) == ( (1) << (bit) ) ) )
