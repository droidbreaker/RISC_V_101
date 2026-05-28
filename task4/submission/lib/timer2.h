
#ifndef TIM2_H
#define TIM2_H
#include <stdint.h>



typedef struct
{
    volatile uint32_t CTLR1;    // 0x00 control register 1
    volatile uint32_t CTLR2;    // 0x04 control register 2
    volatile uint32_t SMCFGR;   // 0x08 slave mode config
    volatile uint32_t DMAINTENR;// 0x0C DMA/interrupt enable                        most important for Interrupt
    volatile uint32_t INTFR;    // 0x10 interrupt flag register
    volatile uint32_t SWEVGR;   // 0x14 software event generation                   most important (Event)
    volatile uint32_t CHCTLR1;  // 0x18 capture/compare control 1
    volatile uint32_t CHCTLR2;  // 0x1C capture/compare control 2
    volatile uint32_t CCER;     // 0x20 capture/compare enable
    volatile uint32_t CNT;      // 0x24 counter                                     most important
    volatile uint32_t PSC;      // 0x28 prescaler                                   most important
    volatile uint32_t ATRLR;    // 0x2C auto-reload register                        most important
    volatile uint32_t CH1CVR;   // 0x34 compare/capture register 1
    volatile uint32_t CH2CVR;   // 0x38 compare/capture register 2
    volatile uint32_t CH3CVR;   // 0x3C compare/capture register 3
    volatile uint32_t CH4CVR;   // 0x40 compare/capture register 4
    volatile uint32_t DMACFGR;  // 0x48 DMA control register
    volatile uint32_t DMAADR;   // 0x4C DMA address register in continuous mode 
} TIM2_TypeDef;

// RCC APB1 enable register
typedef struct
{
    volatile uint32_t TIM2EN  : 1;  // Timer 2 module clock enable bit. 
    volatile uint32_t reserved1: 10;
    volatile uint32_t WWDGEN : 1;   // Window watchdog clock enable bit.
    volatile uint32_t reserved2: 9;
    volatile uint32_t I2C1EN : 1;   // I2C 1 interface clock enable bit.
    volatile uint32_t reserved3: 6;
    volatile uint32_t PWREN : 1;    // Power interface module clock enable bit.
    volatile uint32_t reserved4: 3;
} RCC_APB1PCENR_TypeDef;


void     tim2_init(void);
void     tim2_CNT_reset(void);
uint16_t tim2_get_ms(void);
uint8_t  tim2_elapsed(uint16_t start_ms, uint16_t timeout_ms);


#endif