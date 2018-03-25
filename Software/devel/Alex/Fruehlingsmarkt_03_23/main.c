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

//TODO: aniomation ROTATION_FINISHED

//*********************************<Included files>*****************************
#include <math.h>
#include <avr/io.h>
#include <inttypes.h>

#include "gluecksrad.h"
#include "leds.h"
#include "buttons.h"
#include "systick.h"
#include "random.h"



//*********************************<Types>**************************************



//*********************************<Constants>**********************************
#define VERSION 006
#define EEPROM_KEY (0b1010011101100000 + VERSION)
#define EEPROM_RESET_DELAY 5000
#define PRICES_MAX 5
//#define PRICES_COUNT {300, 150, 150, 15, 5}
//#define PRICES_COUNT {5, 4, 3, 2, 1}
#define PRICES_COUNT {50,50,50,50,50}

#define ROT_VEL 80

const struct sLed price_colors[PRICES_MAX + 1] = {
    { .r = LEDS_MIN, .g = LEDS_MIN, .b = LEDS_MAX },
    { .r = LEDS_MIN, .g = LEDS_MAX, .b = LEDS_MIN },
    { .r = LEDS_MAX, .g = LEDS_MIN, .b = LEDS_MIN },
    { .r = LEDS_MAX, .g = LEDS_MAX, .b = LEDS_MIN },
    { .r = LEDS_MAX, .g = LEDS_MAX, .b = LEDS_MAX },
    { .r = LEDS_MIN, .g = LEDS_MIN, .b = LEDS_MIN }
};



//*********************************<Macros>*************************************
#define ONCE(code) do { code } while (0)
#define mod_float(v,m) (((v) - (m)*(uint32_t)((v)/(m))))
#define angle_rad(v) ((v) * 3.1415 / 180.0)

#define getPriceColor(i) price_colors[i - 1]
#define getLedColor(i) price_colors[getLedPrice(i)]


//*********************************<Prototypes>*********************************
void test(uint8_t i);

uint8_t getLedPrice(uint8_t i);

void updateTime(void);

void eeprom_save_key(void);
int  eeprom_validate (void);
void eeprom_getPrices (void);
void eeprom_setPrices (void);

uint8_t getRotationTarget(void);

void gluecksrad_init (void);
void animate (void);
int  main (void);



float abs_float(float v)
{
    if(v > 0) return v;
    else return -v;
}



//*********************************[getLedPrice]********************************
uint8_t getLedPrice(uint8_t i)
{
    if (i        %       2) return 0; // 1, 3, 5, ..., 19 (50%)
    if ((i % 10) % 6 ==  0) return 1; // 0, 6, 10, 16     (20%)
    if ((i % 10) % 4 ==  0) return 2; // 4, 8, 14, 18     (20%)
    if (i            == 12) return 3; // 12               ( 5%)
    if (i            ==  2) return 4; // 2                ( 5%)

    return 5;
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
    return
        eeprom_read_uint16() == EEPROM_KEY && // 2
        eeprom_read_uint16() == EEPROM_KEY;   // 4
}

void eeprom_save_key(void)
{
    eeprom_adress_set(0);            // 0
    eeprom_write_uint16(EEPROM_KEY); // 2
    eeprom_write_uint16(EEPROM_KEY); // 4
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
    //return random() % 20;
    
    if (!price_sum) return 0;

    // choose random price
    uint16_t rand = random();
    uint16_t ran = rand % price_sum;
    rand /= price_sum;

    // get category of random price
    uint8_t cat;
    for (cat = 0; ran > prices[cat] && cat < 5; cat++)
        ran -= prices[cat];


    // select field of category
    uint8_t i;
    uint8_t count = 0;
    for (i = 0; i < LEDS_COUNT; i++)
        if (getLedPrice(i) == cat)
            count++;

    count = rand % count;
    for (i = 0; i < LEDS_COUNT; i++)
    {
        if (getLedPrice(i) == cat)
        {
            if (count == 0)
                return i;
            count--;
        }
    }

    return 2;
}



//*********************************[variables]**********************************

#define STATE_STARTING        0
#define STATE_DEMO            1
#define STATE_ROTATING        2
#define STATE_ROTATE_FINISH   3
#define STATE_ROTATE_FINISHED 4
#define STATE_RESET_PRICES    5
#define STATE_PRICES_RESETTED 6
#define STATE_PRICES_EMPTY    7
#define STATE_EEPROM_INVALID  8

// current program state
uint8_t state = STATE_STARTING;

// time when btnMode press started
uint32_t time_btnMode_start = 0;

// time when animation started
uint32_t anim_start;

