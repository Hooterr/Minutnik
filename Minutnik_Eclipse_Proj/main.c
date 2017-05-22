/*
 * main.c
 *
 *  Created on: 2017-03-30
 *       Autor: Maksymilian Lach
 */

#include <avr/io.h>		    // do³¹czenie g³ównego systemowego  pliku nag³ówkowego
#include <avr/interrupt.h>  // systemowa biblioteka obs³uguj¹ca przerwania
#include <util/delay.h>		// systemowa biblioteka obs³uguj¹ca opóŸnienia
#include <avr/sleep.h>		// systemowa biblioteka obs³uguj¹ca tryp power down


#include "LED/d_led.h"		 // biblioteka do obs³ugi wyœwietlacza
#include "ENCODER/encoder.h" // biblioteka do obs³ugi enkodera
#include "makra.h"			 // makra programowe


volatile uint16_t Timer1, Timer2, Timer3, Timer4;	/* timery programowe 100Hz */

void enco(void);				//callback biblioteki enkodera
void long_click(void);			//callback d³ugie przytrzymania przycisku
void fast_click(void);			//callback szybkiego przyciœniêca przycisku
void cutDown(void);				//funkcja odliczaj¹ca
void Beebing(void);				//funkcja sygnalizuj¹ca up³yniêcie czasu
void SecondsIncrement(void);	//inkrementacja sekund
void MinutesIncrement(void);	//inkremantacje minut
void SecondsDecrement(void);	//dekrementacja sekund
void MinutesDecrement(void);	//dekrementacja minut
void StartUp(void);				//animacja startowa

//funkcja obs³ugujaca przyciski
void SuperDebounce(uint8_t * key_state, volatile uint8_t *KPIN,
		uint8_t key_mask, uint16_t rep_time, uint16_t rep_wait,
		void (*push_proc)(void), void (*rep_proc)(void) );
uint8_t k1, k2;

enum //enum opisuj¹cy stan urz¹dzenia
{
	cutdown,
	settingMinutes,
	settingSeconds,
	beeping,
};

uint8_t operation = settingMinutes;

int main(void)
{
	// ****** inicjalizacja *********
	d_led_init();   		// inicjalizacja wyœwietlacza multipleksowanego
	mk_encoder_init();
	register_enc_event_callback(enco); // rejestracja funkcji callback biblioteki do obs³ugi enkodera
	DDRB |= BUZZER_PIN;
	DDRB &=  ~SWITCH_PIN;
	PORTB |= SWITCH_PIN;


	TCCR2A 	|= (1<<WGM21);			// tryb pracy CTC
	TCCR2B 	|= (1<<CS22)|(1<<CS20)|(1<<CS21);	// preskaler = 1024
	OCR2A 	= 107;					// przerwanie porównania co 10ms (100Hz)
	TIMSK2	|= (1<<OCIE2A);			// Odblokowanie przerwania CompareMatch

	PCICR |= (1<<PCIE0); 			//przerwanie zewnêtrzene PCINT, s³u¿ace do wybudzenia procesora,
	PCIFR |= (1<<PCIF0);			//reakcje na dowolne zbocze
	PCMSK0 &= ~(1<<PCINT5);

	SMCR |= (1<<SM1);

	cy1 = NIC;
	cy2 = NIC;
	cy3 = NIC;
	cy4 = NIC;
	sei();

	StartUp();
	cy1 = 0;
	cy2 = 5;
	cy3 = 0;
	cy4 = 0;

	Timer4 = 3000; //odliczanie czasu do usypienia

	while(1) // pêtla g³ówna
	{
		ENCODER_EVENT();
		SuperDebounce(&k1, &PINB, SWITCH_PIN, 1, 25, fast_click, long_click);
		if(!Timer2)
		{
			Timer2=50; // co 500ms migamy dwukropkiem
			PORTD ^= (1<<PD2);
			if(operation == cutdown) //odliczanie
			{
				cutDown();
			}
		}
		if(!Timer3 && operation == beeping)
		{
			Beebing();
		}
		if(!Timer4 && operation != cutdown)
		{
			TIMSK2 &= ~(1<<OCIE2A); //uœpienie procesora
			d_led_off();
			PCMSK0 |= (1<<PCINT5);
			sleep_mode();
		}
	}
}

void Beebing(void)
{
	static int8_t i = 0;
	switch(i)
	{
	case 0:
		d_led_on();
		BEEP;
		Timer3 = 20;
		break;
	case 1:
		d_led_off();
		NOBEEP;
		Timer3 = 20;
		break;
	case 2:
		BEEP;
		d_led_on();
		Timer3 = 20;
		break;
	case 3:
		NOBEEP;
		d_led_off();
		Timer3 = 20;
		break;
	case 4:
		BEEP;
		d_led_on();
		Timer3 = 20;
		break;
	case 5:
		NOBEEP;
		d_led_off();
		Timer3 = 60;
		i = -1;
		break;
	}
	i++;
}

