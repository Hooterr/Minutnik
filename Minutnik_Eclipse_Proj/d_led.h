/*
 * d_led.h
 *
 *  Created on: 2010-03-30
 *       Autor: Miros�aw Karda�
 */

#ifndef _d_led_h		// dyrektywy zabezpieczaj�ce przed wielokrotnym do��czaniem
#define _d_led_h		// tego samego pliku nag��wkowego je�li b�dzie do��czany
				// w wielu r�nych plikach programu

// definicje port�w i pin�w wy�wietlacza u�ywanych dla u�atwienia w programie jako sta�e preprocesora
#define 	LED_DATA 	PORTD	// port z pod��czonymi segmentami
#define		LED_DATA_DIR DDRD	// rejestr kierunku portu katod wy�wietlaczy
#define 	ANODY_PORT 	PORTC	// port z pod��czonymi anodami- 4 bity najm�odsze
#define 	ANODY_DIR 	DDRC	// rejestr kierunku portu anod wy�wietlaczy


#define 	CA4 	(1<<PC0)		// CA1 oznacza bit nr.0 portu C
#define 	CA3 	(1<<PC1)		// CA2 oznacza bit nr.1 portu C
#define 	CA2 	(1<<PC2)		// CA3 oznacza bit nr.2 portu C
#define 	CA1 	(1<<PC3)		// CA4 oznacza bit nr.3 portu C

#define MASKA_ANODY (CA1|CA2|CA3|CA4)

// definicje bit�w dla poszczeg�lnych segment�w LED
#define SEG_A (1<<5)
#define SEG_B (1<<7)
#define SEG_C (1<<3)
#define SEG_D (1<<1)
#define SEG_E (1<<0)
#define SEG_F (1<<6)
#define SEG_G (1<<4)
#define SEG_DP (1<<2)

#define NIC 	10
#define A 		11
#define G 		12
#define D 		13
#define B 		14
#define C 		15
#define AB 		16
#define ABC 	17
#define ABCD 	18
#define DE 		19
#define DEF		20
#define ADEF	21
#define EF 		22
#define BCEF	23
#define BC 		24
#define AD 		25
#define ADE		26




// same DEKLARACJE zmiennych globalnych na potrzeby wykorzystania ich w innych plikach projektu
// przydomek externpowoduje, i� te zmienne b�d� dost�pne we wszystkich modu�ach, kt�re do��cz� plik
// d_led.h za pomoc� dyrektywy #include
extern volatile uint8_t cy1;
extern volatile uint8_t cy2;
extern volatile uint8_t cy3;
extern volatile uint8_t cy4;



void d_led_init(void);
void d_led_off(void);
void d_led_on(void);

#endif	// koniec _d_led_h

