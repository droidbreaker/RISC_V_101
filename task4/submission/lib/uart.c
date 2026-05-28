#include "../lib/uart.h"
#include "../lib/gpio.h"
#include <string.h>
#include "../lib/timer2.h"

#define USART1_STATREG ((volatile USART_STATR*)0x40013800)
#define USART1_DATAREG (*(volatile uint8_t*)0x40013804)


// At 24MHz, 9600 baud:
// 1 byte = ~1.04ms
// loops per ms = 24000000 / 6 / 1000 = 4000 loops/ms
// For N ms timeout: timeout = N * 4000

#define BAUD_RATE       9600UL
#define UART_BYTE_TIMEOUT_MS  10     // max wait per byte

/*Tx: PD5, Rx: PD6 by default*/


void uart_init(GPIO_Port port, GPIO_PIN tx_pin, GPIO_PIN rx_pin)
{
    RCC_APB2PCENR volatile *RCC1 = (RCC_APB2PCENR*)0x40021018;
    RCC1->USART1EN = 1;                    // Enable clock for USART1)
    RCC1->AFIOEN   = 1;                    // Enable clock for Alternate Function I/O
    if(port == gpio_portD)
        RCC1->IOPDEN   = 1;                    // Enable clock for GPIO port D
    else if(port == gpio_portA)
        RCC1->IOPAEN   = 1;                    // Enable clock for GPIO port A
    else if(port == gpio_portC)
        RCC1->IOPCEN   = 1;                    // Enable clock for GPIO port C
    

    //by default USART pins are PD5 (TX) and PD6 (RX) on this microcontroller, so we will configure those pins accordingly.
    // PD5 TX
    gpio(port)->gpio_cfglr &= ~(0xF << (tx_pin * 4));    // Clear the 4 bits corresponding to pin 5
    gpio(port)->gpio_cfglr |= (0xB << (tx_pin * 4));       // Set MODE to 11 (50MHz) TX pin — AF push-pull output 50MHz (CNF=10, MODE=11 → 0xB)
   

    // PD6 RX
    gpio(port)->gpio_cfglr &= ~(0xF << (rx_pin * 4));    // Clear the 4 bits corresponding to pin 6
    gpio(port)->gpio_cfglr |= (0x4 << (rx_pin * 4));       // Set MODE to 00 (Input mode) RX pin — floating input (CNF=01, MODE=00 → 0x4)

    USART1->BRR = (uint16_t)(24000000UL / BAUD_RATE);          

    USART_CTLR1 volatile *pUSART1ctlr1 = (USART_CTLR1*)0x4001380C;

    pUSART1ctlr1->UE = 1;                // Enable USART1
    pUSART1ctlr1->TE = 1;                // Enable transmitter
    pUSART1ctlr1->RE = 1;                // Enable receiver
}

void uart_SendBuffer(uint8_t *buffer, uint16_t length)
{

    for(uint16_t i = 0; i < length; i++)
    {
        while(uart_isTxEmpty() == 0);                   // Wait until transmit data register is empty

        USART1_DATAREG = buffer[i];
    }

    while(uart_isTxComplete() == 0);                         // Wait until transmission is complete
}


// for loopback UART testing and checking for strings. Always short Rx and Tx.

uint8_t* uart_sendReceive(const uint8_t* tx_buf,uint8_t* rx_buf,uint16_t  length)     
{
    if(tx_buf == NULL || rx_buf == NULL || length == 0)
        return NULL;

    uint16_t tx_index  = 0;
    uint16_t rx_index  = 0;
    uint16_t start     = tim2_get_ms();                         // timeout reference

    
    while(!uart_isTxEmpty());                                   // Send first byte to start things off
    USART1->DATAR = tx_buf[tx_index++];

    while(rx_index < length)
    {
        
        if(tx_index < length && uart_isTxEmpty())               // Send next TX byte as soon as register is free
        {
            USART1->DATAR = tx_buf[tx_index++];
            start = tim2_get_ms();                              // reset timeout on each new TX byte
        }

        
        if(uart_isRxReady())                                    // Grab RX byte as soon as it arrives
        {
            rx_buf[rx_index++] = (uint8_t)(USART1->DATAR & 0xFF);
            start = tim2_get_ms();                              // reset timeout on each received byte
        }

       
        if(tim2_elapsed(start, UART_BYTE_TIMEOUT_MS))           // Check timeout — no activity for UART_BYTE_TIMEOUT_MS
        {
            rx_buf[rx_index] = '\0';                            // null terminate what we got
            return NULL;                                        // signal timeout to caller
        }
    }
    rx_buf[length] = '\0';
    uart_waitTxComplete();
    return rx_buf;                         // non-NULL = success
}

uint8_t* uart_receiveData_from_User_input(uint8_t *buffer, uint16_t maxLength)   // no need to short TX and RX pin in this mode.
{
    if (buffer == NULL || maxLength < 2)
        return NULL;
    uint16_t i = 0;
    uint8_t byte;
    while (i < (maxLength - 1))
    {
        while (uart_isRxReady() == 0);  // wait for byte
        byte = (uint8_t)USART1_DATAREG;     // read immediately — no delay
        if (byte == '\r' || byte == '\n')       // this will be the stopping condition.
            break;
        buffer[i++] = byte;        
    }
    buffer[i] = '\0';
    return buffer;
}

uint8_t uart_receiveByte()
{
    while(uart_isRxReady() == 0);                   // Wait until receive data register is not empty

    return (uint8_t)(USART1_DATAREG);                 // Read and return received byte
}