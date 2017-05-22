#ifndef ENCODER_H_
#define ENCODER_H_

#define HALF_STEP 	0 				// 0 - full step
									// 1 - Half step

#define USE_ENC_SWITCH	0			// 0 - nope
									// 1 - yes

#define USE_INT_IRQ 0				// 0 - pooling
									// 1 - przerwania

#if USE_INT_IRQ == 1

#define ENC_INT -2 		//(1 - INT0&INT1, -2 - PCINT)

#endif

#if ENC_INT == -2

#define PCINT_IRQ_VECT		PCINT2_vect
#define PCMSK_REG			PCMSK2
#define PCINT_A 			PCINT18
#define PCINT_B				PCINT19

#endif

#define ENC_B		(1<<PB4)
#define ENC_A		(1<<PB1)
#define ENC_AB_PIN	PINB
#define ENC_AB_PORT	PORTB

#define ENC_SW			(1<<PB2)
#define ENC_SW_PORT		PORTB
#define ENC_SW_PIN		PINB
#define ENC_SW_DIR		DDRB

//**********************************

#ifdef GIMSK
#define GICR	GIMSK
#endif

#define ENC_SW_ON		(ENC_SW_PIN & ENC_SW)

#define enc_A_HI		(ENC_A_PIN & ENC_A)
#define enc_B_HI		(ENC_B_PIN & ENC_B)

#define ENC_LEFT 	0x10
#define ENC_RIGHT 	0x20

extern volatile int enco_cnt;
extern volatile uint8_t enco_dir;

//********************************************************
//funkcje biblioteczne
void mk_encoder_init ( void );
void ENCODER_EVENT( void );

void register_enc_event_sw_callback (void (*callback)(void));
void register_enc_event_callback (void (*callback)(void));

void encoder_proc( void );
int get_encoder( void );
void set_encoder(int val);

#endif /* ENCODER_H_ */
