/*******************************************************************************
* main.c                                                                       *
* ======                                                                       *
*                                                                              *
* Version: 11.0.0                                                              *
* Date   : 26.09.21                                                            *
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



//*********************************<Flags>**************************************
#define MODE_DEFAULT           1
#define MODE_COUNTDOWN         2

#define STATE_DEMO             0
#define STATE_ROTATING         1
#define STATE_ROTATE_FINISH    2
#define STATE_PRICE            3
#define STATE_PRICE_STOPPING   4
#define STATE_MENU_STARTING    5
#define STATE_MENU             6
#define STATE_MENU_NEXT        7
#define STATE_MENU_SELECTING   8
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
uint8_t mode  = MODE_DEFAULT;         // will be overriden during
                                      // gluecksrad_init()

// program state
uint8_t state = STATE_EEPROM_INVALID; // will be overriden during
                                      // gluecksrad_init()

// menu state
uint8_t menu  = MENU_EXIT;            // will be overriden during
                                      // start of menu



//*********************************<Constants>**********************************
#define VERSION 11
#define EEPROM_KEY (0xA760 + VERSION)

#define PRICES_COUNT 5
#define PRICES_MAX { 5, 4, 3, 2, 1 }

#define ROT_VEL 80


#define MENU_INIT_DELAY           1000
#define MENU_PRICE_DELAY           500
#define MENU_RESET_DELAY          3000

#define MENU_START_MINDELAY       1000
#define MENU_START_MAXDELAY       3000

#define MENU_NEXT_MINDELAY          50
#define MENU_NEXT_DELAY            200
#define MENU_SELECT_MINDELAY       500
#define MENU_SELECT_MAXDELAY      3000

#define ANIMATE_PRICES_EMPTY_LOOP 4000


#define EEPROM_ADR_KEY1             0x20
#define EEPROM_ADR_MODE             0x22
#define EEPROM_ADR_PRICE_COUNT      0x23
#define EEPROM_ADR_PRICES_MAX_START 0x30
#define EEPROM_ADR_PRICES_START     0x40
#define EEPROM_ADR_KEY2             0x50


const struct sLed menu_colors[MENU_COUNT] = {
    {        0,        0, LEDS_MAX },
    {        0, LEDS_MAX,        0 },
    { LEDS_MAX, LEDS_MAX,        0 },
    { LEDS_MAX,        0,        0 }
};

const struct sLed price_colors[PRICES_COUNT] = {
    {        0,        0, LEDS_MAX },
    {        0, LEDS_MAX,        0 },
    { LEDS_MAX,        0,        0 },
    { LEDS_MAX, LEDS_MAX, LEDS_MAX },
    { LEDS_MAX, LEDS_MAX,        0 }
};

//array of maximum number of prices
const int16_t prices_max[PRICES_COUNT] = PRICES_MAX;



//*********************************<Macros>*************************************
#define eeprom_read_uint8()      eeprom_read()
#define eeprom_write_uint8(data) eeprom_write(data)

#define getPriceColor(i) price_colors[i]
#define getLedColor(i) price_colors[getLedPrice(i)]

#define getMenuColor(i) menu_colors[i]
#define getLedMenu(i) ((i)*MENU_COUNT/LEDS_COUNT)
#define getLedMenuColor(i) menu_colors[getLedMenu(i)]



//*********************************<Prototypes>*********************************
float   abs_float              (float v);

void    updateTime             (void);


uint8_t getLedPrice            (uint8_t i);
void    prices_clear           (void);
void    prices_set_default     (void);
void    prices_update_sum      (void);

void    eeprom_reset           (void); // overriding eeprom
uint8_t eeprom_check           (void); // only reading eeprom
uint8_t eeprom_load            (void); // checks the eeprom first
                                       // afterwards, loads mode & prices
uint8_t eeprom_save_mode                 (void); // checks the eeprom first
void    eeprom_save_prices_without_check (void); // no check is done!

void    gluecksrad_init        (void);
void    setMode                (uint8_t md); // switches to the correct state
                                             // (doesn't save to eeprom)
void    setState               (uint8_t st);

uint8_t _getRotationTarget     (void); // internal helper function
void    setRotationTarget      (void);

void    handleButtons          (void);
void    updateStateMachine     (void);

void    animate                (void);

int     main                   (void);



//*********************************<Math>***************************************
float abs_float (float v)
{
    if (v > 0)
        return v;
    else
       return -v;
}



//*********************************[time]***************************************
// 32-bit timer in milliseconds -> overflow after: 49d 17h 2m 47s 296ms
uint32_t time_now = 0,  time_last = 0;

// time when Mode-Button press started
uint32_t time_btnMode_start = 0;

// time when the current state started
uint32_t time_state_start;


void updateTime (void)
{
    time_last = time_now;
    time_now = systick_get();
}



//*********************************[prices]*************************************
//array of current number of prices
int16_t prices[PRICES_COUNT];

//sum of available prices
int16_t price_sum = 0;


uint8_t getLedPrice (uint8_t i)
{
    if (i        % 2 ==  1) return 0; // 1, 3, 5, ..., 19 (50%)
    if ((i % 10) % 6 ==  0) return 1; // 0, 6, 10, 16     (20%)
    if ((i % 10) % 4 ==  0) return 2; // 4, 8, 14, 18     (20%)
    if (i            == 12) return 3; // 12               ( 5%)
    if (i            ==  2) return 4; // 2                ( 5%)

    return 0;
}


void prices_clear (void)
{
    price_sum = 0;
    for (uint8_t i = 0; i < PRICES_COUNT; i++)
    {
        prices[i] = 0;
    }
}

void prices_set_default (void)
{
    price_sum = 0;
    for (uint8_t i = 0; i < PRICES_COUNT; i++)
    {
        prices[i]  = prices_max[i];
        price_sum += prices_max[i];
    }
}

void prices_update_sum (void)
{
    price_sum = 0;
    for (uint8_t i = 0; i < PRICES_COUNT; i++)
    {
        price_sum += prices[i];
    }
}



//*********************************[eeprom]*************************************
/* EEPROM usage
 *
 * 0x20-0x21  EEPROM_KEY
 * 0x22       Mode
 * 0x23       PRICES_COUNT
 * 0x24-0x2F  (reserved)
 * 0x30-0x31  prices_max[0]
 * 0x32-0x33  prices_max[1]
 * ...
 * 0x40-0x41  prices[0]
 * 0x42-0x43  prices[1]
 * ...
 * 0x50-0x51  EEPROM_KEY
 */


