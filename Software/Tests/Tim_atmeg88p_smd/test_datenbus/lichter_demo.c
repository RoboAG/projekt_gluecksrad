//**************************<Included files>***********************************
#include <avr/io.h>
#include <inttypes.h>

#include "lichter_demo.h"

//**************************<Macros>*******************************************
#define led_setRed(x)   ( x ? (PORTB|= _BV(2)) : (PORTB&= ~_BV(2)))
#define led_setGreen(x) ( x ? (PORTB|= _BV(1)) : (PORTB&= ~_BV(1)))
#define led_setBlue(x)  ( x ? (PORTD|= _BV(3)) : (PORTD&= ~_BV(3)))

#define button_getBumper() ((PIND & _BV(4)) == 0x00)

#define ST_CP(x)    ( x ? (PORTC|= _BV(1)) : (PORTC&= ~_BV(1)))
#define SH_CP(x)    ( x ? (PORTC|= _BV(2)) : (PORTC&= ~_BV(2)))
#define OE(x)       ( x ? (PORTC|= _BV(3)) : (PORTC&= ~_BV(3)))
#define DS(x)       ( x ? (PORTC|= _BV(4)) : (PORTC&= ~_BV(4)))


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
    DDRC = _BV(1) | _BV(2) | _BV(3) | _BV(4);
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


    while (1) {

        set_value(_BV(4));
        delay_ms(500);
        set_value(_BV(5));
        delay_ms(500);
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
