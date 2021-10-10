/*******************************************************************************
* wrapper.c                                                                    *
* =========                                                                    *
*                                                                              *
* Version: 11.2.2                                                              *
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
#define PRICES_MAX { 75, 0, 36, 750, 750}

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
    // left side   #    #   right side
    // white      19     0      yellow
    // blue       18     1        blue
    // yellow     17     2      white
    // white      16     3      yellow
    // red        15     4         red
    // yellow     14     5       white
    // white      13     6      yellow
    // blue       12     7        blue
    // yellow     11     8       white
    // white      10     9      yellow


    // blue  (id=0):  1,  7, 12, 18                (20%)
    // green (id=1):  -                            ( 0%)
    // red   (id=2):  4, 15                        (10%)
    // white (id=3):  2,  5,  8  &  10, 13, 16, 19 (35%)
    // yellow(id=4):  0,  3,  6,  9  &  11, 14, 17 (35%)

    if (i < 10) {
        if (i % 3 ==  0) return 4;
        if (i % 3 ==  2) return 3;
        if (i     ==  4) return 2;
        return                  0;
    } else {
        if (i % 3 ==  1) return 3;
        if (i % 3 ==  2) return 4;
        if (i     == 15) return 2;
        return                  0;
    }
}



//*********************************<Included files>*****************************
// This file is just a simplified wrapper for different settings.
// Therefore, we include the real "main.c" instead of any header.
#include "../common/main.c"
