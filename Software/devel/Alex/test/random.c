
// TODO writing own random function

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>

#define PRICES_COUNT 5
#define LEDS_COUNT 20
#define LOOPS psum

#define random rand

uint16_t prices[PRICES_COUNT] = {300, 150, 150, 15, 5}, price_sum = 0;


void init() {
    uint8_t i;
    
    srand(time(0) + rand()); // randomize
    
    price_sum = 0;
    for(i = 0; i < 5; i++)
        price_sum += prices[i];
}

uint8_t getLedPrice (uint8_t i)
{
    if (i        % 2 ==  1) return 0; // 1, 3, 5, ..., 19 (50%)
    if ((i % 10) % 6 ==  0) return 1; // 0, 6, 10, 16     (20%)
    if ((i % 10) % 4 ==  0) return 2; // 4, 8, 14, 18     (20%)
    if (i            == 12) return 3; // 12               ( 5%)
    if (i            ==  2) return 4; // 2                ( 5%)

    return 0;
}

uint8_t getRotationTarget (void)
{
    if (price_sum <= 0) return 0;

    // choose random price
    uint16_t num = random(), ran = num % price_sum;
    uint8_t cat;

    // select category dependent on the probability
    for (cat = 0; cat < PRICES_COUNT && ran >= prices[cat]; cat++)
        ran -= prices[cat];
    
    //printf("%4i %6i %3i %2i\n", price_sum, num, ran, cat);
    
    #define TARGET_STEP 3
    uint16_t i;
    num = LEDS_COUNT * TARGET_STEP + num % 20;
    
    for (i = 0; i < num; i += TARGET_STEP)
        if (getLedPrice(i % 20) == cat)
            return i % 20;
    
    //leds_setAll(1,1,0);
    //delay_ms(100);
    return 1;
}

int main() {
    init();
    
    const uint16_t psum = price_sum;
    uint16_t count_price[PRICES_COUNT];
    uint16_t count_target[LEDS_COUNT];
    uint16_t i;
    
    i = LEDS_COUNT;
    while(i--) count_target[i] = 0;
    
    i = PRICES_COUNT;
    while(i--) count_price[i] = 0;
    
    i = LOOPS;
    
    while (--i) {
        uint8_t target = getRotationTarget();
        uint8_t price = getLedPrice(target);
        prices[target]--;
        //prices[price]--;
        
        count_price[price]++;
        count_target[target]++;
        if(price_sum > 0) price_sum--;
    }
    printf( "\nPrice amount percent relat\n");
    
    for(i = 0; i < PRICES_COUNT; i++) 
        printf( "%5i:  %5i %6.2f%% %5.1f\n", 
            i + 1, 
            count_price[i], 
            100  * count_price[i] / (float)LOOPS,
            psum * count_price[i] / (float)LOOPS
        );
    
    printf( "\nTarget amount percent relat\n");
    for(i = 0; i < LEDS_COUNT; i++) 
        printf( "%5i:  %5i %6.2f%% %5.1f\n", 
            i + 1, 
            count_target[i], 
            100  * count_target[i] / (float)LOOPS,
            psum * count_target[i] / (float)LOOPS
        );
    
    printf("\nend\n");
}
