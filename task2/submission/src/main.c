
#include "../lib/gpio.h"
#include "../lib/usart.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

int main(){
    
    SystemCoreClockUpdate();                                           // Update the system core clock frequency
    USARTx_CFG();                                                      // Configure USART1 for communication
    Delay_Init();                                                      // Initialize the delay function
    GPIO_Config();                                                     // Configure GPIO pins for output

    uint32_t i = 10; 
    uint8_t buffer[50] = {0};      
    UartBufferSend("=======================================\r\n", 43);                                    // buffer to store the message to be sent over USART
    UartBufferSend("      Board Name: CH32V003\r\n", 30);
    UartBufferSend("     Welcome to RISC-V embedded systems\r\n", 43);
    UartBufferSend("     Firmware Version: 1.0\r\n", 30);
    UartBufferSend("=======================================\r\n", 43);
    while(i != 0)
    {
        sprintf((char*)buffer, "Booting in  %ld seconds\r\n", i--);    // Format the booting message with the current countdown value 
        UartBufferSend((uint8_t *)buffer, strlen((char*)buffer));      // send the message over USART
        Delay_Ms(1000); // delay for 1 second
    }
    UartBufferSend("Booting complete!\r\n", 20);
    UartBufferSend("System is now running.\r\n", 24);
    UartBufferSend("=======================================\r\n", 43);
    UartBufferSend("     Welcome to Blink LED exercise\r\n", 37);
    UartBufferSend("=======================================\r\n", 43);
    while(1){

        GPIO_SetBits(GPIOD, GPIO_Pin_6);                                // Set PD.6 high to turn on the LED in-built LED
        Delay_Ms(1000);                                                 // Delay for 1 second
        GPIO_ResetBits(GPIOD, GPIO_Pin_6);                              // Set PD.6 low to turn off the LED in-built LED
        Delay_Ms(1000);                                                 // Delay for 1 second
    }
    
}
