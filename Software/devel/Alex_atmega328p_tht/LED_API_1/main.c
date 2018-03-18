//*******************************<Included files>*******************************#
#include <avr/io.h>
#include <inttypes.h>

#include "gluecksrad.h"

//***********************************<Macros>***********************************
#define led_setRed(x)   ( x ? (PORTB |= _BV(1)) : (PORTB &= ~_BV(1)))
#define led_setGreen(x) ( x ? (PORTB |= _BV(2)) : (PORTB &= ~_BV(2)))
#define led_setBlue(x)  ( x ? (PORTD |= _BV(5)) : (PORTD &= ~_BV(5)))

#define button_getBumper() ((PIND & _BV(4)) == 0x00)

#define ST_CP(x)    ( x ? (PORTC |= _BV(3)) : (PORTC &= ~_BV(3)))     //Storage register clock pin;   Pin 6 (Platine)
#define SH_CP(x)    ( x ? (PORTC |= _BV(2)) : (PORTC &= ~_BV(2)))     //Shift register clock pin;     Pin 5 (Platine)
#define OE(x)       ( x ? (PORTC |= _BV(1)) : (PORTC &= ~_BV(1)))     //Output enable (active low);   Pin 4 (Platine)
#define DS(x)       ( x ? (PORTC |= _BV(0)) : (PORTC &= ~_BV(0)))     //Serial data input;            Pin 3 (Platine)


//**********************************<Prototypes>********************************
void init_hardware(void);
void display_state(uint8_t state);
int main (void);

//********************************[init_hardware]********************************
void init_hardware(void) {

    // set leds to output
    DDRB = _BV(1) | _BV(2);
    DDRD = _BV(3) | _BV(5);
    DDRC = _BV(0) | _BV(1) | _BV(2) | _BV(3);
}

//***********************************[LED_API]***********************************

#define R 2
#define G 1
#define B 0

//amount of led circuit boards containing two LEDs
#define LED_COUNT 20  

/*
bgr array: { p1.b1, p1.g1, p1.r1, p1.b2, p1.g2, p1.r2,
             p2.b1, p2.g1, p2.r1, p2.b2, p2.g2, p2.r2,
             p3...,
             ...
           }
*/

uint8_t led_states[3 * LED_COUNT];

//set rgb values for specific led
void setLED(uint8_t i, uint8_t r, uint8_t g, uint8_t b) {
    uint8_t *led = getLED(i);
    led[R] = r; led[G] = g; led[B] = b;
}

void setLEDs(uint8_t r, uint8_t g, uint8_t b) {
    uint8_t i = LED_COUNT;
    while(i--) setLED(i, r, g, b)
}

#define clearLED(i) setLED(i, 0, 0, 0)
void clearLEDs(void) {
    uint16_t i = 3 * LED_COUNT;
    while (i--) led_states[i] = 0;
}

//write led_states to register and show them
void updateLEDs(void) {
    uint8_t i = LED_COUNT / 2, b, byte, *led = led_states;

    while (i--) {
        byte = 0;

        //don't change the order! add led[n] and led += 6; before!
        if (*led++) byte |= _BV(5); // B1
        if (*led++) byte |= _BV(6); // G1
        if (*led++) byte |= _BV(7); // R1
        if (*led++) byte |= _BV(2); // B2
        if (*led++) byte |= _BV(3); // G2
        if (*led++) byte |= _BV(4); // R2

        b = 8;
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

//**********************************[functions]*********************************
int main (void) {

    // initialize hardware
    init_hardware();

    uint8_t i, n = 0;

    //test rgb for each LED
    while (1) {
        for (i = 0; i < 3; i++) {
            setLED(n, i == 0, i == 1, i == 2);
            updateLEDs();
            delay_ms(300);
        }
        clearLED(n);

        ++n;
        if (n >= LED_COUNT) n = 0;
    }

    return (0);
}
