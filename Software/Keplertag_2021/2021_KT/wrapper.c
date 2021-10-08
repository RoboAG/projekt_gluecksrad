/*******************************************************************************
* wrapper.c                                                                    *
* =========                                                                    *
*                                                                              *
* Version: 11.2.1                                                              *
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
    // white      15    16      yellow
    // blue       14    17        blue
    // yellow     13    18       white
    // white      12    19      yellow
    // red        11     0         red
    // yellow     10     1       white
    // white       9     2      yellow
    // blue        8     3        blue
    // yellow      7     4       white
    // white       6     5      yellow


    // blue  (id=0):  3,  8, 14, 17                (20%)
    // green (id=1):  -                            ( 0%)
    // red   (id=2):  0, 11                        (10%)
    // white (id=3):  6,  9,  12, 15, 18  &  1,  4 (35%)
    // yellow(id=4):  7, 10,  13, 16, 19  &  2,  5 (35%)

    if (i > 5) {
        if (i        % 3 ==  0) return 3;
        if (i        % 3 ==  1) return 4;
        if (i            == 11) return 2;
        return                         0;
    } else {
        if (i        % 3 ==  1) return 3;
        if (i        % 3 ==  2) return 4;
        if (i            ==  0) return 2;
        return                         0;
    }
}



//*********************************<Included files>*****************************
// This file is just a simplified wrapper for different settings.
// Therefore, we include the real "main.c" instead of any header.
#include "../common/main.c"
