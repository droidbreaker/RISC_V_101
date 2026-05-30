#include "../lib/uart.h"
#include "../lib/gpio.h"
#include <string.h>

#define USART1_STATREG ((volatile USART_STATR*)0x40013800)
#define USART1_DATAREG (*(volatile uint8_t*)0x40013804)

/*Tx: PD5, Rx: PD6 by default*/
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

void uart_init(GPIO_Port port, GPIO_PIN tx_pin, GPIO_PIN rx_pin)
{
    RCC_APB2PCENR volatile *RCC1 = (RCC_APB2PCENR*)0x40021018;
    RCC1->USART1EN = 1;                    // Enable clock for USART1)
    if(port == gpio_portD)
    {
        RCC1->IOPDEN   = 1;                    // Enable clock for GPIO port D
    }
    else if(port == gpio_portA)
    {
        RCC1->IOPAEN   = 1;                    // Enable clock for GPIO port A
    }
    else if(port == gpio_portC)
    {
        RCC1->IOPCEN   = 1;                    // Enable clock for GPIO port C
    }
    RCC1->AFIOEN   = 1;                    // Enable clock for Alternate Function I/O

    //by default USART pins are PD5 (TX) and PD6 (RX) on this microcontroller, so we will configure those pins accordingly.
    // PD5 TX
    gpio(port)->gpio_cfglr &= ~(0xF << (tx_pin * 4));      // Clear the 4 bits corresponding to pin 5
    gpio(port)->gpio_cfglr |= (0xB << (tx_pin * 4));       // Set MODE to 11 (50MHz) Set CNF to 10 (Alternate function output Push-pull) 1011b
   

    // PD6 RX
    gpio(port)->gpio_cfglr &= ~(0xF << (rx_pin * 4));    // Clear the 4 bits corresponding to pin 6
    gpio(port)->gpio_cfglr |= (0x4 << (rx_pin * 4));     // Set MODE to 00 (Input mode) Set CNF to 01 (Alternate function input floating) 0100b
  


    USART1->BRR = (uint16_t)(24000000UL / 9600);          

    USART_CTLR1 volatile *pUSART1ctlr1 = (USART_CTLR1*)0x4001380C;

    pUSART1ctlr1->UE = 1;                // Enable USART1
    pUSART1ctlr1->TE = 1;                // Enable transmitter
    pUSART1ctlr1->RE = 1;                // Enable receiver
}

void uart_SendBuffer(uint8_t *buffer, uint16_t length)
{

    for(uint16_t i = 0; i < length; i++)
    {
        while(USART1_STATREG->TXE == 0);                   // Wait until transmit data register is empty

        USART1_DATAREG = buffer[i];
    }

    while(USART1_STATREG->TC == 0);                         // Wait until transmission is complete
}

uint8_t* uart_receiveData(uint8_t *buffer, uint16_t maxLength)
{
    if (buffer == NULL || maxLength < 2)
        return NULL;

    uint16_t i = 0;
    uint8_t byte;

    while (i < (maxLength - 1))
    {
        while (USART1_STATREG->RXNE == 0);  // wait for byte

        byte = (uint8_t)USART1_DATAREG;     // read immediately — no delay

        if (byte == '\r' || byte == '\n')
            break;

        buffer[i++] = byte;
        
    }

    buffer[i] = '\0';
    return buffer;
}

uint8_t uart_receiveByte()
{
    while(USART1_STATREG->RXNE == 0);                   // Wait until receive data register is not empty

    return (uint8_t)(USART1_DATAREG);                 // Read and return received byte
}