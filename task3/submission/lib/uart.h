

#ifndef UART_H
#define UART_H

#include "gpio.h"
typedef struct{
    volatile uint32_t PE:1;
    volatile uint32_t FE:1;
    volatile uint32_t NE:1;
    volatile uint32_t ORE:1;
    volatile uint32_t IDLE:1;
    volatile uint32_t RXNE:1;
    volatile uint32_t TC:1;
    volatile uint32_t TXE:1;
    volatile uint32_t LBD:1;
    volatile uint32_t CTS:1;
    volatile uint32_t reserved:22;
}USART_STATR;

typedef struct{
    volatile uint32_t div_fraction:4;
    volatile uint32_t div_mantissa:12;
    volatile uint32_t reserved:16;
}USART_BRR;

typedef struct{
    volatile uint32_t SBK:1;
    volatile uint32_t RWU:1;
    volatile uint32_t RE:1;
    volatile uint32_t TE:1;
    volatile uint32_t IDLEIE:1;
    volatile uint32_t RXNEIE:1;
    volatile uint32_t TCIE:1;
    volatile uint32_t TXEIE:1;
    volatile uint32_t PEIE:1;
    volatile uint32_t PS:1;
    volatile uint32_t PCE:1;
    volatile uint32_t WAKE:1;
    volatile uint32_t M:1;
    volatile uint32_t UE:1;
    volatile uint32_t reserved:18;
}USART_CTLR1;



void uart_init(GPIO_Port port, GPIO_PIN tx_pin, GPIO_PIN rx_pin);
void uart_SendBuffer(uint8_t *buffer, uint16_t length);
uint8_t* uart_receiveData(uint8_t *buffer, uint16_t length);
uint8_t uart_receiveByte();

#endif  /* UART_H */