
#ifndef UART_H
#define UART_H

#include "gpio.h"
#include <stdint.h>

#define USART1_STATREG ((volatile USART_STATR*)0x40013800)
#define USART1_DATAREG (*(volatile uint8_t*)0x40013804)

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
