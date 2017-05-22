#ifndef _d_led_h
#define _d_led_h

#define 	LED_DATA 	PORTD	// port z pod³¹czonymi segmentami
#define		LED_DATA_DIR DDRD	// rejestr kierunku portu katod wyœwietlaczy
#define 	ANODY_PORT 	PORTC	// port z pod³¹czonymi anodami- 4 bity najm³odsze
#define 	ANODY_DIR 	DDRC	// rejestr kierunku portu anod wyœwietlaczy

//piny tranzysotrów pod³aczonych do wyœwietlacza
#define 	CA4 	(1<<PC0)
#define 	CA3 	(1<<PC1)
#define 	CA2 	(1<<PC2)
#define 	CA1 	(1<<PC3)

#define MASKA_ANODY (CA1|CA2|CA3|CA4)

// definicje bitów dla poszczególnych segmentów LED
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

extern volatile uint8_t cy1;
extern volatile uint8_t cy2;
extern volatile uint8_t cy3;
extern volatile uint8_t cy4;

//funkcje biblioteczne
void d_led_init(void);
void d_led_off(void);
void d_led_on(void);

#endif	// koniec _d_led_h

