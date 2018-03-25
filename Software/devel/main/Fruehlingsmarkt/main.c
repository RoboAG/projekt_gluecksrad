/*******************************************************************************
* main.c                                                                       *
* ======                                                                       *
*                                                                              *
* Version: 11.2.0                                                               *
* Date   : 25.03.18                                                            *
* Author : Alexander Feilke                                                    *
*                                                                              *
* See also:                                                                    *
*   https://github.com/RoboAG/projekt_gluecksrad                               *
*******************************************************************************/



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
#define VERSION 10
#define EEPROM_KEY (0b1010011101100000 + VERSION)
#define EEPROM_RESET_DELAY 5000
#define PRICES_COUNT 5
#define PRICES_MAX {300, 150, 150, 15, 5}

#define ROT_VEL 80

const struct sLed price_colors[PRICES_COUNT] = {
    { LEDS_MIN, LEDS_MIN, LEDS_MAX },
    { LEDS_MIN, LEDS_MAX, LEDS_MIN },
    { LEDS_MAX, LEDS_MIN, LEDS_MIN },
    { LEDS_MAX, LEDS_MAX, LEDS_MIN },
    { LEDS_MAX, LEDS_MAX, LEDS_MAX }
};



//*********************************<Macros>*************************************
#define ONCE(code) do { code } while (0)
#define angle_rad(v) ((v) * 3.1415 / 180.0)

#define getPriceColor(i) price_colors[i]
#define getLedColor(i) price_colors[getLedPrice(i)]


//*********************************<Prototypes>*********************************
void test (uint8_t i);

uint8_t getLedPrice (uint8_t i);

void updateTime (void);

void eeprom_save_key (void);
int  eeprom_validate (void);
void eeprom_getPrices (void);
void eeprom_setPrices (void);

uint8_t getRotationTarget (void);

void gluecksrad_init (void);
void animate (void);
int  main (void);



//*********************************<Flags>**************************************
#define MODE_DEFAULT          1
#define MODE_LENZ             2

#define STATE_STARTING        0
#define STATE_DEMO            1
#define STATE_ROTATING        2
#define STATE_ROTATE_FINISH   3
#define STATE_ROTATE_FINISHED 4
#define STATE_RESET_PRICES    5
#define STATE_PRICES_RESETTED 6
#define STATE_PRICES_EMPTY    7
#define STATE_EEPROM_INVALID  8

// wheel mode
uint8_t mode = MODE_DEFAULT;

// current program state
uint8_t state = STATE_STARTING;



//*********************************<Math>***************************************
float mod_float (float v, float m)
{
    return v - m * (int32_t)(v / m);
}

float abs_float (float v)
{
    if (v > 0)
        return v;
    else
       return -v;
}

float abs_int16 (int16_t v)
{
    if (v > 0)
        return v;
    else
        return -v;
}



//*********************************[getLedPrice]********************************
uint8_t getLedPrice (uint8_t i)
{
    if (i        % 2 ==  1) return 0; // 1, 3, 5, ..., 19 (50%)
    if ((i % 10) % 6 ==  0) return 1; // 0, 6, 10, 16     (20%)
    if ((i % 10) % 4 ==  0) return 2; // 4, 8, 14, 18     (20%)
    if (i            == 12) return 3; // 12               ( 5%)
    if (i            ==  2) return 4; // 2                ( 5%)

    return 0;
}



//*********************************[timer]**************************************
// 32-bit timer in milliseconds -> overflow after: 49d 17h 2m 47s 296ms
uint32_t time_cur = 0,  time_last = 0;

void updateTime (void)
{
    time_last = time_cur;
    time_cur = systick_get();
}



//*********************************[eeprom]*************************************
//array of maximum amounts of prices
int16_t prices[PRICES_COUNT] = PRICES_MAX;

//sum of available prices
int16_t price_sum = 620;


//use a specific key to check whether the eeprom
//is on the actual gluecksrad program version
int eeprom_validate (void)
{
    eeprom_adress_set(0);                     // 0
    return
        eeprom_read_uint16() == EEPROM_KEY && // 2
        eeprom_read_uint16() == EEPROM_KEY;   // 4
}

void eeprom_save_key (void)
{
    eeprom_adress_set(0);            // 0
    eeprom_write_uint16(EEPROM_KEY); // 2
    eeprom_write_uint16(EEPROM_KEY); // 4
}

//read and write prices
void eeprom_getPrices (void)
{
    if (mode == MODE_LENZ)
    {
        eeprom_adress_set(4);                      // 4
        prices[0] = (int16_t)eeprom_read_uint16(); // 6
        prices[1] = (int16_t)eeprom_read_uint16(); // 8
        prices[2] = (int16_t)eeprom_read_uint16(); //10
        prices[3] = (int16_t)eeprom_read_uint16(); //12
        prices[4] = (int16_t)eeprom_read_uint16(); //14
    }

    price_sum = prices[0] + prices[1] + prices[2] + prices[3] + prices[4];
}

void eeprom_setPrices (void)
{
    if (mode == MODE_LENZ)
    {
        eeprom_adress_set(4);                     // 4
        eeprom_write_uint16((uint16_t)prices[0]); // 6
        eeprom_write_uint16((uint16_t)prices[1]); // 8
        eeprom_write_uint16((uint16_t)prices[2]); //10
        eeprom_write_uint16((uint16_t)prices[3]); //12
        eeprom_write_uint16((uint16_t)prices[4]); //14
    }

    price_sum = prices[0] + prices[1] + prices[2] + prices[3] + prices[4];
}



//*********************************[random]*************************************

