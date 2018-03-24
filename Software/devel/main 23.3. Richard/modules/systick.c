/*******************************************************************************
* systick.c                                                                    *
* =========                                                                    *
*                                                                              *
* Version: 1.0.0                                                               *
* Date   : 21.03.18                                                            *
* Author : Peter Weissig                                                       *
*                                                                              *
* See also:                                                                    *
*   https://github.com/RoboAG/projekt_gluecksrad                               *
*******************************************************************************/

//*********************************<Included files>*****************************
#include "gluecksrad.h"
#include "systick.h"
#include "leds.h"



//*********************************<Types>**************************************



//*********************************<Constants>**********************************



//*********************************<Variables>**********************************
volatile uint32_t systick_count;



//*********************************<Macros>*************************************



//*********************************<Prototypes>*********************************
ISR(TIMER0_COMPA_vect);



//*********************************[systick_init]*******************************
void systick_init(void) {

    #if   F_CPU / 1000 / 1024 > 256
        #error "can't setup systick with TIMER0 - prescaler maximum is 1024"

    #elif F_CPU / 1000 /  256 > 256
        #define TICK_CS  0b101
        #define TICK_MAX ((uint8_t) (F_CPU / 1000 / 1024))

    #elif F_CPU / 1000 /   32 > 256
        #define TICK_CS  0b100
        #define TICK_MAX ((uint8_t) (F_CPU / 1000 /  256))

    #elif F_CPU / 1000 /    8 > 256
        #define TICK_CS  0b011
        #define TICK_MAX ((uint8_t) (F_CPU / 1000 /   64))

    #elif F_CPU / 1000 /    1 > 256
        #define TICK_CS  0b010
        #define TICK_MAX ((uint8_t) (F_CPU / 1000 /    8))

    #else
        #define TICK_CS  0b001
        #define TICK_MAX ((uint8_t) (F_CPU / 1000 /    1))
    #endif

    // 8-bit Timer
    // Mode 2 (CTC until OCRA)
    TCCR0A =  _BV(WGM01);
        // Bit 6-7 (COM0Ax) =   00 output mode (none)
        // Bit 4-5 (COM0Bx) =   00 output mode (none)
        // Bit 2-3 (  -   ) =      reserved
        // Bit 0-1 (WGM0x ) =   10 select timer mode [WGM02 in TCCR0B]

    TCCR0B = (TICK_CS & 0x07);
        // Bit 6-7 (FOC0n)  =    0 force output compare (none)
        // Bit 4-5 (  -   ) =      reserved
        // Bit 3   (WGM02 ) =    0 select timer mode [WGM0x in TCCR0A]
        // Bit 0-2 (CS0x  ) =  ??? [calculated]

    TCNT0 = 0;
        // Timer/Counter Register - current value of timer

    OCR0A  = TICK_MAX;
        // Output Compare Register - top for timer

    TIMSK0 = _BV(OCIE0A);
        // Bit 3-7 (  -   ) =      reserved
        // Bit 2   (OCIE0B) =    0 interrupt for compare match B
        // Bit 1   (OCIE0A) =    1 interrupt for compare match A (systick)
        // Bit 0   (TOIE0 ) =    0 interrupt for overflow

    TIFR0 = _BV(OCF0A);
        // Bit 3-7 (  -   ) =      reserved
        // Bit 2   (OCF0B ) =    0 interrupt for compare match B
        // Bit 1   (OCF0A ) =    1 interrupt for compare match A (systick)
        // Bit 0   (TOV0  ) =    0 interrupt for overflow
}

//*********************************[systick_reset]******************************
void systick_reset() {

    uint8_t mSREG = SREG;
    cli();
    systick_count = 0;
    SREG = mSREG;
}

//*********************************[systick_delay]******************************
void systick_delay(uint16_t mseconds) {

    uint32_t start;

    start = systick_get();

    while (systick_get() - start < (uint32_t)mseconds) {
        sei();
    }
}

//*********************************[systick_get]********************************
uint32_t systick_get() {

    uint32_t result;
    uint8_t mSREG = SREG;
    cli();
    result = systick_count;
    SREG = mSREG;

    return result;
}

//*********************************[systick_toHour]*****************************
uint8_t systick_toHour(uint32_t time) {

    return time / (uint32_t) 3600000;
}

//*********************************[systick_toMin]******************************
uint8_t systick_toMin(uint32_t time) {

    return (time / (uint32_t) 60000) % (uint32_t) 60;
}

//*********************************[systick_toSec]******************************
uint8_t systick_toSec(uint32_t time) {

    return (time / (uint32_t) 1000) % (uint32_t) 60;
}

//*********************************[systick_toMsec]*****************************
uint16_t systick_toMsec(uint32_t time) {

    return (time % (uint32_t) 1000);
}

//**************************[ISR(TIMER0_COMPA_vect)]****************************
ISR(TIMER0_COMPA_vect) {

    // count up
    systick_count++;

    // turn on interrupts
    TIMSK0&= ~ _BV(OCIE0A);
    sei();

    // update leds
    leds_update();

    // turn off interrupts
    cli();
    TIMSK0|= _BV(OCIE0A);
}