void cutDown(void)
{
	static uint8_t pom = 0;

	if(pom > 1)
	{
		SecondsDecrement();
		pom = 0;
	}
	if(!cy1 && !cy2 && !cy3 && !cy4)
	{
		Timer4 = 3000;
		operation = beeping;
	}
	pom++;
}

void fast_click()
{
	Timer4 = 3000;
	if(operation == cutdown || operation == beeping)
	{
		operation = settingMinutes;
		NOBEEP;
		d_led_on();
	}
	else if(operation == settingMinutes)
		operation = settingSeconds;
	else if(operation == settingSeconds)
		operation = settingMinutes;
}

void long_click()
{
	Timer4 = 3000;
	if(operation == cutdown)
	{
		operation = settingMinutes;
		NOBEEP;
		d_led_on();
	}
	else if(operation == beeping)
	{
		operation = settingMinutes;
		NOBEEP;
		d_led_on();
		cy1 = 0;
		cy2 = 5;
		cy3 = 0;
		cy4 = 0;
	}
	else
		operation = cutdown;
}


void enco(void)
{
	Timer4 = 3000;
	PORTB |= BUZZER_PIN;
	_delay_ms(3);
	PORTB &= ~BUZZER_PIN;
	if(operation == beeping)
	{
		operation = settingMinutes;
		d_led_on();
	}
	if(enco_dir == ENC_RIGHT)
	{
		if(operation == settingMinutes || operation == cutdown)
		{
			MinutesIncrement();
		}
		else if(operation == settingSeconds)
		{
			SecondsIncrement();
		}
	}
	else
	{
		if(operation == settingMinutes || operation == cutdown)
		{
			MinutesDecrement();
		}
		else if(operation == settingSeconds)
		{
			SecondsDecrement();
		}
	}
}

void MinutesIncrement(void)
{
	if(cy2 >= 9)
	{
		cy2 = 0;
		if(cy1 >=9)
		{
			cy1 = 0;
			if(!cy4 && !cy3)
				cy2 = 1;
		}
		else
			cy1++;
	}
	else
	{
		cy2++;
	}
}

void SecondsIncrement(void)
{
	if(cy4 >= 9)
	{
		cy4 = 0;
		if(cy3 >= 5)
		{
			cy3 = 0;
			MinutesIncrement();
			if(!cy1 && !cy2)
				cy4 = 1;
		}
		else
		{
			cy3++;
		}
	}
	else
	{
		cy4++;
	}
}

void MinutesDecrement(void)
{
	if(cy2 <= 0)
	{
		cy2 = 9;
		if(cy1 <= 0)
		{
			cy1 = 9;
		}
		else
		{
			cy1--;
		}
	}
	else
	{
		cy2--;
		if(!cy1 && !cy2 && !cy3 && !cy4)
		{
			cy1 = 9;
			cy2 = 9;
			cy3 = 0;
			cy4 = 0;
		}
	}
}

void SecondsDecrement(void)
{
	if(cy4 <=0)
	{
		cy4 = 9;
		if(cy3 <= 0)
		{
			cy3 = 5;
			MinutesDecrement();
		}
		else
		{
			cy3 --;
		}
	}
	else
	{
		cy4--;
		if(!cy1 && !cy2 && !cy3 && !cy4 && operation != cutdown)
		{
			cy1 = 9;
			cy2 = 9;
			cy3 = 0;
			cy4 = 0;
		}
	}
}
/************** funkcja SuperDebounce do obs³ugi pojedynczych klawiszy ***************
 * 							AUTOR: Miros³aw Kardaœ
 * ZALETY:
 * 		- nie wprowadza najmniejszego spowalnienia
 * 		- posiada funkcjê REPEAT (powtarzanie akcji dla d³u¿ej wciœniêtego przycisku)
 * 		- mo¿na przydzieliæ ró¿ne akcje dla trybu REPEAT i pojedynczego klikniêcia
 * 		- mo¿na przydzieliæ tylko jedn¹ akcjê wtedy w miejsce drugiej przekazujemy 0 (NULL)
 *
 * Wymagania:
 * 	Timer programowy utworzony w oparciu o Timer sprzêtowy (przerwanie 100Hz)
 *
 * 	Parametry wejœciowe:
 *
 * 	*key_state - wskaŸnik na zmienn¹ w pamiêci RAM (1 bajt) - do przechowywania stanu klawisza
 *  *KPIN - nazwa PINx portu na którym umieszczony jest klawisz, np: PINB
 *  key_mask - maska klawisza np: (1<<PB3)
 *  rep_time - czas powtarzania funkcji rep_proc w trybie REPEAT
 *  rep_wait - czas oczekiwania do przejœcia do trybu REPEAT
 *  push_proc - wskaŸnik do w³asnej funkcji wywo³ywanej raz po zwolenieniu przycisku
 *  rep_proc - wskaŸnik do w³asnej funkcji wykonywanej w trybie REPEAT
 **************************************************************************************/
