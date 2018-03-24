/*******************************************************************************
* leds.h                                                                       *
* ======                                                                       *
*                                                                              *
* Version: 1.1.1                                                               *
* Date   : 22.03.18                                                            *
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
struct sLed {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};



//*********************************<Constants>**********************************
#define LEDS_COUNT 20
#define LEDS_MAX 10
#define LEDS_MIN 0



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
