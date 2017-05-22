#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>


#include "d_led.h"

//zmienne repezentuj¹ce kolejne cyfry wyœwietlacza
volatile uint8_t cy1;
volatile uint8_t cy2;
volatile uint8_t cy3;
volatile uint8_t cy4;

// definicja tablicy zawieraj¹cej definicje bitowe cyfr LED
const uint8_t cyfry[30] PROGMEM = {
		~(SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F),			// 0
		~(SEG_B|SEG_C),									// 1
		~(SEG_A|SEG_B|SEG_D|SEG_E|SEG_G),				// 2
		~(SEG_A|SEG_B|SEG_C|SEG_D|SEG_G),				// 3
		~(SEG_B|SEG_C|SEG_F|SEG_G),						// 4
		~(SEG_A|SEG_C|SEG_D|SEG_F|SEG_G),				// 5
		~(SEG_A|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G),			// 6
		~(SEG_A|SEG_B|SEG_C|SEG_F),						// 7
		~(SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G),	// 8
		~(SEG_A|SEG_B|SEG_C|SEG_D|SEG_F|SEG_G),			// 9
		0xFF,											// NIC (puste miejsce)
		~(SEG_A),
		~(SEG_G),
		~(SEG_D),
		~(SEG_B),
		~(SEG_C),
		~(SEG_A|SEG_B),
		~(SEG_A|SEG_B|SEG_C),
		~(SEG_A|SEG_B|SEG_C|SEG_D),
		~(SEG_D|SEG_E),
		~(SEG_D|SEG_E|SEG_F),
		~(SEG_D|SEG_E|SEG_F|SEG_A),
		~(SEG_F|SEG_E),
		~(SEG_F|SEG_E|SEG_B|SEG_C),
		~(SEG_B|SEG_C),
		~(SEG_A|SEG_D),
		~(SEG_D|SEG_E|SEG_F|SEG_A),
};



// ****** definicja funkcji inicjalizuj¹cej pracê z wyœwietlaczem multipleksowanym
void d_led_init(void)
{
	LED_DATA_DIR = 0xFF;   					// wszystkie piny portu C jako WYJŒCIA(katody)
	LED_DATA = 0xFF;						// wygaszenie wszystkich katod – stan niski
	ANODY_DIR |= CA1 | CA2 | CA3 | CA4;		// 4 piny portu A jako WYJŒCIA (anody wyœwietlaczy)
	ANODY_PORT |= CA1 | CA2 | CA3  | CA4;	// wygaszenie wszystkich wyœwietlaczy - anody

	// ustawienie TIMER0
	TCCR0A |= (1<<WGM01);				// tryb CTC
	TCCR0B |= (1<<CS02)|(1<<CS00);		// preskaler = 1024
	OCR0A = 38;							// dodatkowy podzia³ przez 39 (rej. przepe³nienia)
	TIMSK0 |= (1<<OCIE0A);				// zezwolenie na przerwanie CompareMatch
}

void d_led_on(void)
{
	TIMSK0 |= (1<<OCIE0A);
}

void d_led_off(void)
{
	TIMSK0 &= ~(1<<OCIE0A);
	ANODY_PORT |= MASKA_ANODY;
}

// ================= PROCEDURA OBS£UGI PRZERWANIA – COMPARE MATCH
ISR(TIMER0_COMPA_vect) //multiplexowanie
{
	static uint8_t licznik=1;

	ANODY_PORT &= ~MASKA_ANODY;

	if(licznik==1) 		LED_DATA = (LED_DATA & (1<<2)) | (pgm_read_byte( &cyfry[cy4] ) & ~(1<<2));
	else if(licznik==2) LED_DATA = (LED_DATA & (1<<2)) | (pgm_read_byte( &cyfry[cy3] ) & ~(1<<2));
	else if(licznik==4) LED_DATA = (LED_DATA & (1<<2)) | (pgm_read_byte( &cyfry[cy2] ) & ~(1<<2));
	else if(licznik==8) LED_DATA = (LED_DATA & (1<<2)) | (pgm_read_byte( &cyfry[cy1] ) & ~(1<<2));

	ANODY_PORT = (ANODY_PORT & MASKA_ANODY) | ~(licznik & MASKA_ANODY);
	licznik <<= 1;
	if(licznik>8) licznik = 1;
}

