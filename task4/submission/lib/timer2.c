#include "../lib/timer2.h"

#define TIM2  ((TIM2_TypeDef*)0x40000000)
#define Rcc_APB1PCENR ((RCC_APB1PCENR_TypeDef*)0x4002101C)


void tim2_init(void)
{
    
    Rcc_APB1PCENR->TIM2EN = 1;                 // 1. Enable TIM2 clock
    
    TIM2->CTLR1 = 0;                           // 2. Stop timer before configuring
 
    TIM2->PSC = 24000 - 1;                     // 3. Prescaler — 24MHz / 24000 = 1kHz (1 tick per ms) 
    TIM2->ATRLR = 0xFFFF;                      // 4. Auto-reload — max range 65535 decimal
    
    TIM2->SWEVGR |= (1 << 0);                  // 5. Force prescaler update [UG bit — update generation]

    
    TIM2->CNT   = 0;                           // 6. Clear counter and flags
    TIM2->INTFR = 0;

    TIM2->CTLR1 |= (1 << 0) ;                  // 7. Start timer — CEN bit
}

void tim2_CNT_reset(void)
{
    TIM2->CNT = 0;                            // reset counter to 0
}

uint16_t tim2_get_ms(void)
{
    return (uint16_t)(TIM2->CNT);                     // each count = 1ms
}

uint8_t tim2_elapsed(uint16_t start_ms, uint16_t timeout_ms)
{
    uint16_t now = tim2_get_ms();                     // storing the value in now which will be in ms
    uint16_t elapsed = now - start_ms;                // handles uint16 rollover naturally
    return (elapsed >= timeout_ms) ? 1 : 0;           // if elapsed is greater then timeout_ms then it will return 1, else 0.
}