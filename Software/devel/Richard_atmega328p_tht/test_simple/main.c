//**************************<Included files>***********************************
//s. /home/roboag/roboag/Eagle/RoboSAX/licht_tht/circuit/full/circuit.sch

#include <avr/io.h>
#include <inttypes.h>

#include "gluecksrad.h"

//**************************<Macros>*******************************************
#define led_setRed(x)   ( x ? (PORTB|= _BV(1)) : (PORTB&= ~_BV(1)))
#define led_setGreen(x) ( x ? (PORTB|= _BV(2)) : (PORTB&= ~_BV(2)))
#define led_setBlue(x)  ( x ? (PORTD|= _BV(5)) : (PORTD&= ~_BV(5)))

#define button_getBumper() ((PIND & _BV(4)) == 0x00)

#define ST_CP(x)    ( x ? (PORTC|= _BV(3)) : (PORTC&= ~_BV(3)))     //Storage register clock pin;   Pin 6 (Platine)
#define SH_CP(x)    ( x ? (PORTC|= _BV(2)) : (PORTC&= ~_BV(2)))     //Shift register clock pin;     Pin 5 (Platine)
#define OE(x)       ( x ? (PORTC|= _BV(1)) : (PORTC&= ~_BV(1)))     //Output enable (active low);   Pin 4 (Platine)
#define DS(x)       ( x ? (PORTC|= _BV(0)) : (PORTC&= ~_BV(0)))     //Serial data input;            Pin 3 (Platine)


//**************************<Prototypes>***************************************
void init_hardware(void);
void start(void);
void display_state(uint8_t state);
void set_value(uint8_t value);
int main (void);

//**************************<Methods>******************************************

//**************************[init_hardware]************************************
void init_hardware(void) {

    // set leds to output
    DDRB = _BV(1) | _BV(2);
    DDRD = _BV(3) | _BV(5);
    DDRC = _BV(0) | _BV(1) | _BV(2) | _BV(3);
}

//**************************[start]********************************************
void start(void) {

    // start sequence
    delay_ms(500);

    led_setRed(1);
    delay_ms(500);

    led_setGreen(1);
    led_setRed(0);
    delay_ms(500);

    led_setBlue(1);
    led_setGreen(0);
    delay_ms(500);

    led_setBlue(0);
}

//**************************[display_state]************************************
void display_state(uint8_t state) {

    switch (state) {
        case  0: led_setRed(1); led_setGreen(0); led_setBlue(0);
                 break;
        case  1: led_setRed(0); led_setGreen(1); led_setBlue(0);
                 break;
        case  2: led_setRed(0); led_setGreen(0); led_setBlue(1);
                 break;
        case  3: led_setRed(1); led_setGreen(1); led_setBlue(0);
                 break;
        case  4: led_setRed(1); led_setGreen(0); led_setBlue(1);
                 break;
        case  5: led_setRed(0); led_setGreen(1); led_setBlue(1);
                 break;
        default: led_setRed(1); led_setGreen(1); led_setBlue(0);
                 break;
    }
}

//**************************[main]*********************************************
int main (void) {


    // initialize hardware
    init_hardware();

    // start sequence
    start();

    set_value(_BV(5) | _BV(4));
    while (1) {

        OE(0);
        delay_ms(1);
        OE(1);
        delay_ms(9);
    }
    return (0);
}
void set_value(uint8_t value) {
    //set input
    int8_t i=7;
    while (i >= 0){
        if (value & _BV(i)){
            DS(1);
        } else {
            DS(0);
        }
        delay_us(1);
        SH_CP(1);
        delay_us(1);
        SH_CP(0);

        i=i-1; // i-=1; // i--;
    }

    //enable output
    OE(0);
    //store input
    ST_CP(1);
    delay_us(10);
    ST_CP(0);


}