// reset the EEPROM
void eeprom_reset (void)
{
    eeprom_adress_set(EEPROM_ADR_KEY1);
    eeprom_write_uint16(EEPROM_KEY);
    eeprom_write_uint16(EEPROM_KEY);


    // write initial EEPROM_KEY
    eeprom_adress_set(EEPROM_ADR_KEY1);
    eeprom_write_uint16(EEPROM_KEY);

    // write final EEPROM_KEY
    eeprom_adress_set(EEPROM_ADR_KEY2);
    eeprom_write_uint16(EEPROM_KEY);

    // write mode
    eeprom_adress_set(EEPROM_ADR_MODE);
    eeprom_write_uint8(mode);

    // write PRICES_COUNT
    eeprom_adress_set(EEPROM_ADR_PRICE_COUNT);
    eeprom_write_uint8(PRICES_COUNT);

    // write prices_max
    eeprom_adress_set(EEPROM_ADR_PRICES_MAX_START);
    for (uint8_t i = 0; i < PRICES_COUNT; i++)
    {
        eeprom_write_uint16(prices_max[i]);
    }

    // write prices
    eeprom_adress_set(EEPROM_ADR_PRICES_START);
    for (uint8_t i = 0; i < PRICES_COUNT; i++)
    {
        eeprom_write_uint16(prices[i]);
    }
}

