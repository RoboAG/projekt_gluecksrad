/*******************************************************************************
* leds.c                                                                       *
* ======                                                                       *
*                                                                              *
* Version: 1.1.0                                                               *
* Date   : 21.03.18                                                            *
* Author : Alex Feilke, Peter Weissig                                          *
*                                                                              *
* See also:                                                                    *
*   https://github.com/RoboAG/projekt_gluecksrad                               *
*******************************************************************************/

//*********************************<Included files>*****************************
#include "gluecksrad.h"
#include "leds.h"


//*********************************<Types>**************************************



//*********************************<Constants>**********************************



//*********************************<Variables>**********************************
volatile struct sLed leds[LEDS_COUNT];
volatile struct sLed _leds_update[LEDS_COUNT];



//*********************************<Macros>*************************************
// Storage register clock pin;   Pin 6 (Platine)
#define ST_CP(x)    ( x ? (PORTC |= _BV(3)) : (PORTC &= ~_BV(3)))

// Shift register clock pin;     Pin 5 (Platine)
#define SH_CP(x)    ( x ? (PORTC |= _BV(2)) : (PORTC &= ~_BV(2)))

// Output enable (active low);   Pin 4 (Platine)
#define OE(x)       ( x ? (PORTC |= _BV(1)) : (PORTC &= ~_BV(1)))

// Serial data input;            Pin 3 (Platine)
#define DS(x)       ( x ? (PORTC |= _BV(0)) : (PORTC &= ~_BV(0)))



//*********************************<Prototypes>*********************************
void leds_delayUs(void);



//*********************************[leds_init]**********************************
void leds_init(void) {

    DDRB = _BV(1) | _BV(2);
    DDRD = _BV(3) | _BV(5);
    DDRC = _BV(0) | _BV(1) | _BV(2) | _BV(3);
}



//*********************************[leds_set]***********************************
void leds_set(uint8_t i, uint8_t r, uint8_t g, uint8_t b) {

    if (i < LEDS_COUNT) {
        leds[i].r = r;
        leds[i].g = g;
        leds[i].b = b;
    }
}

//*********************************[leds_setAll]********************************
void leds_setAll(uint8_t r, uint8_t g, uint8_t b) {

    uint8_t i = LEDS_COUNT;
    while(i--) {
        leds_set(i, r, g, b);
    }
}

//*********************************[leds_clear]*********************************
void leds_clear(uint8_t i) {

    leds_set(i, 0, 0, 0);
}

//*********************************[leds_clearAll]******************************
void leds_clearAll(void) {

    uint8_t i = LEDS_COUNT;
    while (i--) {
        leds_clear(i);
    }
}

//*********************************[leds_update]********************************
void leds_update(void) {

    static uint8_t current_iteration = 0;
    uint8_t i;
    if (current_iteration == 0) {
        // new update cycle start ==> copy all values
        for (i = 0; i < LEDS_COUNT; i++) {
            _leds_update[i].r = leds[i].r;
            _leds_update[i].g = leds[i].g;
            _leds_update[i].b = leds[i].b;
        }
    }

    uint8_t pos = 4;
    i = LEDS_COUNT/2;

    while (i--) {
        uint8_t byte = 0;

        pos--; if (pos > LEDS_COUNT) { pos+= LEDS_COUNT;}
        if (_leds_update[pos].b > current_iteration) byte |= _BV(2); // B2
        if (_leds_update[pos].g > current_iteration) byte |= _BV(3); // G2
        if (_leds_update[pos].r > current_iteration) byte |= _BV(4); // R2

        pos--; if (pos > LEDS_COUNT) { pos+= LEDS_COUNT;}
        if (_leds_update[pos].b > current_iteration) byte |= _BV(5); // B1
        if (_leds_update[pos].g > current_iteration) byte |= _BV(6); // G1
        if (_leds_update[pos].r > current_iteration) byte |= _BV(7); // R1

        uint8_t b = 8;
        while (b--) {
            DS(byte & _BV(7));
            byte <<= 1;

            leds_delayUs();
            SH_CP(1);
            leds_delayUs();
            SH_CP(0);
        }
    }

    //enable output
    OE(0);

    //store input
    ST_CP(1);
    delay_us(1); // 10us
    ST_CP(0);

    current_iteration++;
    if (current_iteration >= LEDS_MAX) {
        current_iteration = 0;
    }
}

//*********************************[leds_delayUs]*******************************
void leds_delayUs(void) {

            // 4 cycles for call
    nop();  // 1 cycle
    nop();  // 1 cycle
    nop();  // 1 cycle
    nop();  // 1 cycle
    nop();  // 1 cycle
    nop();  // 1 cycle
    nop();  // 1 cycle
    nop();  // 1 cycle
    return; // 4 cycles for return

    // ==> 16 cycles (1µs)
}
