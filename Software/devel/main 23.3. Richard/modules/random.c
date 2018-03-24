/*******************************************************************************
* random.c                                                                     *
* ========                                                                     *
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
#include "random.h"



//*********************************<Types>**************************************



//*********************************<Constants>**********************************



//*********************************<Variables>**********************************



//*********************************<Macros>*************************************



//*********************************<Prototypes>*********************************



//*********************************[random_init]********************************
void random_init(void) {

    // 16-bit Timer
    // Mode 0 (Normal Mode)
    TCCR1A =  0x00;
        // Bit 6-7 (COM1Ax) =   00 output mode (none)
        // Bit 4-5 (COM1Bx) =   00 output mode (none)
        // Bit 2-3 (  -   ) =      reserved
        // Bit 0-1 (WGM1x ) =   00 select timer mode [WGM1x in TCCR0B]

    TCCR1B = _BV(CS10);
        // Bit 7   (ICNC1 ) =    0 input capture noise canceler
        // Bit 6   (ICES1 ) =    0 input capture edge select
        // Bit 5   (  -   ) =      reserved
        // Bit 3-4 (WGM1x ) =   00 select timer mode [WGM1x in TCCR0A]
        // Bit 0-2 (CS1x  ) =  001 prescaler of 1 (full speed)

    TCNT1H = 0;
    TCNT1L = 0;
        // Timer/Counter Register - current value of timer

    TIMSK1 = 0x00;
        // Bit 6-7 (  -   ) =      reserved
        // Bit 5   (ICIE  ) =    0 interrupt for input capture
        // Bit 3-4 (  -   ) =      reserved
        // Bit 2   (OCIE1B) =    0 interrupt for compare match B
        // Bit 1   (OCIE1A) =    0 interrupt for compare match A
        // Bit 0   (TOIE1 ) =    0 interrupt for overflow
}

//*********************************[random]*************************************
uint16_t random() {

    union uint16 result;
    uint8_t mSREG = SREG;
    cli();
    result.l = TCNT1L;
    result.h = TCNT1H;
    SREG = mSREG;

    return (result.u << 11) - (result.u << 4) + (result.u >> 4) + 0x3f7412e;
}