// check the EEPROM for
//    programm version,
//    the price count,
//    the maximum number of prices and
//    the range of the stored prices
uint8_t eeprom_check (void)
{
    // check initial EEPROM_KEY
    eeprom_adress_set(EEPROM_ADR_KEY1);
    if (eeprom_read_uint16() != EEPROM_KEY) return 0;

    // check final EEPROM_KEY
    eeprom_adress_set(EEPROM_ADR_KEY2);
    if (eeprom_read_uint16() != EEPROM_KEY) return 0;

    // check mode
    eeprom_adress_set(EEPROM_ADR_MODE);
    uint8_t temp_mode = eeprom_read_uint8();
    if ((temp_mode == 0) || (temp_mode > 2)) return 0;

    // check PRICES_COUNT
    eeprom_adress_set(EEPROM_ADR_PRICE_COUNT);
    if (eeprom_read_uint8() != PRICES_COUNT) return 0;

    // check prices_max
    eeprom_adress_set(EEPROM_ADR_PRICES_MAX_START);
    for (uint8_t i = 0; i < PRICES_COUNT; i++)
    {
        if (eeprom_read_uint16() != prices_max[i]) return 0;
    }

    // check ranges of prices
    eeprom_adress_set(EEPROM_ADR_PRICES_START);
    int16_t temp_price;
    for (uint8_t i = 0; i < PRICES_COUNT; i++)
    {
        temp_price = (int16_t) eeprom_read_uint16();
        if ((temp_price < 0) || (temp_price > prices_max[i])) return 0;
    }

    // all fine :-)
    return 1;
}

// load content (mode & prices) from EEPROM
uint8_t eeprom_load (void)
{
    if (! eeprom_check())
        return 0;

    // load mode
    eeprom_adress_set(EEPROM_ADR_MODE);
    mode = eeprom_read_uint8();
    // check again!
    if ((mode == 0) || (mode > 2)) return 0;

    // load prices
    price_sum = 0;
    eeprom_adress_set(EEPROM_ADR_PRICES_START);
    int16_t temp;
    for (uint8_t i = 0; i < PRICES_COUNT; i++)
    {
        temp = (int16_t) eeprom_read_uint16();
        // check again!
        if ((temp < 0) || (temp > prices_max[i]))
            return 0;

        // store prices
        prices[i] = temp;
        // calculate sum
        price_sum += prices[i];
    }
    return 1;
}

// save current mode to EEPROM, if possible
uint8_t eeprom_save_mode (void)
{
    if (!eeprom_check())
    {
        return 0;
    }

    eeprom_adress_set(EEPROM_ADR_MODE);
    eeprom_write_uint8(mode);

    return 1;
}

