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



//*********************************<Macros>*************************************



//*********************************<Prototypes>*********************************
void init_hardware(void);
void cycle_test(void);
int main (void);



//*********************************[cycle_test]*********************************
void cycle_test(void) {

    static uint16_t time_last_tic  = 0;
    static uint16_t time_last_sync = 0;

    // check if time changed
    uint16_t time = tick_get();
    if (time == time_last_tic) { return; }
    time_last_tic = time;
    // calculate relative time since last sync
    time-= time_last_sync;

    // repeats every 6 seconds
    if (time >= 600) {
        time_last_sync+= 600;
        time -= 600;
    }


    // calculate new update position
    static uint8_t pos_last = 0;
    uint8_t pos = (LED_COUNT * time) / 200; // always within 16-bit range

    // check if current position changed
    if (pos == pos_last) { return; }

    uint8_t i;
    // first  cycle - turn on all RED   leds
    if (pos < LED_COUNT) {
        if (pos_last >= LED_COUNT) {
            leds_setAll(0,0,1);
            pos_last = 0;
        }

        for(i = pos_last; i < pos; i++) {
            leds_set(i, 1, 0, 0);
        }

    // second cycle - turn on all GREEN leds
    } else if (pos < 2*LED_COUNT) {
        if (pos_last <= LED_COUNT) {
            leds_setAll(1,0,0);
            pos_last = LED_COUNT;
        }

        for(i = pos_last; i < pos; i++) {
            leds_set(i - LED_COUNT, 0, 1, 0);
        }

    // third  cycle - turn on all BLUE  leds
    } else {
        if (pos_last <= 2*LED_COUNT) {
            leds_setAll(0,1,0);
            pos_last = 2*LED_COUNT;
        }

        for(i = pos_last; i < pos; i++) {
            leds_set(i - 2*LED_COUNT, 0, 0, 1);
        }
    }

    pos_last = pos;
}

//**********************************[functions]*********************************
int main (void) {

    // initialize
    leds_init();
    robolib_init();
    leds_clearAll();

    // main loop
    while(1) {

        cycle_test();
    }

    return (0);
}