void SuperDebounce(uint8_t * key_state, volatile uint8_t *KPIN,
		uint8_t key_mask, uint16_t rep_time, uint16_t rep_wait,
		void (*push_proc)(void), void (*rep_proc)(void) ) {

	enum {idle, debounce, go_rep, wait_rep, rep, wait_after_rep};

	if(!rep_time) rep_time=20;
	if(!rep_wait) rep_wait=150;

	uint8_t key_press = !(*KPIN & key_mask);

	if( key_press && !*key_state ) {
		*key_state = debounce;
		Timer1 = 15;
	} else
	if( *key_state  ) {

		if( key_press && debounce==*key_state && !Timer1 ) {
			*key_state = 2;
			Timer1=5;
		} else
		if( !key_press && *key_state>1 && *key_state<4 ) {
			if(push_proc) push_proc();						/* KEY_UP */
			*key_state=idle;
		} else
		if( key_press && go_rep==*key_state && !Timer1 ) {
			*key_state = wait_rep;
			Timer1=rep_wait;
		} else
		if( key_press && wait_rep==*key_state && !Timer1 ) {
			*key_state = rep;
		} else
		if( key_press && rep==*key_state && !Timer1 ) {
			Timer1 = rep_time;
			if(rep_proc) rep_proc();
			*key_state = wait_after_rep;
		} //else
		//if( key_press && wait_after_rep==*key_state)

	}
	if( *key_state>=3 && !key_press ) *key_state = idle;
}

void StartUp(void) //animacja przy starcie
{
#define D_TIME1 80
#define D_TIME2 80
	PORTB |= (1<<PB5);
	cy1 = NIC;
	cy2 = NIC;
	cy3 = NIC;
	cy4 = NIC;

	cy1 = A;
	cy2 = A;
	cy3 = A;
	cy4 = A;
	_delay_ms(D_TIME1);
	cy1 = G;
	cy2 = G;
	cy3 = G;
	cy4 = G;
	_delay_ms(D_TIME1);
	cy1 = D;
	cy2 = D;
	cy3 = D;
	cy4 = D;
	_delay_ms(D_TIME1);
	cy1 = G;
	cy2 = G;
	cy3 = G;
	cy4 = G;
	_delay_ms(D_TIME1);
	cy1 = A;
	cy2 = A;
	cy3 = A;
	cy4 = A;
	_delay_ms(D_TIME1);
	cy1 = NIC;
	cy2 = NIC;
	cy3 = NIC;
	cy4 = NIC;
	_delay_ms(D_TIME2);
	cy1 = EF;
	cy2 = NIC;
	cy3 = NIC;
	cy4 = NIC;
	_delay_ms(D_TIME2);
	cy1 = BC;
	cy2 = NIC;
	cy3 = NIC;
	cy4 = NIC;
	_delay_ms(D_TIME2);
	cy1 = NIC;
	cy2 = EF;
	cy3 = NIC;
	cy4 = NIC;
	_delay_ms(D_TIME2);
	cy1 = NIC;
	cy2 = BC;
	cy3 = NIC;
	cy4 = NIC;
	_delay_ms(D_TIME2);
	cy1 = NIC;
	cy2 = NIC;
	cy3 = EF;
	cy4 = NIC;
	_delay_ms(D_TIME2);
	cy1 = NIC;
	cy2 = NIC;
	cy3 = BC;
	cy4 = NIC;
	_delay_ms(D_TIME2);
	cy1 = NIC;
	cy2 = NIC;
	cy3 = NIC;
	cy4 = EF;
	_delay_ms(D_TIME2);
	cy1 = NIC;
	cy2 = NIC;
	cy3 = NIC;
	cy4 = BC;
	_delay_ms(D_TIME2);
	cy1 = NIC;
	cy2 = NIC;
	cy3 = NIC;
	cy4 = NIC;

	_delay_ms(D_TIME2);
	cy1 = A;
	_delay_ms(D_TIME2);
	cy2 = A;
	_delay_ms(D_TIME2);
	cy3 = A;
	_delay_ms(D_TIME2);
	cy4 = A;
	_delay_ms(D_TIME2);
	cy4 = AB;
	_delay_ms(D_TIME2);
	cy4 = ABC;
	_delay_ms(D_TIME2);
	cy4 = ABCD;
	_delay_ms(D_TIME2);
	cy3 = AD;
	_delay_ms(D_TIME2);
	cy2 = AD;
	_delay_ms(D_TIME2);
	cy1 = AD;
	_delay_ms(D_TIME2);
	cy1 = ADE;
	_delay_ms(D_TIME2);
	cy1 = ADEF;
}


ISR(PCINT0_vect) //procedura obs³ugi przerwania PCINT wybudzj¹cege procesor
{
	sleep_disable();
	d_led_on();
	TIMSK2	|= (1<<OCIE2A);
	PCMSK0 &= ~(1<<PCINT5);
	Timer4 = 3000;
}

ISR(TIMER2_COMPA_vect) // timery programowe
{
	uint16_t n;

	n = Timer1;		/* 100Hz Timer1 */
	if (n) Timer1 = --n;
	n = Timer2;		/* 100Hz Timer2 */
	if (n) Timer2 = --n;
	n = Timer3;
	if (n) Timer3 = --n;
	n = Timer4;
	if (n) Timer4 = --n;
}
