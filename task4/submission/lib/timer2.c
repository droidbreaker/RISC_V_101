#include "../lib/timer2.h"

#define TIM2  ((TIM2_TypeDef*)0x40000000)
#define Rcc_APB1PCENR ((RCC_APB1PCENR_TypeDef*)0x4002101C)

/**
 * @brief : Initializes TIM2 as a 1ms timebase.
 *
 * RCC_APB1PCENR.TIM2EN = 1   — enables TIM2 peripheral clock.
 * TIM2->CTLR1 = 0            — stops the timer before configuration.
 * TIM2->PSC   = 24000 - 1    — divides 24MHz APB1 clock → 1kHz (1 tick = 1ms).
 * TIM2->ATRLR = 0xFFFF       — sets counter ceiling to 65535ms (~65.5s).
 * TIM2->SWEVGR |= UG (bit0)  — forces PSC/ATRLR shadow register update.
 * TIM2->CNT   = 0            — clears counter to start from known state.
 * TIM2->INTFR = 0            — clears UIF flag raised by UG event.
 * TIM2->CTLR1 |= CEN (bit0)  — starts the counter.
 *
 * @param : Nothing as parameter/ function arguments.
 * @return : Nothing to return.
 */
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

/**
 * @brief : Resets the TIM2 counter to zero.
 *
 * TIM2->CNT = 0  — directly overwrites the Counter Register without
 *                  stopping the timer or altering any other register.
 *
 * @param : No parameter argument
 * @return : Nothing to return.
 */
void tim2_CNT_reset(void)
{
    TIM2->CNT = 0;                            // reset counter to 0
}

/**
 * @brief : Returns the current TIM2 counter value in milliseconds.
 *
 * TIM2->CNT (read) — raw 16-bit counter value equals elapsed ms
 *                    directly, since PSC is set for 1 tick per ms.
 *
 * @param : None
 * @return : Current TIM2->CNT value (0 – 65535 ms).
 */
uint16_t tim2_get_ms(void)
{
    return (uint16_t)(TIM2->CNT);                     // each count = 1ms
}

/** 
 * @brief : Checks whether a timeout has elapsed since a given start time.
 * Only one counter rollover is handled correctly (max 65535ms).
 * TIM2->CNT (read via tim2_get_ms()) — sampled into 'now'.
 * elapsed = (uint16_t)(now - start_ms) — unsigned subtraction handles
 *           a single CNT rollover (0xFFFF → 0x0000) naturally.
 * Returns 1 if elapsed >= timeout_ms, otherwise 0.
 *
 * @param  "@start_ms"    TIM2->CNT value captured at interval start.
 *  ,@"timeout_ms"  Desired timeout in milliseconds (1 – 65535).
 *
 * @return : 1  Timeout reached or exceeded.
 * @return : 0  Timeout not yet elapsed.
 *   
 */
uint8_t tim2_elapsed(uint16_t start_ms, uint16_t timeout_ms)
{
    uint16_t now = tim2_get_ms();                     // storing the value in now which will be in ms
    uint16_t elapsed = now - start_ms;                // handles uint16 rollover naturally
    return (elapsed >= timeout_ms) ? 1 : 0;           // if elapsed is greater then timeout_ms then it will return 1, else 0.
}