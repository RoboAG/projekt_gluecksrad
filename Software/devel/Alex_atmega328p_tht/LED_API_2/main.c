//*******************************<Included files>*******************************
#include <avr/io.h>
#include <inttypes.h>

#include "gluecksrad.h"

//***********************************<Macros>***********************************

#define getBtnBumper() ((PINB & _BV(0)) == 0x00)
#define getBtnMode()   ((PIND & _BV(4)) == 0x00)

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

//amount of leds on every circuit board (2 * amount of boards)
#define LED_COUNT 20


//leds -> array of rgb structs
struct LEDstate {
    uint8_t r, g, b;
} leds[2 * LED_COUNT];

#define setLED(i, _r, _g, _b) leds[i] = (struct LEDstate) {.r=_r, .g=_g, .b=_b }
void setLEDs(uint8_t r, uint8_t g, uint8_t b) {
    uint8_t i = LED_COUNT;
    while(i--) setLED(i, r, g, b);
}

#define clearLED(i) setLED(i, 0, 0, 0)
void clearLEDs(void) {
    uint16_t i;
    for (i = 0; i < LED_COUNT; i++) clearLED(i);
}

//write led_states to register and show them
void updateLEDs(void) {
    uint8_t i = LED_COUNT;

    while (i--) {

        uint8_t b = 8;
        uint8_t byte = 0;
        struct LEDstate* led = leds + 2 * i;
        
        if (led->r) byte |= _BV(7); // R1
        if (led->g) byte |= _BV(6); // G1
        if (led->b) byte |= _BV(5); // B1
 
        ++led;
        if (led->r) byte |= _BV(4); // R2
        if (led->g) byte |= _BV(3); // G2
        if (led->b) byte |= _BV(2); // B2

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
