/*
 * makra.h
 *
 *  Created on: 22 maj 2017
 *      Author: Maksymilian Lach
 */

#ifndef MAKRA_H_
#define MAKRA_H_

#define BUZZER_PIN (1<<PB3)
#define BEEP PORTB |= (1<<PB3)
#define NOBEEP PORTB &= ~(1<<PB3)

#define SWITCH_PIN (1<<PB2)

#endif /* MAKRA_H_ */
