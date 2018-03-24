
// TODO writing own random function

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>

#define PRICES_MAX 5
#define LOOPS (00 + psum)

uint16_t prices[PRICES_MAX] = {300, 150, 150, 15, 5}, price_sum = 0;

void init() {
    uint8_t i;
    
    srand(time(0) + random()); // randomize
    
    price_sum = 0;
    for(i = 0; i < 5; i++) price_sum += prices[i];
}

uint8_t getRotationTarget (void)
{
    if (price_sum <= 0) return 0;

    // choose random price
    uint16_t ran_num = random();
    uint8_t cat, ran = (ran_num % price_sum) + 1;

    // select category dependent on the probability
    for (cat = 0; ran > prices[cat] && cat < PRICES_MAX; cat++)
        ran -= prices[cat];
    
    #define TARGET_STEP 3
    uint16_t i;
    ran_num = LEDS_COUNT * TARGET_STEP + ran_num % 20;
    
    for (i = 0; i < ran_num; i += TARGET_STEP)
        if (getLedPrice(i % 20) == cat)
            return i % 20;
    
    return 1;
}

int main() {
    init();
    const uint16_t psum = price_sum;
    
    uint16_t count[PRICES_MAX] = {0, 0, 0, 0, 0};
    uint16_t i = LOOPS;
    
    while(--i) {
        uint8_t target = getRandomTarget();
        if(prices[target]) prices[target]--;
        count[target]++;
        if(price_sum) price_sum--;
    }
    printf( "Price amount percent relat\n");
    
    for(i = 0; i < PRICES_MAX; i++) 
        printf( "%4i:  %5i %6.2f%% %5.1f\n", 
            i + 1, 
            count[i], 
            100  * count[i] / (float)LOOPS,
            psum * count[i] / (float)LOOPS
        );
    
    printf("\nend\n");
}