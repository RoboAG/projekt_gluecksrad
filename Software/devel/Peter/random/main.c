/*******************************************************************************
* main.c                                                                       *
* ======                                                                       *
*                                                                              *
* Version: 1.0.0                                                               *
* Date   : 20.03.18                                                            *
* Author : Alex Feilke, Peter Weissig                                          *
*                                                                              *
* See also:                                                                    *
*   https://github.com/RoboAG/projekt_gluecksrad                               *
*******************************************************************************/

//*********************************<Included files>*****************************
#include <avr/io.h>
#include <inttypes.h>

#include "gluecksrad.h"
#include "leds.h"
#include "buttons.h"
#include "systick.h"
#include "random.h"



//*********************************<Types>**************************************



//*********************************<Constants>**********************************



//*********************************<Variables>**********************************



//*********************************<Macros>*************************************



//*********************************<Prototypes>*********************************
void random_test(void);
int main (void);



//*********************************[random_test]********************************
void random_test(void) {

    uint8_t nr;
    nr = random() % LEDS_COUNT;

    leds_clearAll();

    uint8_t value = 0;
    while (!buttons_getBumper()) {

        value++;
        if (value >= 4*LEDS_MAX) {
            value-= 3*LEDS_MAX;
        }

        if (value < LEDS_MAX) {
            leds_set(nr, value, 0 , 0);

        } else if (value < 2*LEDS_MAX) {
            leds_set(nr, 2*LEDS_MAX - value, value - LEDS_MAX , 0);

        } else if (value < 3*LEDS_MAX) {
            leds_set(nr, 0, 3*LEDS_MAX - value, value - 2*LEDS_MAX);

        } else {
            leds_set(nr, value - 3*LEDS_MAX, 0, 4*LEDS_MAX - value);
        }

        systick_delay(200);
    }
}

//**********************************[functions]*********************************
int main (void) {

    // initialize
    leds_init();
    robolib_init();

    systick_init();
    random_init();
    leds_clearAll();

    // main loop
    while(1) {
        random_test();
    }

    return (0);
}
