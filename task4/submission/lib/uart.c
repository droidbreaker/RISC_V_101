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

/**
 * @brief : This function initializes the GPIO PORTx  PINx as Rx and Tx.
 * by default USART pins are PD5 (TX) and PD6 (RX) on this microcontroller, 
 * so we will configure those pins accordingly.
 * by enabling the clock for the "@UART1" "@RCC_APB2PCENR" "@RCC1->USART1EN".
 * and by enabling "@RCC1->AFIOEN" so that GPIO PIN can work as Alternate-function
 * mode.
 * Also need to configure the MODE and CNF for that Rx and Tx pins by using
 * "@CFGLR" register and setting the corresponding pin as Rx and Tx, by making 
 * TX pin — AF push-pull output 50MHz , RX pin — floating input.
 * also include the baud rate: "@BRR" by using proper register and input to that register for `9600` baudrate.
 * @param : poRT : [GPIOD] : Tx_pin : [PD5] , Rx : [PD6], this is already as default.
 * @return : nothing to return.
 * 
 */
void uart_init(GPIO_Port port, GPIO_PIN tx_pin, GPIO_PIN rx_pin)
{
	volatile RCC_APB2PCENR *RCC1 = (RCC_APB2PCENR*)0x40021018;
	RCC1->USART1EN = 1;                                               // Enable clock for USART1)
	RCC1->AFIOEN   = 1;                                               // Enable clock for Alternate Function I/O
	if(port == gpio_portD)
		RCC1->IOPDEN   = 1;                                           // Enable clock for GPIO port D
	else if(port == gpio_portA)
		RCC1->IOPAEN   = 1;                                           // Enable clock for GPIO port A
	else if(port == gpio_portC)
		RCC1->IOPCEN   = 1;                                           // Enable clock for GPIO port C


	// PD5 TX
	gpio(port)->gpio_cfglr &= ~(0xF << (tx_pin * 4));                 // Clear the 4 bits corresponding to pin 5
	gpio(port)->gpio_cfglr |= (0xB << (tx_pin * 4));                  // Set MODE to 11 (50MHz) TX pin — AF push-pull output 50MHz (CNF=10, MODE=11 → 0xB)


	// PD6 RX
	gpio(port)->gpio_cfglr &= ~(0xF << (rx_pin * 4));                 // Clear the 4 bits corresponding to pin 6
	gpio(port)->gpio_cfglr |= (0x4 << (rx_pin * 4));                  // Set MODE to 00 (Input mode) RX pin — floating input (CNF=01, MODE=00 → 0x4)

	USART1->BRR = (uint16_t)(24000000UL / BAUD_RATE);          

	volatile USART_CTLR1 *pUSART1ctlr1 = (USART_CTLR1*)0x4001380C;    // register address of "@CTLR" register.

	pUSART1ctlr1->UE = 1;                                             // Enable USART1
	pUSART1ctlr1->TE = 1;                                             // Enable transmitter
	pUSART1ctlr1->RE = 1;                                             // Enable receiver
}

/**
 * @brief : This function send the string onto the serial monitor,
 * where we can check the output as a debug interface. 
 * Using "@USART1_DATAREG" register where the data can stored byte by byte.
 * basically we are writting data onto that "@USART1_DATAREG" register.  
 * which can display on the serial monitor.
 * @param : buffer :["hello world\n"] , and its [strlen(buffer)];
 * @return : nothing to return.
 * 
 */
void uart_SendBuffer(uint8_t *buffer, uint16_t length)
{

	for(uint16_t i = 0; i < length; i++)
	{
		while(uart_isTxEmpty() == 0);                                 // Wait until transmit data register is empty

		USART1_DATAREG = buffer[i];
	}

	while(uart_isTxComplete() == 0);                                  // Wait until transmission is complete
}

/**
 * @brief : for loopback UART testing and checking for strings. Always short Rx and Tx.
 * This function is only for loopback UART testing, what is written on the transmitted buffer
 * need to store it in the received buffer byte by byte , by using proper timer timeout.
 * if the charachter didnt received under 10ms after getting last charachter then it will stop receiving
 * the new data so that it will be safe for the string to send in the one-go.
 * also we will reset the time "@tim2_get_ms()" when we recieve each byte.  
 * and take the data from the "@DATAREG" as soon as its been written by the one byte.
 * 
 * @param : tx_buf : ["this is test\n"] and rx_buf : will store the received bytes one by one.
 * length : the "@tx_buf" length  which has final end of line '\0'. 
 * 
 * @return : it will return the received string "@rx_buf" from the serial monitor.
 * 
 */ 
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
	return rx_buf;                                              // non-NULL = success
}

/**
 * @brief : This function is used for application as ECHO message on screen,
 * what we have written on the serial monitor that can be re-send it on the serial monitor, 
 * so that we can have proper understanding of how this UART is working properly or not.
 * By providing the ending line as '\r\n' we can get the proper condition,
 * so that till the "CR-LF" doesnt come till then write the bytes onto the DATA register and store it onto buffer.
 * after that buffer is filled with the string/ data. we can push that string onto the serial monitor.
 * by returning the buffer which has the data filled with the data which is coming from the serial monitor,
 * as user input data.
 * 
 * @param : buffer : [empty buffer] , maxLength: [100] or as per the need.
 *
 * @return : return the buffer which has been filled with the data from the user input."@buffer".
 *
 */

uint8_t* uart_receiveData_from_User_input(uint8_t *buffer, uint16_t maxLength)   // no need to short TX and RX pin in this mode.
{
	if (buffer == NULL || maxLength < 2)
		return NULL;
	uint16_t i = 0;
	uint8_t byte;
	while (i < (maxLength - 1))
	{
		while (uart_isRxReady() == 0);                         // wait for byte
		byte = (uint8_t)USART1_DATAREG;                        // read immediately — no delay
		if (byte == '\r' || byte == '\n')                      // this will be the stopping condition.
			break;
		buffer[i++] = byte;        
	}
	buffer[i] = '\0';
	return buffer;
}

/**
 * @brief : This function will recieve only 1 byte from the serial monitor.
 * This can be used to select the option from the menu.
 * by writting one byte onto "@USART1_DATAREG" , we can directly send it back to the 
 * application as return value.
 * 
 * @param : nothing as parameter needed.
 * 
 * @return : it will return the data on the "@USART1_DATAREG" directly.
 * 
 */
uint8_t uart_receiveByte()
{
	while(uart_isRxReady() == 0);                             // Wait until receive data register is not empty

	return (uint8_t)(USART1_DATAREG);                         // Read and return received byte
}