// save current prices to EEPROM - no EEPROM check is done!
void eeprom_save_prices_without_check (void)
{
    if (mode == MODE_COUNTDOWN)
    {
        // write prices
        eeprom_adress_set(EEPROM_ADR_PRICES_START);
        for (uint8_t i = 0; i < PRICES_COUNT; i++)
        {
            eeprom_write_uint16(prices[i]);
        }
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

    // load current mode (& price list)
    if (eeprom_load()) {

        // show current mode on startup
        if (mode == MODE_COUNTDOWN)
            leds_setAll2(getMenuColor(MENU_MODE_COUNTDOWN));
        else if (mode == MODE_DEFAULT)
            leds_setAll2(getMenuColor(MENU_MODE_DEFAULT  ));
        else // this should never happen ...
            leds_setAll(LEDS_MAX, 0, LEDS_MAX); // purple

        // switch to the correct mode afterwards
        //   (this will set the correct state & may init the prices)
        setMode(mode);
    } else {
        // show error on startup
        leds_setAll(LEDS_MAX, 0, 0); // full red

        // switch to default mode afterwards
        setMode(MODE_DEFAULT);
    }

    systick_delay(MENU_INIT_DELAY);
    updateTime();
}



//*********************************[setMode]************************************
void setMode (uint8_t md)
{
    mode = md;

    switch (md)
    {
        case MODE_DEFAULT:
        {
            prices_set_default();
            setState(STATE_DEMO);
        }
        break;

        case MODE_COUNTDOWN:
        {
            prices_update_sum();
            if (price_sum <= 0)
                setState(STATE_PRICES_EMPTY);
            else
                setState(STATE_DEMO);
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



//*********************************[setRotationTarget]**************************
// rotation target
uint16_t rot_target_abs; // absolute target including rounds
uint8_t  rot_target;     // target led (0-20]

// rotation acceleration and time
float rot_acc, rot_time;


// internal helper function
uint8_t _getRotationTarget (void)
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

    return 0;
}

void setRotationTarget (void)
{

    rot_target = _getRotationTarget();

    uint16_t rounds = 2 * (14 + getLedPrice(rot_target)) * (15 + (time_now % 5)) / 39;
    rot_target_abs = rounds * 20 + rot_target;
    rot_acc = - ROT_VEL * ROT_VEL / (float)(2 * (rot_target_abs) + 1);
    rot_time = 1000.0 * abs_float(ROT_VEL / rot_acc);
}



//**********************************[handleButtons]*****************************
uint8_t btnMode          = 0; // current state as boolean
uint8_t btnMode_released = 0; // boolean flag for falling flank
uint8_t bumper  = 0; // current state as boolean

void handleButtons (void)
{
    bumper  = buttons_getBumper();

    uint8_t btnMode_previous = btnMode;
    btnMode = buttons_getMode();
    btnMode_released = btnMode_previous && !btnMode;

    if (btnMode)
    {
        if ((!btnMode_previous) || (time_btnMode_start == 0))
            time_btnMode_start = time_now;
    } else {
        // in case of release, delay clearing of start time by one cycle
        if (!btnMode_released)
            time_btnMode_start = 0;
    }
}



//**********************************[updateStateMachine]************************
void updateStateMachine (void)
{
    uint32_t diff_state   = time_now - time_state_start;
    uint32_t diff_btnMode = 0;
    if (time_btnMode_start)
        diff_btnMode = time_now - time_btnMode_start;

    switch (state)
    {
        case STATE_DEMO:
        {
            if (diff_btnMode > MENU_START_MINDELAY)
                setState(STATE_MENU_STARTING);
            else if (bumper)
            {
                if (price_sum > 0)
                {
                    setRotationTarget();
                    setState(STATE_ROTATING);
                }
                else
                    setState(STATE_PRICES_EMPTY);
            }
        }
        break;

        case STATE_ROTATING:
        {
            if (diff_state >= rot_time)
            {
                if (mode == MODE_COUNTDOWN)
                {
                    prices[getLedPrice(rot_target)]--;
                    prices_update_sum();
                    eeprom_save_prices_without_check();
                }

                setState(STATE_ROTATE_FINISH);
            }
        }
        break;

        case STATE_ROTATE_FINISH:
        {
            if (diff_state >= MENU_PRICE_DELAY)
                setState(STATE_PRICE);
        }
        break;

        case STATE_PRICE:
        {
            if (btnMode)
            {
                setState(STATE_PRICE_STOPPING);
            }

        }
        break;
        case STATE_PRICE_STOPPING:
        {
            if (btnMode_released)
            {
                if (price_sum > 0)
                    setState(STATE_DEMO);
                else
                    setState(STATE_PRICES_EMPTY);
            }
        }

        case STATE_MENU_STARTING:
        {
            if (btnMode_released)
            {
                if (diff_btnMode >= MENU_START_MAXDELAY)
                {
                    setState(STATE_MENU);
                    menu = MENU_EXIT;
                } else {
                    if (price_sum > 0)
                        setState(STATE_DEMO);
                    else
                        setState(STATE_PRICES_EMPTY);
                }
            }
        }
        break;

        case STATE_MENU:
        {
            if (diff_btnMode >= MENU_SELECT_MINDELAY)
                setState(STATE_MENU_SELECTING);
            else if (btnMode_released && (diff_btnMode >= MENU_NEXT_MINDELAY))
                setState(STATE_MENU_NEXT);
        }
        break;

        case STATE_MENU_NEXT:
        {
            if (diff_state > MENU_NEXT_DELAY)
            {
                setState(STATE_MENU);
                menu = (menu + 1) % MENU_COUNT;
            }
        }
        break;

        case STATE_MENU_SELECTING:
        {
            if (diff_btnMode >= MENU_SELECT_MAXDELAY)
                setState(STATE_MENU_SELECTED);
            else if (!btnMode)
                setState(STATE_MENU);
        }
        break;

        case STATE_MENU_SELECTED:
        {
            if (btnMode_released)
            {
                switch (menu)
                {
                    case MENU_EXIT:
                    {
                        if (price_sum > 0)
                            setState(STATE_DEMO);
                        else
                            setState(STATE_PRICES_EMPTY);
                    }
                    break;

                    case MENU_MODE_DEFAULT:
                    {
                        setMode(MODE_DEFAULT);
                        if (!eeprom_save_mode())
                            setState(STATE_EEPROM_INVALID);
                        else
                            setState(STATE_DEMO);
                    }
                    break;

                    case MENU_MODE_COUNTDOWN:
                    {
                        mode = MODE_COUNTDOWN; // needed for eeprom_save_mode()
                        if (!eeprom_save_mode() || !eeprom_load())
                        {
                            setMode(MODE_COUNTDOWN); // sets mode in case of
                                                     // error
                            setState(STATE_EEPROM_INVALID);
                        } else
                            setMode(MODE_COUNTDOWN); // selects between DEMO
                                                     // and PRICES_EMPTY
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
                        prices_set_default();

                        //reset eeprom
                        eeprom_reset();

                        // switch to current mode (changes state)
                        setMode(mode);
                    }
                    break;
                }
            }
        }
        break;
/*
        case STATE_SHOW_PRICES:
        {
            if (bumper)
            {
                setState(STATE_MENU);
            }
        }
        break;
*/
        case STATE_PRICES_EMPTY:
        {
            if (diff_btnMode > MENU_START_MINDELAY)
                setState(STATE_MENU_STARTING);
        }
        break;

        case STATE_EEPROM_INVALID:
        {
            if (btnMode_released && (diff_btnMode > MENU_RESET_DELAY))
            {
                //reset price array
                prices_set_default();

                //reset eeprom
                eeprom_reset();

                // switch to current mode (changes state)
                setMode(mode);
            }
        }
        break;
    }
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
            // set current led to 100% brightness
            // set other leds  to  10% brightness (1/6)

            uint16_t led = ((float)(diff * (rot_acc * diff / 2000. + ROT_VEL) / 1000.));
            if (led > rot_target_abs)
                led = rot_target_abs;

            uint8_t i = LEDS_COUNT;

            while (i--)
            {
                struct sLed color = getLedColor(i);
                uint8_t d = 5 * (i != led % 20) + 1;
                leds_set(i, color.r / d, color.g / d, color.b / d);
            }
        }
        break;

        case STATE_ROTATE_FINISH:
        {
            // nothing todo - freeze last leds (STATE_ROTATING)
        }
        break;

        case STATE_PRICE:
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
                    uint16_t msec50 = diff / 50;

                    while (i--)
                    {
                        struct sLed led_color = getLedColor(i);

                        if ((msec50 - i + 40) % 4)
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

        case STATE_MENU_SELECTING:
        {
            // fade somehow all menu leds into current menu leds

            struct sLed led_color, menu_color;

            uint8_t
                f = 10 * diff / MENU_SELECT_MAXDELAY,
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
        }
        break;

        case STATE_MENU_SELECTED:
        {
            // blink all leds in current menu-color (0%<->100% @1Hz)
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
            uint8_t f = 0;
            if (time_btnMode_start)
            {
                f = (time_now - time_btnMode_start) * 10 / MENU_RESET_DELAY;
                if (f > LEDS_MAX)
                    f = LEDS_MAX;
            }
            uint8_t i = LEDS_COUNT;
            while (i--)
            {
                uint8_t d = (i + (diff % 1000 < 500)) % 2;
                leds_set(i, d - (d * f / 10) + f, 0, 0);
            }
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

        handleButtons();
        updateStateMachine();

        animate();
    }

    return 0;
}
