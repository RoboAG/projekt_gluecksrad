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



//*********************************<Macros>*************************************



//*********************************<Prototypes>*********************************
void cycle_test(void);
int main (void);



//*********************************[cycle_test]*********************************
void cycle_test(void) {

    // 16-bit is enough to ensure working properly
    static uint16_t time_last_tic  = 0;
    static uint16_t time_last_sync = 0;

    // get relative time, since last sync
    uint16_t time = systick_get() - time_last_sync;

    // repeats every 60 seconds ==> slow motion to check led change
    if (time >= 60000) {
        time_last_sync+= 60000;
        time -= 60000;
    }

    // leds can only be updated every 10ms
    time/= 10;
    if (time == time_last_tic) { return; }
    time_last_tic = time;


    // calculate new update position
    uint16_t pos_full = ((uint32_t)LEDS_COUNT * (uint32_t)LEDS_MAX
                         * (uint32_t)time) / (uint32_t)2000;
    uint8_t pos     = pos_full / LEDS_MAX;
    uint8_t pos_sub = pos_full % LEDS_MAX;

    static uint8_t pos_last = 0;

    uint8_t i;
    // first  cycle - turn on all RED   leds
    if (pos < LEDS_COUNT) {
        if (pos_last >= LEDS_COUNT) {
            leds_setAll(0, 0, LEDS_MAX);
            pos_last = 0;
        }

        for(i = pos_last; i < pos; i++) {
            leds_set(i, LEDS_MAX, 0, 0);
        }

        leds_set(pos, pos_sub, 0, LEDS_MAX - pos_sub);

    // second cycle - turn on all GREEN leds
    } else if (pos < 2*LEDS_COUNT) {
        if (pos_last <= LEDS_COUNT) {
            leds_setAll(LEDS_MAX, 0, 0 );
            pos_last = LEDS_COUNT;
        }

        for(i = pos_last; i < pos; i++) {
            leds_set(i - LEDS_COUNT, 0, LEDS_MAX, 0);
        }

        leds_set(pos - LEDS_COUNT, LEDS_MAX - pos_sub, pos_sub, 0);

    // third  cycle - turn on all BLUE  leds
    } else {
        if (pos_last <= 2*LEDS_COUNT) {
            leds_setAll(0, LEDS_MAX, 0);
            pos_last = 2*LEDS_COUNT;
        }

        for(i = pos_last; i < pos; i++) {
            leds_set(i - 2*LEDS_COUNT, 0, 0, LEDS_MAX);
        }

        leds_set(pos - 2*LEDS_COUNT, 0, LEDS_MAX - pos_sub, pos_sub);
    }

    pos_last = pos;
}

//**********************************[functions]*********************************
int main (void) {

    // initialize
    leds_init();
    robolib_init();

    systick_init();
    leds_clearAll();

    // main loop
    while(1) {

        cycle_test();
    }

    return (0);
}
