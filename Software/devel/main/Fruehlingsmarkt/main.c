/*******************************************************************************
* main.c                                                                       *
* ======                                                                       *
*                                                                              *
* Version: 1.0.0                                                               *
* Date   : 21.03.18                                                            *
* Author : Alexander Feilke                                                    *
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
#include "random.h"



//*********************************<Types>**************************************



//*********************************<Constants>**********************************
#define VERSION 001
#define EEPROM_KEY (0b1010011101100000 + VERSION)
#define EEPROM_RESET_DELAY 5000
#define PRICES_MAX 5
#define PRICES_COUNT {300, 150, 150, 15, 5}

#define ROT_VEL 200

const struct sLed price_colors[PRICES_MAX + 1] = {
    { 0,  0,  0},
    { 0,  0, 10},
    { 0, 10,  0},
    {10,  0,  0},
    {10, 10,  0},
    {10, 10, 10}
};



//*********************************<Macros>*************************************
#define ONCE(code) do { code } while (0)
#define mod_float(v,m) ((v - m*(uint32_t)(v/m)))

#define getPriceColor(i) price_colors[i]
#define getLedColor(i) price_colors[getLedPrice(i)]



//*********************************<Prototypes>*********************************
uint8_t getLedPrice(uint8_t i);

void updateTime(void);

int  eeprom_validate (void);
void eeprom_getPrices (void);
void eeprom_setPrices (void);

uint8_t getRotationTarget(void);

void gluecksrad_init (void);
void animate (void);
int  main (void);



//*********************************[getLedPrice]********************************
uint8_t getLedPrice(uint8_t i)
{
    if (i            >= 20) return 0; // impossible value ( 0%)
    if (i        %       2) return 1; // 1, 3, 5, ..., 19 (50%)
    if ((i % 10) % 6 ==  0) return 2; // 0, 6, 10, 16     (20%)
    if ((i % 10) % 4 ==  0) return 3; // 4, 8, 14, 18     (20%)
    if (i            == 12) return 4; // 12               ( 5%)
    if (i            ==  2) return 5; // 2                ( 5%)

    return 0;
}



//*********************************[timer]**************************************
// 32-bit timer in milliseconds -> overflow after: 49d 17h 2m 47s 296ms
uint32_t  time_cur = 0,  time_last = 0;

void updateTime (void)
{
    time_last = time_cur;
    time_cur = systick_get();
}



//*********************************[eeprom]*************************************
//array of maximum amounts of prices
uint16_t prices[PRICES_MAX] = PRICES_COUNT;

//sum of available prices
uint16_t price_sum = 620;


//use a specific key to check whether the eeprom
//is on the actual gluecksrad program version
int eeprom_validate (void)
{
    eeprom_adress_set(0);                     // 0
    if (eeprom_read_uint16() == EEPROM_KEY && // 2
        eeprom_read_uint16() == EEPROM_KEY    // 4
    ) {        
        return 1;
    }
    else
    {
        eeprom_adress_set(0);            // 0
        eeprom_write_uint16(EEPROM_KEY); // 2
        eeprom_write_uint16(EEPROM_KEY); // 4
    }
    return 0;
}

//read and write prices
void eeprom_getPrices (void)
{
    eeprom_adress_set(4);             // 4
    prices[0] = eeprom_read_uint16(); // 6
    prices[1] = eeprom_read_uint16(); // 8
    prices[2] = eeprom_read_uint16(); //10
    prices[3] = eeprom_read_uint16(); //12
    prices[4] = eeprom_read_uint16(); //14

    price_sum = prices[0] + prices[1] + prices[2] + prices[3] + prices[4];
}

void eeprom_setPrices (void)
{
    eeprom_adress_set(4);           // 4
    eeprom_write_uint16(prices[0]); // 6
    eeprom_write_uint16(prices[1]); // 8
    eeprom_write_uint16(prices[2]); //10
    eeprom_write_uint16(prices[3]); //12
    eeprom_write_uint16(prices[4]); //14

    price_sum = prices[0] + prices[1] + prices[2] + prices[3] + prices[4];
}



//*********************************[random]*************************************

uint8_t getRotationTarget(void)
{
    if (!price_sum) return 0;

    // choose random price
    uint16_t rand = random();
    uint16_t ran = rand % price_sum;
    rand /= price_sum;

    // get category of random price
    uint8_t cat;
    for (cat = 0; ran > prices[cat] && cat < PRICES_MAX; cat++)
        ran -= prices[cat];


    // select field of category
    uint8_t i;
    uint8_t count = 0;
    for (i = 0; i < LEDS_MAX; i++)
        if (getLedPrice(i) == cat)
            count++;

    count = rand % count;
    for (i = 0; i < LEDS_MAX; i++)
    {
        if (getLedPrice(i) == cat)
        {
            if (count == 0)
                return i;
            count--;
        }
    }

    return 0;
}



//*********************************[init]***************************************
// initialize program
void gluecksrad_init (void)
{
    leds_init();
    robolib_init();
    leds_clearAll();

    if (eeprom_validate()) eeprom_getPrices();
    else eeprom_setPrices();

    updateTime();
}



//*********************************[variables]**********************************

#define STATE_STARTING        0
#define STATE_DEMO            1
#define STATE_ROTATING        2
#define STATE_ROTATE_FINISHED 3
#define STATE_RESET_PRICES    4
#define STATE_PRICES_RESETTED 5

// current program state
uint8_t state = STATE_STARTING;

// time when btnMode press started
uint32_t time_btnMode_start = 0;

// time when animation started
uint32_t anim_start;

// rotation target
uint8_t rot_target;

float rot_acc, rot_time, rot_led_start;



//*********************************[setState]***********************************
void setState (uint8_t st)
{
    state = st;
    anim_start = time_cur;
}



//**********************************[animate]***********************************

void animate (void)
{
    uint32_t diff = time_cur - anim_start;

    switch (state)
    {
        case STATE_DEMO:
        {
            uint8_t i = LEDS_MAX;
            while(i--)
            {
                struct sLed color = getLedColor(i);
                uint8_t d = ((diff / 1000) + i) % 3 + 1;
                leds_set(i, color.r / d, color.g / d, color.b / d);
            }
        }
        break;

        case STATE_ROTATING:
        {
            uint8_t led = ((int32_t)(diff * (rot_acc * diff / 2000.0 + ROT_VEL) / 1000.0)) % 20;
            uint8_t i = LEDS_MAX;

            while(i--)
            {
                struct sLed color = getLedColor(i);
                uint8_t d = (i == led) + 1;
                leds_set(i, color.r / d, color.g / d, color.b / d);
            }

            if (diff + 1 > rot_time && led == rot_target)
            {
                setState(STATE_ROTATE_FINISHED);
                prices[rot_target - 1]--;
                eeprom_setPrices();
            }
        }
        break;

        case STATE_RESET_PRICES:
        {
            uint16_t half = EEPROM_RESET_DELAY / 2;

            if (diff < half)  // green  -> yellow
                leds_setAll(LEDS_MAX * diff / half, 10, 0);
            else  // yellow -> red
                leds_setAll(LEDS_MAX, LEDS_MAX * (2 - diff / half), 0);
        }
        break;

        case STATE_PRICES_RESETTED:
        {
            leds_setAll(LEDS_MAX * (diff % 1000 < 500), 0, 0);  // blink red

            //break up after 3 seconds
            if (diff >= 3000)
                setState(STATE_DEMO);
        }
        break;
    }
}



//************************************[main]************************************
int main (void)
{    
    // initialize
    gluecksrad_init();

    setState(STATE_DEMO);

    // main loop
    while (1)
    {
       updateTime();

        if (state == STATE_DEMO && buttons_getBumper())
        {
            state = STATE_ROTATING;
            anim_start = time_cur;
            rot_target = getRotationTarget();
            rot_time = (1 + getLedPrice(rot_target) / 13) * (5 + (time_cur % 200) / 150.0);
            rot_acc = - ROT_VEL / rot_time;

            float led_end = mod_float(rot_time * (rot_acc * rot_time / 2.0 + ROT_VEL), 20.0);
            if (led_end < rot_target + 0.5) led_end += 20.0;
            rot_led_start = led_end - rot_target + 0.5;
        }

        if (buttons_getMode()) // btnMode pressed
        {
            switch (state)
            {
                case STATE_ROTATE_FINISHED:
                {
                    setState(STATE_DEMO);
                }
                break;

                case STATE_DEMO:
                { 
                    setState(STATE_RESET_PRICES);
                    time_btnMode_start = time_cur;
                }
                break;

                case STATE_RESET_PRICES:
                {
                    //wait EEPROM_RESET_DELAY milliseconds during btnMode pressed
                    if (time_cur - time_btnMode_start >= EEPROM_RESET_DELAY)
                    {
                        setState(STATE_PRICES_RESETTED);

                        //reset eeprom
                        uint16_t _prices[PRICES_MAX] = PRICES_COUNT;
                        prices[0] = _prices[0];
                        prices[1] = _prices[1];
                        prices[2] = _prices[2];
                        prices[3] = _prices[3];
                        prices[4] = _prices[4];
                        eeprom_setPrices();
                    }
                }
                break;
            }
        }
        else  // btnMode not pressed
        {
            switch (state)
            {
                case STATE_RESET_PRICES:
                {
                    setState(STATE_DEMO);
                    time_btnMode_start = 0;
                }
                break;
            }
        }

        animate();
    }

    return 0;
}
