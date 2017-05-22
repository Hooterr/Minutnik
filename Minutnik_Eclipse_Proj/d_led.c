/*
 * d_led.c
 *
 *  Created on: 2010-03-30
 *       Autor: Miros�aw Karda�
 */
#include <avr/io.h>			// do��czenie g��wnego systemowego  pliku nag��wkowego
#include <avr/interrupt.h>	// do��czenie pl. nag��wkowego potrzebnego do obs�. przerwa�
#include <avr/pgmspace.h>	// do��czenie pl. nag��wkowego potrzebnego do odczytu
							// danych zawartych w pami�ci programu FLASH

#include "d_led.h"			// do��czenie naszego pliku nag��wkowego
							// w nim znajduj� si� potrzebne tu m.in. definicje preprocesora

// definicje zmiennych globalnych przechowuj�cych cyfry do wy�wietlania
// volatile � poniewa� b�d� wykorzystywane do odczytu i zapisu zar�wno w przerwaniu i programie
// g��wnym. Trzeba wi�c wy��czy� optymalizacj� dost�pu do nich. (zmienne ulotne)
volatile uint8_t cy1;
volatile uint8_t cy2;
volatile uint8_t cy3;
volatile uint8_t cy4;

// definicja tablicy zawieraj�cej definicje bitowe cyfr LED
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



// ****** definicja funkcji inicjalizuj�cej prac� z wy�wietlaczem multipleksowanym
void d_led_init(void)
{
	LED_DATA_DIR = 0xFF;   					// wszystkie piny portu C jako WYJ�CIA(katody)
	LED_DATA = 0xFF;						// wygaszenie wszystkich katod � stan niski
	ANODY_DIR |= CA1 | CA2 | CA3 | CA4;		// 4 piny portu A jako WYJ�CIA (anody wy�wietlaczy)
	ANODY_PORT |= CA1 | CA2 | CA3  | CA4;	// wygaszenie wszystkich wy�wietlaczy - anody

	// ustawienie TIMER0
	TCCR0A |= (1<<WGM01);				// tryb CTC
	TCCR0B |= (1<<CS02)|(1<<CS00);		// preskaler = 1024
	OCR0A = 38;							// dodatkowy podzia� przez 39 (rej. przepe�nienia)
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

// ================= PROCEDURA OBS�UGI PRZERWANIA � COMPARE MATCH
ISR(TIMER0_COMPA_vect)
{
	static uint8_t licznik=1;		// zmienna do prze��czania kolejno anod wyrwietlacza

	ANODY_PORT &= ~MASKA_ANODY;	// wygaszenie wszystkich wy�wietlaczy

	if(licznik==1) 		LED_DATA = (LED_DATA & (1<<2)) | (pgm_read_byte( &cyfry[cy4] ) & ~(1<<2));	// gdy zapalony wy�w.1 podaj stan zmiennej c1
	else if(licznik==2) LED_DATA = (LED_DATA & (1<<2)) | (pgm_read_byte( &cyfry[cy3] ) & ~(1<<2));	// gdy zapalony wy�w.2 podaj stan zmiennej c2
	else if(licznik==4) LED_DATA = (LED_DATA & (1<<2)) | (pgm_read_byte( &cyfry[cy2] ) & ~(1<<2));	// gdy zapalony wy�w.3 podaj stan zmiennej c3
	else if(licznik==8) LED_DATA = (LED_DATA & (1<<2)) | (pgm_read_byte( &cyfry[cy1] ) & ~(1<<2));	// gdy zapalony wy�w.4 podaj stan zmiennej c4

	ANODY_PORT = (ANODY_PORT & MASKA_ANODY) | ~(licznik & MASKA_ANODY);			// cykliczne prze��czanie kolejnej anody w ka�dym przerwaniu
	// operacje cyklicznego przesuwania bitu zapalaj�cego anody w zmiennej licznik
	licznik <<= 1;					// przesuni�cie zawarto�ci bit�w licznika o 1 w lewo
	if(licznik>8) licznik = 1;		// je�li licznik wi�kszy ni� 8 to ustaw na 1
}
