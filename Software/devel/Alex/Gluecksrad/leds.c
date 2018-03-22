/*******************************************************************************
* leds.c                                                                       *
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
#include "gluecksrad.h"
#include "leds.h"



//*********************************<Types>**************************************
struct sLed {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};



//*********************************<Constants>**********************************



//*********************************<Variables>**********************************
struct sLed _leds[LED_COUNT];



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
void tick_userfunction(void);
void updateLeds(void);



//*********************************<Functions>**********************************

//*********************************[leds_init]**********************************
void leds_init(void) {

    DDRB = _BV(1) | _BV(2);
    DDRD = _BV(3) | _BV(5);
    DDRC = _BV(0) | _BV(1) | _BV(2) | _BV(3);
}

//*********************************[leds_set]***********************************
void leds_set(uint8_t i, uint8_t r, uint8_t g, uint8_t b) {

    if (i < LED_COUNT) {
        _leds[i].r = r;
        _leds[i].g = g;
        _leds[i].b = b;
    }
}

//*********************************[leds_setAll]********************************
void leds_setAll(uint8_t r, uint8_t g, uint8_t b) {

    uint8_t i = LED_COUNT;
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

    uint8_t i = LED_COUNT;
    while (i--) {
        leds_clear(i);
    }
}

//*********************************[leds_update]********************************
void leds_update(void) {

    uint8_t i = LED_COUNT/2;
    uint8_t pos = 4;

    while (i--) {
        uint8_t byte = 0;

        pos--;
        if (_leds[pos].b) byte |= _BV(2); // B2
        if (_leds[pos].g) byte |= _BV(3); // G2
        if (_leds[pos].r) byte |= _BV(4); // R2
        if (pos == 0) pos = LED_COUNT;

        pos--;
        if (_leds[pos].b) byte |= _BV(5); // B1
        if (_leds[pos].g) byte |= _BV(6); // G1
        if (_leds[pos].r) byte |= _BV(7); // R1
        if (pos == 0) pos = LED_COUNT;

        uint8_t b = 8;
        while (b--) {
            DS(byte & _BV(7));
            byte <<= 1;

            delay_us(1);
            SH_CP(1);
            delay_us(1);
            SH_CP(0);
        }
    }

    //enable output
    OE(0);

    //store input
    ST_CP(1);
    delay_us(10);
    ST_CP(0);
}

//*******************************[tick_userfunction]****************************
void tick_userfunction(void) {

    leds_update();
}