
#ifndef UART_H
#define UART_H

#include "gpio.h"
#include <stdint.h>

#define USART1_STATREG ((volatile USART_STATR*)0x40013800)
#define USART1_DATAREG (*(volatile uint8_t*)0x40013804)

typedef struct{
    uint32_t volatile PE:1;
    uint32_t volatile FE:1;
    uint32_t volatile NE:1;
    uint32_t volatile ORE:1;
    uint32_t volatile IDLE:1;
    uint32_t volatile RXNE:1;
    uint32_t volatile TC:1;
    uint32_t volatile TXE:1;
    uint32_t volatile LBD:1;
    uint32_t volatile CTS:1;
    uint32_t reserved:22;
}USART_STATR;

typedef struct{
    uint32_t volatile div_fraction:4;
    uint32_t volatile div_mantissa:12;
    uint32_t reserved:16;
}USART_BRR;

typedef struct{
    uint32_t volatile SBK:1;
    uint32_t volatile RWU:1;
    uint32_t volatile RE:1;
    uint32_t volatile TE:1;
    uint32_t volatile IDLEIE:1;
    uint32_t volatile RXNEIE:1;
    uint32_t volatile TCIE:1;
    uint32_t volatile TXEIE:1;
    uint32_t volatile PEIE:1;
    uint32_t volatile PS:1;
    uint32_t volatile PCE:1;
    uint32_t volatile WAKE:1;
    uint32_t volatile M:1;
    uint32_t volatile UE:1;
    uint32_t reserved:18;
}USART_CTLR1;

static inline uint8_t uart_isTxEmpty(void)
{
    return (uint8_t)(USART1_STATREG->TXE);    // 1 = TX register empty, ready to send
}

static inline uint8_t uart_isTxComplete(void)
{
    return (uint8_t)(USART1_STATREG->TC);     // 1 = last byte fully shifted out
}

static inline uint8_t uart_isRxReady(void)
{
    return (uint8_t)(USART1_STATREG->RXNE);   // 1 = byte waiting in DATAR
}

static inline void uart_flushRx(void)
{
    if(USART1_STATREG->RXNE)
        (void)(USART1->DATAR & 0xFF);         // discard stale byte
}

static inline void uart_waitTxComplete(void)
{
    while(!USART1_STATREG->TC);               // block until last bit shifted out
}


static Typedef_Gpio_Port* gpio_get_port(GPIO_Port port)    // Helper function to get the GPIO port structure based on the specified port
{
    switch(port)
    {
        case gpio_portA:
            return gpioA;
        case gpio_portC:
            return gpioC;
        case gpio_portD:
            return gpioD;
        default:
            return 0;
    }
}

static inline Typedef_Gpio_Port* gpio(GPIO_Port port)
{
    return gpio_get_port(port);
}

void uart_init(GPIO_Port port, GPIO_PIN tx_pin, GPIO_PIN rx_pin);
void uart_SendBuffer(uint8_t *buffer, uint16_t length);
uint8_t* uart_sendReceive(const uint8_t* tx_buf,uint8_t* rx_buf,uint16_t  length);
uint8_t* uart_receiveData_from_User_input(uint8_t *buffer, uint16_t maxLength);
uint8_t uart_receiveByte();

#endif  /* UART_H */