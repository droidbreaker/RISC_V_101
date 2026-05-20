#ifndef USART_H                                                               
#define USART_H                                                               

#include <ch32v00x_usart.h>

void USARTx_CFG(void);                                                         // Function to configure USART1
void UartBufferSend(const void* buffer, uint16_t length);                      // Function to send a buffer of data over USART1


#endif // USART_H