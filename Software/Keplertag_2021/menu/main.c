/*******************************************************************************
* main.c                                                                       *
* ======                                                                       *
*                                                                              *
* Version: 11.0.0                                                              *
* Date   : 25.09.21                                                            *
* Author : Alexander Feilke, Peter Weissig                                     *
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



//*********************************<Flags>**************************************
#define MODE_DEFAULT           1
#define MODE_COUNTDOWN         2

#define STATE_STARTING         0
#define STATE_DEMO             1
#define STATE_ROTATING         2
#define STATE_ROTATE_FINISH    3
#define STATE_ROTATE_FINISHED  4
#define STATE_MENU_STARTING    5
#define STATE_MENU             6
#define STATE_MENU_NEXT        7
#define STATE_MENU_SELECT      8
#define STATE_MENU_SELECTED    9
//#define STATE_SHOW_PRICES     10
#define STATE_RESET_PRICES    11
#define STATE_PRICES_EMPTY    12
#define STATE_EEPROM_INVALID  13

#define MENU_EXIT              0
#define MENU_MODE_DEFAULT      1
#define MENU_MODE_COUNTDOWN    2
//#define MENU_SHOW_PRICES       3
#define MENU_EEPROM_RESET      3
#define MENU_COUNT             4

// wheel mode
uint8_t mode = MODE_DEFAULT;

// program state
uint8_t state = STATE_STARTING;

// menu state
uint8_t menu = MENU_EEPROM_RESET;



//*********************************<Constants>**********************************
#define VERSION 11
#define EEPROM_KEY (0xA760 + VERSION)

#define PRICES_COUNT 5
#define PRICES_MAX { 5, 4, 3, 2, 1 }

#define ROT_VEL 80


#define MENU_INIT_DELAY      1000

#define MENU_START_MINDELAY  1000
#define MENU_START_MAXDELAY  3000

#define MENU_SELECT_DELAY    3000

#define ANIMATE_PRICES_EMPTY_LOOP 4000


const struct sLed menu_colors[MENU_COUNT] = {
    { LEDS_MIN, LEDS_MIN, LEDS_MAX },
    { LEDS_MIN, LEDS_MAX, LEDS_MIN },
    { LEDS_MAX, LEDS_MAX, LEDS_MIN },
    { LEDS_MAX, LEDS_MIN, LEDS_MIN }
};

const struct sLed price_colors[PRICES_COUNT] = {
    { LEDS_MIN, LEDS_MIN, LEDS_MAX },
    { LEDS_MIN, LEDS_MAX, LEDS_MIN },
    { LEDS_MAX, LEDS_MIN, LEDS_MIN },
    { LEDS_MAX, LEDS_MAX, LEDS_MAX },
    { LEDS_MAX, LEDS_MAX, LEDS_MIN }
};



//*********************************<Macros>*************************************
#define eeprom_read_uint8 eeprom_read
#define eeprom_write_uint8 eeprom_write

#define getPriceColor(i) price_colors[i]
#define getLedColor(i) price_colors[getLedPrice(i)]

#define getMenuColor(i) menu_colors[i]
#define getLedMenu(i) ((i)*MENU_COUNT/LEDS_COUNT)
#define getLedMenuColor(i) menu_colors[getLedMenu(i)]



//*********************************<Prototypes>*********************************
float   abs_float              (float v);

uint8_t getLedPrice            (uint8_t i);

void    updateTime             (void);

void    eeprom_save_key        (void);
uint8_t eeprom_validate        (void);
void    eeprom_getPrices       (void);
void    eeprom_setPrices       (void);
uint8_t eeprom_getMode         (void);
void    eeprom_setMode         (uint8_t md);

void    setMode                (uint8_t md);
void    setState               (uint8_t st);

void    gluecksrad_init        (void);
void    animate                (void);
uint8_t getRotationTarget      (void);

void    handleBumperPressed    (void);
void    handleBumperNotPressed (void);
void    handleModePressed      (void);
void    handleModeNotPressed   (void);

int     main                   (void);



//*********************************<Math>***************************************
float abs_float (float v)
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
uint32_t time_now = 0,  time_last = 0;

void updateTime (void)
{
    time_last = time_now;
    time_now = systick_get();
}



//*********************************[eeprom]*************************************
//array of maximum number of prices
int16_t prices_max[PRICES_COUNT] = PRICES_MAX;
//array of current number of prices
int16_t prices[PRICES_COUNT];

//sum of available prices
int16_t price_sum = 0;


//use a specific key to check whether the eeprom
//is on the actual gluecksrad program version
uint8_t eeprom_validate (void)
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
    if (mode == MODE_COUNTDOWN)
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
    if (mode == MODE_COUNTDOWN)
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


//read and write mode
uint8_t eeprom_getMode (void)
{
    eeprom_adress_set(14);               //14
    return (uint8_t)eeprom_read_uint8(); //15
}

void eeprom_setMode (uint8_t md)
{
    eeprom_adress_set(14);           //14
    eeprom_write_uint8((uint8_t)md); //15
}



//*********************************[variables]**********************************

// time when btnMode press started
uint32_t time_btnMode_start = 0;

// time when btnBumper press started
uint32_t time_btnBumper_start = 0;

// time when animation started
uint32_t time_state_start;

// rotation target
uint16_t rot_target_abs; // absolute target including rounds
uint8_t  rot_target;     // target led (0-20]

// rotation acceleration and time
float rot_acc, rot_time;


//*********************************[setMode]************************************
void setMode (uint8_t md)
{
    mode = md;
    eeprom_setMode(mode);

    switch (mode)
    {
        case MODE_DEFAULT:
        {
            //reset price array
            price_sum = 0;
            for (uint8_t i = 0; i < PRICES_COUNT; i++)
            {
                prices[i] = prices_max[i];
                price_sum += prices[i];
            }

            setState(STATE_DEMO);
        }
        break;

        case MODE_COUNTDOWN:
        {
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
        }
        break;
    }
}



//*********************************[setState]***********************************
void setState (uint8_t st)
{
    state = st;
    time_state_start = time_now;
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

    setMode(eeprom_getMode());

    // show current mode
    if (mode == MODE_COUNTDOWN)
        leds_setAll2(getMenuColor(MENU_MODE_COUNTDOWN));
    else // if (mode == MODE_DEFAULT)
        leds_setAll2(getMenuColor(MENU_MODE_DEFAULT));

    systick_delay(MENU_INIT_DELAY);

    updateTime();
}



//**********************************[animate]***********************************

void animate (void)
{
    uint32_t diff = time_now - time_state_start;

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
                if (mode == MODE_COUNTDOWN)
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
                        leds_set2((i + 800) % 20, color);
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
                            leds_set2(i, color);
                    }
                }
                break;
            }
        }
        break;

        case STATE_MENU_STARTING:
        {
            // fade current leds into menu leds
            // (limit max. brightness to 50%)
            struct sLed led_color, menu_color;

            uint8_t
                f = 10 * diff / MENU_START_MAXDELAY,
                g = 10 - f,
                i = LEDS_COUNT;

            while (i--)
            {
                led_color = getLedColor(i);
                menu_color = getLedMenuColor(i);

                leds_set(i,
                    (menu_color.r * f + led_color.r * g) / 20,
                    (menu_color.g * f + led_color.g * g) / 20,
                    (menu_color.b * f + led_color.b * g) / 20
                );
            }

            //wait MENU_START_MAXDELAY milliseconds during btnMode pressed
            if (time_now - time_btnMode_start >= MENU_START_MAXDELAY)
            {
                setState(STATE_MENU_NEXT);
                menu = MENU_EEPROM_RESET;
            }
        }
        break;

        case STATE_MENU:
        {
            // set current menu to 100% brightness
            // set other menus  to  10% brightness
            uint8_t i = LEDS_COUNT, led_menu, d;
            struct sLed menu_color;

            while (i--)
            {
                led_menu = getLedMenu(i);
                d = 9 * (led_menu != menu) + 1;
                menu_color = getMenuColor(led_menu);

                leds_set(i, menu_color.r / d, menu_color.g / d, menu_color.b / d);
            }
        }
        break;

        case STATE_MENU_NEXT:
        {
            // set current and next menu to 50% brightness
            // set other menus           to 10% brightness
            uint8_t i = LEDS_COUNT, led_menu, d;
            struct sLed menu_color;

            while (i--)
            {
                led_menu = getLedMenu(i);
                d = 8 * ((led_menu != menu) && (led_menu != (menu + 1) % MENU_COUNT)) + 2;
                menu_color = getMenuColor(led_menu);

                leds_set(i, menu_color.r / d, menu_color.g / d, menu_color.b / d);
            }
        }
        break;

        case STATE_MENU_SELECT:
        {
            // fade somehow all menu leds into current menu leds

            struct sLed led_color, menu_color;

            uint8_t
                f = 10 * diff / MENU_SELECT_DELAY,
                i = LEDS_COUNT;

            while (i--)
            {
                led_color = getLedMenuColor(i);
                menu_color = getMenuColor(menu);

                leds_set(i,
                    (menu_color.r * f + led_color.r * 2) / 20,
                    (menu_color.g * f + led_color.g * 2) / 20,
                    (menu_color.b * f + led_color.b * 2) / 20
                );
            }

            //wait MENU_SELECT_DELAY milliseconds during btnBumper pressed
            if (time_now - time_btnBumper_start >= MENU_SELECT_DELAY)
                setState(STATE_MENU_SELECTED);
        }
        break;

        case STATE_MENU_SELECTED:
        {
            // blink with current menu-color
            struct sLed color = getMenuColor(menu);
            if (diff % 1000 < 500)
                leds_setAll2(color);
            else
                leds_setAll(0, 0, 0);
        }
        break;
/*
        case STATE_SHOW_PRICES:
        {
            uint8_t i, show = (diff / 10000) % PRICES_COUNT;

            struct sLed color = price_colors[(show + 1) % PRICES_COUNT];

            // set different color before starting position
            for (i = 4; i < LEDS_COUNT; i += 6)
            {
                leds_set2(i    , color);
                leds_set2(i + 1, color);
            }

            color = price_colors[show];

            uint16_t
                val = prices[show];     // value
                dig = val % 10,         // digit value
                dec = 0;                // decimal pos

            leds_clearAll();

            // show value in bindec system
            while (dig)
            {
                dig = val % 10;
                val /= 10;

                for (i = 0; i < 4; i++)
                {
                    if (dig & 1 << i)
                        leds_set2(6 * dec + i, color);
                }
                dec++;
            }
        }
        break;
*/
        case STATE_PRICES_EMPTY:
        {
            // continuously fade all leds on and off within 4 seconds
            // (using sin-waves, not just linear ramps)
            uint8_t i = LEDS_COUNT;

            while (i--)
            {
                struct sLed color = getLedColor(i);
                float f = sin(6.283 * diff / (float) ANIMATE_PRICES_EMPTY_LOOP)
                          / 2.0 + 0.5;
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



//*********************************[getRotationTarget]**************************

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



//**********************************[handleBumperPressed]***********************

void handleBumperPressed (void)
{
    switch (state)
    {
        case STATE_DEMO:
        {
            if (price_sum > 0)
            {
                rot_target = getRotationTarget();

                uint16_t rounds = 2 * (14 + getLedPrice(rot_target)) * (15 + (time_now % 5)) / 39;
                rot_target_abs = rounds * 20 + rot_target;
                rot_acc = - ROT_VEL * ROT_VEL / (float)(2 * (rot_target_abs) + 1);
                rot_time = 1000.0 * abs_float(ROT_VEL / rot_acc);

                setState(STATE_ROTATING);
            }
            else
                setState(STATE_PRICES_EMPTY);
        }
        break;

        case STATE_MENU:
        {
            setState(STATE_MENU_SELECT);
            time_btnBumper_start = time_now;
        }
	break;
/*
        case STATE_SHOW_PRICES:
        {
            setState(STATE_MENU);
        }
        break;
*/
    }
}



//**********************************[handleBumperNotPressed]********************

void handleBumperNotPressed (void)
{
    if (time_btnBumper_start)
        time_btnBumper_start = 0;

    switch (state)
    {
        case STATE_MENU_SELECT:
        {
            setState(STATE_MENU);
        }
        break;

        case STATE_MENU_SELECTED:
        {
            //break up after MENU_SELECT_DELAY seconds
            if (time_now - time_state_start >= MENU_SELECT_DELAY)
            {
                switch (menu)
                {
                    case MENU_EXIT:
                    {
                        if (price_sum <= 0 && mode != MODE_DEFAULT)
                            setState(STATE_PRICES_EMPTY);
                        else
                            setState(STATE_DEMO);
                    }
                    break;

                    case MENU_MODE_DEFAULT:
                    {
                        setMode(MODE_DEFAULT);
                    }
                    break;

                    case MENU_MODE_COUNTDOWN:
                    {
                        setMode(MODE_COUNTDOWN);
                    }
                    break;
/*
                    case MENU_SHOW_PRICES:
                    {
                        setState(STATE_SHOW_PRICES);
                    }
                    break;
*/
                    case MENU_EEPROM_RESET:
                    {
                        //reset price array
                        price_sum = 0;
                        for (uint8_t i = 0; i < PRICES_COUNT; i++)
                        {
                            prices[i] = prices_max[i];
                            price_sum += prices[i];
                        }

                        //reset eeprom
                        eeprom_setPrices();

                        setState(STATE_DEMO);
                    }
                    break;
                }
            }
        }
    }
}



//**********************************[handleModePressed]*************************

void handleModePressed (void)
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
                if (time_now - time_btnMode_start > MENU_START_MINDELAY)
                    setState(STATE_MENU_STARTING);
            }
            else
                time_btnMode_start = time_now;
        }
        break;

        case STATE_MENU:
        {
            setState(STATE_MENU_NEXT);
            systick_delay(50);
        }
        break;

        case STATE_EEPROM_INVALID:
        {
            eeprom_save_key();
            eeprom_setPrices();
            setState(STATE_DEMO);
        }
        break;
    }
}



//**********************************[handleModeNotPressed]**********************

void handleModeNotPressed (void)
{
    if (time_btnMode_start)
        time_btnMode_start = 0;

    switch (state)
    {
        case STATE_MENU_STARTING:
        {
            if (price_sum > 0)
                setState(STATE_DEMO);
            else
                setState(STATE_PRICES_EMPTY);
        }
        break;

        case STATE_MENU_NEXT:
        {
            setState(STATE_MENU);
            menu = (menu + 1) % MENU_COUNT;
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
            handleBumperPressed();
        else
            handleBumperNotPressed();

        if (buttons_getMode())
            handleModePressed();
        else
            handleModeNotPressed();

        animate();
    }

    return 0;
}
