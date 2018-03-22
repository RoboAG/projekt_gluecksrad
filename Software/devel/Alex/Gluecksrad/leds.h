/*******************************************************************************
* leds.h                                                                       *
* ======                                                                       *
*                                                                              *
* Version: 1.0.0                                                               *
* Date   : 20.03.18                                                            *
* Author : Alex Feilke, Peter Weissig                                          *
*                                                                              *
* See also:                                                                    *
*   https://github.com/RoboAG/projekt_gluecksrad                               *
*******************************************************************************/

#ifndef _leds_h_
#define _leds_h_

//*******************************<Included files>*******************************
#include <avr/io.h>
#include <inttypes.h>


//*********************************<Types>**************************************



//*********************************<Constants>**********************************
#define LED_COUNT 20



//*********************************<Variables>**********************************



//*********************************<Macros>*************************************



//**********************************<Prototypes>********************************
void leds_init(void);
void leds_set(uint8_t i, uint8_t r, uint8_t g, uint8_t b);
void leds_setAll(uint8_t r, uint8_t g, uint8_t b);
void leds_clear(uint8_t i);
void leds_clearAll(void);
void leds_update(void);

#endif // #ifndef _leds_h_