// rotation target
uint16_t rot_target_abs;
uint8_t  rot_target;

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
            uint8_t i = LEDS_COUNT;
            uint16_t sec = diff / 1000;

            while(i--)
            {
                struct sLed color = getLedColor(i);
                if ((sec - i + 30) % 4)
                {
                    leds_set(i, color.r / 5, color.g / 5, color.b / 5);
                }
                else
                {
                    float f = 1 + 0.3 * (diff % 2000 < 1000) - abs_float((diff % 1000) / 500.0 - 1);
                    leds_set(i, (uint8_t)(color.r * f), (uint8_t)(color.g * f), (uint8_t)(color.b * f));
                }
            }
        }
        break;

        case STATE_ROTATING:
        {
            
            
            uint16_t led = ((float)(diff * (rot_acc * diff / 2000. + ROT_VEL) / 1000.));

            uint8_t i = LEDS_COUNT;

            while (i--)
            {
                struct sLed color = getLedColor(i);
                uint8_t d = 5 * (i != rot_target % 20) + 1; //5 * (i != led % 20) + 1;
                leds_set(i, color.r / d, color.g / d, color.b / d);
            }
/*
            if (diff >= rot_time || led >= rot_target_abs)
            {
                prices[getLedPrice(rot_target) - 1]--;
                eeprom_setPrices();

                systick_delay(500);
                updateTime();
                setState(STATE_ROTATE_FINISH);
            }*/
                delay_ms(200);
                updateTime();
                setState(STATE_DEMO);
        }
        break;

        case STATE_ROTATE_FINISH:
        {
            uint8_t price = getLedPrice(rot_target);
            struct sLed color = price_colors[price];

            if (diff <= 1000) {
                float f = diff / 1000.0;
                leds_setAll((uint8_t)(color.r * f), (uint8_t)(color.g * f), (uint8_t)(color.b * f));

                /*
                switch (price)
                {
                    case 1:
                    {
                        float f = diff / 1000.0;
                        leds_setAll(color.r * f, color.g * f, color.b * f);
                    }
                    break;

                    case 2:
                    case 3:
                    {
                        int16_t d = diff % 1001 / 100.0, i = LEDS_COUNT;

                        while (i--)
                        {
                            if (i >= rot_target - d && i <= rot_target + d)
                            {
                                leds_set(i, color.r, color.g, color.b);
                            }
                            else
                            {
                                struct sLed col = getLedColor(i);
                                leds_set(i, col.r / 5, col.g / 5, col.b / 5);
                            }
                        }
                    }
                    break;

                    case 4:
                    case 5:
                    {

                    }
                    break;
                }
                */
            }
        }
        break;

        case STATE_RESET_PRICES:
        {
            static const uint16_t half = EEPROM_RESET_DELAY / 2;

            if (diff < half)  // green -> yellow
                leds_setAll(LEDS_MAX * diff / half, LEDS_MAX, 0);
            else  // yellow -> red
                leds_setAll(LEDS_MAX, LEDS_MAX * (EEPROM_RESET_DELAY - diff) / half, 0);

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

        case STATE_PRICES_RESETTED:
        {
            leds_setAll(LEDS_MAX * (diff % 1000 < 500), 0, 0);  // blink red

            //break up after 3 seconds
            if (diff >= 3000)
            {
                if (price_sum) { setState(STATE_DEMO); }
                else { setState(STATE_PRICES_EMPTY); }
            }
        }
        break;

        case STATE_PRICES_EMPTY:
        {
            uint8_t i = LEDS_COUNT;

            while (i--)
            {
                struct sLed color = getLedColor(i);
                float f = 0.45 * sin(6.283 * diff / 4000.0) + 0.55;
                leds_set(i, color.r * f, color.g * f, color.b * f);
            }
        }
        break;

        case STATE_EEPROM_INVALID:
        {
            uint8_t i = LEDS_COUNT;

            while (i--)
            {
                uint8_t d = (i + (diff % 1000 < 500)) % 2;
                leds_set(i, d, 0, 0);
            }
        }
        break;
    }
}



//*********************************[init]***************************************
// initialize program
void gluecksrad_init (void)
{
    leds_init();
    robolib_init();
    leds_clearAll();
    systick_init();
    random_init();

    if (eeprom_validate())
    {
        eeprom_getPrices();
        if (price_sum == 0) { setState(STATE_PRICES_EMPTY);}
        else { setState(STATE_DEMO); }
    }
    else { setState(STATE_EEPROM_INVALID); }

    updateTime();
}



//************************************[test]************************************
void test(uint8_t i)
{
    leds_set(i,10,10,10);
    systick_delay(500);
}



//************************************[main]************************************
int main (void)
{
    // initialize
    gluecksrad_init();

    // main loop
    while (1)
    {
        updateTime();

        if (buttons_getBumper())
        {
            if (state == STATE_DEMO)
            {
                setState(STATE_ROTATING);
                rot_target = getRotationTarget();

                uint16_t rounds = 2 * (13 + getLedPrice(rot_target)) * (15 + (time_cur % 5)) / 39;
                rot_target_abs = rounds * 20 + rot_target;
                rot_acc = - ROT_VEL * ROT_VEL / (float)(2 * (rot_target_abs) + 1);
                rot_time = 1000.0 * abs_float(ROT_VEL / rot_acc);
            }
        }

        if (buttons_getMode()) // btnMode pressed
        {
            switch (state)
            {
                case STATE_ROTATE_FINISH:
                {
                    setState(STATE_ROTATE_FINISHED);
                }
                break;

                case STATE_DEMO:
                case STATE_PRICES_EMPTY:
                {
                    setState(STATE_RESET_PRICES);
                    time_btnMode_start = time_cur;
                }
                break;

                case STATE_EEPROM_INVALID:
                {
                    eeprom_save_key();
                    eeprom_setPrices();
                    setState(STATE_DEMO);
                }
            }
        }
        else  // btnMode not pressed
        {
            switch (state)
            {
                case STATE_RESET_PRICES:
                {
                    if (price_sum) { setState(STATE_DEMO); }
                    else { setState(STATE_PRICES_EMPTY); }
                    time_btnMode_start = 0;
                }
                break;

                case STATE_ROTATE_FINISHED:
                {
                    if (price_sum) { setState(STATE_DEMO); }
                    else { setState(STATE_PRICES_EMPTY); }
                }
                break;
            }
        }

        animate();
    }

    return 0;
}
