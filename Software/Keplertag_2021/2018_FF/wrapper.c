/*******************************************************************************
* wrapper.c                                                                    *
* =========                                                                    *
*                                                                              *
* Version: 11.1.0                                                              *
* Date   : 08.10.2021                                                          *
* Author : Peter Weissig, Alex Feilke                                          *
*                                                                              *
* See also:                                                                    *
*   https://github.com/RoboAG/projekt_gluecksrad                               *
*******************************************************************************/



//*********************************<Included files>*****************************
// this header is needed for price_colors
#include "leds.h"



//*********************************<Constants>**********************************
#define VERSION 11

#define PRICES_COUNT 5
#define PRICES_MAX { 300, 150, 150, 15, 5 }

const struct sLed price_colors[PRICES_COUNT] = {
    {        0,        0, LEDS_MAX }, // 1. blue
    {        0, LEDS_MAX,        0 }, // 2. green
    { LEDS_MAX,        0,        0 }, // 3. red
    { LEDS_MAX, LEDS_MAX, LEDS_MAX }, // 4. white  (silver)
    { LEDS_MAX, LEDS_MAX,        0 }  // 5. yellow (gold)
};



//*********************************<Prototypes>*********************************
uint8_t getLedPrice            (uint8_t i);



//*********************************[prices]*************************************
uint8_t getLedPrice (uint8_t i)
{
    if (i        % 2 ==  1) return 0; // 1, 3, 5, ..., 19 (50%)
    if ((i % 10) % 6 ==  0) return 1; // 0, 6, 10, 16     (20%)
    if ((i % 10) % 4 ==  0) return 2; // 4, 8, 14, 18     (20%)
    if (i            == 12) return 3; // 12               ( 5%)
    if (i            ==  2) return 4; // 2                ( 5%)

    return 0;
}



//*********************************<Included files>*****************************
// This file is just a simplified wrapper for different settings.
// Therefore, we include the real "main.c" instead of any header.
#include "../common/main.c"
