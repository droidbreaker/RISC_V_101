#include "../lib/usart.h"



void USARTx_CFG(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};
   

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_USART1, ENABLE); // Enable the clock for Port D and USART1

    /* USART1 TX-->D.5   RX-->D.6 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                               // Configure D.5 as alternate function push-pull (USART1 TX)
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;                         // Configure D.6 as floating input (USART1 RX)
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;                                  // Set baud rate to 115200
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                   // 8 data bits
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                        // 1 stop bit
    USART_InitStructure.USART_Parity = USART_Parity_No;                           // No parity
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // No hardware flow control
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;               // Enable both transmit and receive mode

    USART_Init(USART1, &USART_InitStructure);                                     // Initialize USART1 with the specified parameters

    USART_Cmd(USART1, ENABLE);                                                    // Enable USART1
}

void UartBufferSend(const void* buffer, uint16_t length)
{
    const uint8_t* data = (const uint8_t*)buffer;
    uint16_t tmp = 0;
    for(tmp = 0; tmp < length; tmp++)
    {
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){               // wait until the transmit data register is empty (TXE flag is set)    
        }
        USART_SendData(USART1, data[tmp]);                                        // send data
    }

}

