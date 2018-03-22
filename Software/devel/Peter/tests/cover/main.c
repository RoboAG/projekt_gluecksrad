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



//*********************************<Types>**************************************
enum eLedColor {
    lcBlack  ,
    lcRed    ,
    lcGreen  ,
    lcBlue   ,
    lcCyan   ,
    lcMagenta,
    lcYellow ,
    lcWhite
};

struct sLedAbstract {

    enum eLedColor color;
    uint8_t   brightness;
};



//*********************************<Constants>**********************************



//*********************************<Variables>**********************************
struct sLedAbstract leds_abstract[LEDS_COUNT];


//*********************************<Macros>*************************************



//*********************************<Prototypes>*********************************
void _leds_updateAbstract(uint8_t nr);
void leds_setColor       (uint8_t nr, enum eLedColor color);
void leds_setBrightness  (uint8_t nr, uint8_t bright);

void cover1_test(void);
void cover2_test(void);
int main (void);



//*********************************[_leds_updateAbstract]***********************
void _leds_updateAbstract(uint8_t nr) {

    if (nr >= LEDS_COUNT) { return;}

    uint8_t r,g,b;

    switch (leds_abstract[nr].color) {
        case lcBlack  : r =  0; g =  0; b =  0; break;
        case lcRed    : r = 10; g =  0; b =  0; break;
        case lcGreen  : r =  0; g = 10; b =  0; break;
        case lcBlue   : r =  0; g =  0; b = 10; break;
        case lcCyan   : r =  0; g = 10; b = 10; break;
        case lcMagenta: r = 10; g =  0; b = 10; break;
        case lcYellow : r = 10; g =  8; b =  0; break;
        case lcWhite  : r = 10; g = 10; b =  8; break;
        default       : r =  0; g =  0; b =  0; break;
    }

    uint8_t bright = leds_abstract[nr].brightness;
    r*= bright; r/= 10;
    g*= bright; g/= 10;
    b*= bright; b/= 10;
    leds_set(nr, r, g, b);
}

//*********************************[leds_setColor]******************************
void leds_setColor(uint8_t nr, enum eLedColor color) {

    nr = nr % LEDS_COUNT;
    leds_abstract[nr].color = color;
    _leds_updateAbstract(nr);
}

//*********************************[leds_setBrightness]*************************
void leds_setBrightness(uint8_t nr, uint8_t bright) {

    nr = nr % LEDS_COUNT;
    leds_abstract[nr].brightness = bright;
    _leds_updateAbstract(nr);
}

//*********************************[cover1_test]********************************
void cover1_test(void) {

    // init Version Alex
    leds_setColor( 0,        lcGreen         );
    leds_setColor( 1, lcBlue                 );
    leds_setColor( 2,                lcWhite );
    leds_setColor( 3, lcBlue                 );
    leds_setColor( 4,        lcRed           );
    leds_setColor( 5, lcBlue                 );
    leds_setColor( 6,        lcGreen         );
    leds_setColor( 7, lcBlue                 );
    leds_setColor( 8,        lcRed           );
    leds_setColor( 9, lcBlue                 );
    leds_setColor(10,        lcGreen         );
    leds_setColor(11, lcBlue                 );
    leds_setColor(12,                lcYellow);
    leds_setColor(13, lcBlue                 );
    leds_setColor(14,        lcRed           );
    leds_setColor(15, lcBlue                 );
    leds_setColor(16,        lcGreen         );
    leds_setColor(17, lcBlue                 );
    leds_setColor(18,        lcRed           );
    leds_setColor(19, lcBlue                 );

    uint8_t i;
    for (i = 0; i < LEDS_COUNT; i++) {
        leds_setBrightness(i, 2);
    }

    uint8_t pos = 0;
    while (!buttons_getBumper()) {
        leds_setBrightness(pos,  2);
        pos++;
        if (pos >= LEDS_COUNT) { pos -= LEDS_COUNT;}
        leds_setBrightness(pos, 10);

        systick_delay(200);
    }
}

//*********************************[cover2_test]********************************
void cover2_test(void) {

    // init Version Alex
    leds_setColor( 0,        lcGreen         );
    leds_setColor( 1, lcBlue                 );
    leds_setColor( 2,                lcWhite );
    leds_setColor( 3, lcBlue                 );
    leds_setColor( 4,        lcYellow        );
    leds_setColor( 5, lcBlue                 );
    leds_setColor( 6,        lcGreen         );
    leds_setColor( 7, lcBlue                 );
    leds_setColor( 8,        lcYellow        );
    leds_setColor( 9, lcBlue                 );
    leds_setColor(10,        lcGreen         );
    leds_setColor(11, lcBlue                 );
    leds_setColor(12,                lcRed);
    leds_setColor(13, lcBlue                 );
    leds_setColor(14,        lcYellow        );
    leds_setColor(15, lcBlue                 );
    leds_setColor(16,        lcGreen         );
    leds_setColor(17, lcBlue                 );
    leds_setColor(18,        lcYellow        );
    leds_setColor(19, lcBlue                 );

    uint8_t i;
    for (i = 0; i < LEDS_COUNT; i++) {
        leds_setBrightness(i, 2);
    }

    uint8_t pos = 0;
    while (!buttons_getBumper()) {
        leds_setBrightness(pos,  2);
        pos++;
        if (pos >= LEDS_COUNT) { pos -= LEDS_COUNT;}
        leds_setBrightness(pos, 10);

        systick_delay(200);
    }
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

        leds_setAll(LEDS_MAX, LEDS_MAX, LEDS_MAX);
        systick_delay(100);

        cover1_test();

        leds_setAll(LEDS_MAX, LEDS_MAX, LEDS_MAX);
        systick_delay(100);

        cover2_test();
    }

    return (0);
}
