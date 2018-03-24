/*******************************************************************************
* systick.h                                                                    *
* =========                                                                    *
*                                                                              *
* Version: 1.0.0                                                               *
* Date   : 21.03.18                                                            *
* Author : Peter Weissig                                                       *
*                                                                              *
* See also:                                                                    *
*   https://github.com/RoboAG/projekt_gluecksrad                               *
*******************************************************************************/

#ifndef _systick_h_
#define _systick_h_

//*******************************<Included files>*******************************
#include <avr/io.h>
#include <inttypes.h>



//*********************************<Types>**************************************



//*********************************<Constants>**********************************



//*********************************<Variables>**********************************



//*********************************<Macros>*************************************



//**********************************<Prototypes>********************************
void systick_init(void);
void systick_reset(void);

void systick_delay(uint16_t mseconds);

uint32_t systick_get(void);
uint8_t systick_toHour (uint32_t time);
uint8_t systick_toMin  (uint32_t time);
uint8_t systick_toSec  (uint32_t time);
uint16_t systick_toMsec(uint32_t time);



#endif // #ifndef _systick_h_