uint8_t getRotationTarget (void)
{
    if (price_sum <= 0) return 0;

    // choose random price
    uint16_t i, num = random(), ran = num % price_sum;
    uint8_t cat;

    // select category dependent on the probability
    for (cat = 0; cat < PRICES_COUNT && ran >= prices[cat]; cat++)
        ran -= prices[cat];

    #define TARGET_STEP 3
    num = LEDS_COUNT * TARGET_STEP + num % 20;

    for (i = num % 20; i < num; i += TARGET_STEP)
        if (getLedPrice(i % 20) == cat)
            return i % 20;

    //leds_setAll(1,1,0);
    //delay_ms(100);
    return 0;
}



//*********************************[variables]**********************************

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



//*********************************[setMode]***********************************
void setMode (uint8_t md)
{
    mode = md;
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

            while (i--)
            {
                struct sLed color = getLedColor(i);
                if ((sec - i + 40) % 4)
                    leds_set(i, color.r / 5, color.g / 5, color.b / 5);
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
                uint8_t d = 5 * (i != led % 20) + 1;
                leds_set(i, color.r / d, color.g / d, color.b / d);
            }

            if (diff >= rot_time || led >= rot_target_abs)
            {
                if (mode == MODE_LENZ)
                {
                    prices[getLedPrice(rot_target)]--;
                    eeprom_setPrices();
                }

                systick_delay(500);
                updateTime();
                setState(STATE_ROTATE_FINISH);
            }
        }
        break;

        case STATE_ROTATE_FINISH:
        {
            uint8_t price = getLedPrice(rot_target);
            struct sLed color = getPriceColor(price);

            switch (price)
            {
                case 0:
                {
                    float abs = abs_float((diff % 1000) / 500.0 - 1);
                    uint8_t
                        f = 10 - 10 * abs,
                        g = 7 * abs,
                        i = LEDS_COUNT;

                    while (i--)
                    {
                        struct sLed led_color = getLedColor(i);
                        leds_set(i,
                            (color.r * f     + led_color.r * g) / 20,
                            (color.g * f     + led_color.g * g) / 20,
                            (color.b * f * 2 + led_color.b * g) / 40
                        );
                    }
                }
                break;

                case 1:
                case 2:
                {
                    int32_t i, d = round((diff % 1001) / 50.0);

                    for (i = rot_target - d % 10; i <= rot_target + d % 10; i++)
                    {
                        if (d > 10) color = getLedColor((i + 800) % 20);
                        leds_set((i + 800) % 20, color.r, color.g, color.b);
                    }
                }
                break;

                case 3:
                case 4:
                {
                    uint8_t i = LEDS_COUNT;
                    uint16_t sec = diff / 50;

                    while (i--)
                    {
                        struct sLed led_color = getLedColor(i);

                        if ((sec - i + 40) % 4)
                            leds_set(i, led_color.r / 5, led_color.g / 5, led_color.b / 5);
                        else
                            leds_set(i, color.r, color.g, color.b);
                    }
                }
                break;
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
                uint16_t _prices[PRICES_COUNT] = PRICES_MAX;
                prices[0] = _prices[0];
                prices[1] = _prices[1];
                prices[2] = _prices[2];
                prices[3] = _prices[3];
                prices[4] = _prices[4];

                if (mode == MODE_LENZ)
                    eeprom_setPrices();
            }
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

        case STATE_PRICES_EMPTY:
        {
            uint8_t i = LEDS_COUNT;

            while (i--)
            {
                struct sLed color = getLedColor(i);
                float f = sin(6.283 * diff / 4000.0) / 2.0 + 0.5;
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



    // select mode
    if (buttons_getMode())
    {
        setMode(MODE_LENZ);
        if (eeprom_validate())
        {
            eeprom_getPrices();
            if (price_sum <= 0)
                setState(STATE_PRICES_EMPTY);
            else
                setState(STATE_DEMO);
        }
        else
            setState(STATE_EEPROM_INVALID);

        leds_setAll(LEDS_MAX, LEDS_MAX, LEDS_MAX);
        systick_delay(1000);
    }
    else
    {
        price_sum = prices[0] + prices[1] + prices[2] + prices[3] + prices[4];
        setState(STATE_DEMO);
        setMode(MODE_DEFAULT);
    }

    updateTime();
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
                if (price_sum > 0)
                {
                    rot_target = getRotationTarget();

                    uint16_t rounds = 2 * (14 + getLedPrice(rot_target)) * (15 + (time_cur % 5)) / 39;
                    rot_target_abs = rounds * 20 + rot_target;
                    rot_acc = - ROT_VEL * ROT_VEL / (float)(2 * (rot_target_abs) + 1);
                    rot_time = 1000.0 * abs_float(ROT_VEL / rot_acc);

                    setState(STATE_ROTATING);
                }
                else
                    setState(STATE_PRICES_EMPTY);
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
                    if (time_btnMode_start)
                    {
                        if (time_cur - time_btnMode_start > 1000)
                            setState(STATE_RESET_PRICES);
                    }
                    else
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
            if (time_btnMode_start)
                time_btnMode_start = 0;

            switch (state)
            {
                case STATE_RESET_PRICES:
                {
                    if (price_sum > 0)
                        setState(STATE_DEMO);
                    else
                        setState(STATE_PRICES_EMPTY);
                }
                break;

                case STATE_ROTATE_FINISHED:
                {
                    if (price_sum > 0)
                        setState(STATE_DEMO);
                    else
                        setState(STATE_PRICES_EMPTY);
                }
                break;
            }
        }

        animate();
    }

    return 0;
}
