#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

//definiciones arduino-like
#include "attiny85_arduino.h"
//arrays de samples
#include "sounds.h"

//pines 
#define SND_GND		1
#define BEAT_CLICK	0
#define SND0_CLICK	2
#define SND1_CLICK	3
#define SND_OUT		4

//constantes
const unsigned int snd1_beat1_len  	= sizeof(snd1_beat1);
const unsigned int snd1_beat_len  	= sizeof(snd1_beat);
const unsigned int snd2_beat1_len  	= sizeof(snd2_beat1);
const unsigned int snd2_beat_len  	= sizeof(snd2_beat);
const unsigned int snd3_beat1_len  	= sizeof(snd3_beat1);
const unsigned int snd3_beat_len  	= sizeof(snd3_beat);

//variables globales
#ifdef DEBUG
unsigned int sound_len 		= 11; //como no simula la velocidad de interrupcion, le ponemos menos samples
volatile unsigned char test = 0;
#else
unsigned int sound_len 		= 0;
#endif

unsigned int sample_index 		= 0;
uint8_t played 					= false;
volatile uint8_t is_playing 	= false;
uint8_t sound 					= 0;
uint8_t beat 					= 0;


//interrupcion para reproducir el siguiente sample
ISR(TIMER0_COMPA_vect) {
	//leemos el siguiente sample del sonido seleccionado
	unsigned char sample = 0;
	//comprobamos sonido seleccionado
	switch (sound){
		case 1:
			if (beat){
				sample = pgm_read_byte(&snd1_beat1[sample_index++]);	
				#ifndef DEBUG 
					sound_len = snd1_beat1_len;
				#endif
			}else{
				sample = pgm_read_byte(&snd1_beat[sample_index++]);	
				#ifndef DEBUG 
					sound_len = snd1_beat_len;
				#endif
			}break;
		case 2:
			if (beat){
				sample = pgm_read_byte(&snd2_beat1[sample_index++]);
				#ifndef DEBUG 
					sound_len = snd2_beat1_len;
				#endif	
			}else{
				sample = pgm_read_byte(&snd2_beat[sample_index++]);	
				#ifndef DEBUG 
					sound_len = snd2_beat_len;
				#endif
			}break;
		case 3:
			if (beat){
				sample = pgm_read_byte(&snd3_beat1[sample_index++]);	
				#ifndef DEBUG 
					sound_len = snd3_beat1_len;
				#endif
			}else{
				sample = pgm_read_byte(&snd3_beat[sample_index++]);	
				#ifndef DEBUG 
					sound_len = snd3_beat_len;
				#endif
			}break;		
	}
	//debug
	#ifdef DEBUG
	test = sample;
	#endif
	//seteamos valor PMW de la salida A
	OCR1A = sample; 
	//seteamos valor PMW invertido de la salida B (asi no hace falta condensador de acoplamiento AC)
	OCR1B = sample ^ 255;
	//si hemos llegado al final del sample
	if (sample_index == sound_len) {
		//bajamos flag reproduccion
		is_playing = false;
		//desactivamos la interrupcion
		TIMSK = 0;    
	}
}

//reproducir sonido
void play()
{
	//no empieza un sonido hasta terminar el actual
	if (!is_playing)
	{
		//obtenemos el numero de sonido y el beat
		sound = (PINB & 0b00001100)>>2;
		beat  = (PINB & 0b00000001);
		//subimos flag reproduccion
		is_playing = true;
		//comenzamos audio desde el principio
		sample_index = 0; 	
		//activamos la interrupcion Timer0_COMPA
		TIMSK = 1<<OCIE0A;
	}
}

//inicializacion
void setup()
 { 
	//necesario arrancar las interrupciones porque en avr vienen desactivadas por defecto
	sei(); 

	// Enable 64 MHz PLL and use as source for Timer1
	PLLCSR = 1<<PCKE | 1<<PLLE;     

	// Set up Timer/Counter1 for PWM output
	TIMSK = 0;                              // Timer interrupts OFF
	//debug proteus (no parece ser capaz de simular el PLL a 64Mhz)
	#ifndef DEBUG
	TCCR1 = 1<<PWM1A | 2<<COM1A0 | 1<<CS10; // PWM A, clear on match, 1:1 prescale
	#endif
	GTCCR = 1<<PWM1B | 2<<COM1B0;           // PWM B, clear on match
	OCR1A = 128; OCR1B = 128;               // 50% duty at start


	// Set up Timer/Counter0 for 8kHz interrupt to output samples.
	TCCR0A = 3<<WGM00;                      // Fast PWM
	TCCR0B = 1<<WGM02 | 2<<CS00;            // 1/8 prescale
	//TIMSK = 1<<OCIE0A;                      // Enable compare match
	OCR0A = 124;                            // Divide by 1000
	
	//configuramos los pines
	pinOutput(SND_OUT);
	pinOutput(SND_GND);

	pinInputPullup(BEAT_CLICK);
	pinInputPullup(SND0_CLICK);
	pinInputPullup(SND1_CLICK);
}

//bucle principal
void loop()
   {
	//si se activa cualquier de los 2 bits de sonido click
	if ((PINB & 0b00001100) > 0 && !played){
		play();
		played = true;
	}
	
	//si desactivan los dos bits de sonido click
	if ((PINB & 0b00001100) == 0){
		played = false;
	}	
}

//estructura programa estilo arduino
int main() {
  setup();
  for(;;) loop();
}
