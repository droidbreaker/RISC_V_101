#include "../lib/gpio.h"
#include "../lib/uart.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>

int main ()
{
    
    Delay_Init();
    gpio_init((GPIO_Port)gpio_portD, GPIO_PIN_2,1); //Initialize PD2 as output, 1 for output mode.
    gpio_init((GPIO_Port)gpio_portA,GPIO_PIN_2,0); //Initialize PA2 as input, 0 for input mode.
    uart_init((GPIO_Port)gpio_portD, GPIO_PIN_5, GPIO_PIN_6); // Initialize UART with PD5 (TX) and PD6 (RX)
    
    uint8_t bufferTx[] = "Hello, World!\r\n";
    uint8_t bufferRx[100]; // Buffer to store received data.
    uint8_t buffer[50];
    uint8_t receivedByte = 0;// receiving a single byte
    uint8_t pin_status = 0;   //pin status for input pin if its high or low.
    uint8_t ch = 0;          // choice for the 3 modes of operation.
    uint8_t i = 10;           // count down counter for booting sequence.
    uint8_t flag = 0;        // loop controlling flag
    uart_SendBuffer(bufferTx, strlen((char*)bufferTx)); // Send the "Hello, World!" message over UART
    gpio_clear_pin((GPIO_Port)gpio_portD, GPIO_PIN_2); //Set PD2 low
    uint8_t message[] = "\nPress 1: to turn ON/OFF LED via command, 2: check LED status 3: send and recieve echo message 4: Exit\r\n";
    uint8_t usage_msg[] = "\n '1' to Turn ON LED '0' to Turn OFF LED\r\n";
    uart_SendBuffer((uint8_t*)"=======================================\r\n", strlen("=======================================\r\n"));                                    // buffer to store the message to be sent over USART
    uart_SendBuffer((uint8_t*)"      Board Name: CH32V003\r\n", strlen("      Board Name: CH32V003\r\n"));
    uart_SendBuffer((uint8_t*)"     Welcome to RISC-V embedded systems\r\n", strlen("     Welcome to RISC-V embedded systems\r\n"));
    uart_SendBuffer((uint8_t*)"     Firmware Version: 1.1\r\n", strlen("     Firmware Version: 1.1\r\n"));
    uart_SendBuffer((uint8_t*)"=======================================\r\n", strlen("=======================================\r\n"));
    while(i != 0)
    {
        sprintf((char*)buffer, "Booting in  %ld seconds\r\n", i--);    // Format the booting message with the current countdown value 
        uart_SendBuffer((uint8_t *)buffer, strlen((char*)buffer));      // send the message over USART
        Delay_Ms(1000); // delay for 1 second
    }
    uart_SendBuffer((uint8_t*)"Booting complete!\r\n", strlen("Booting complete!\r\n"));
    uart_SendBuffer((uint8_t*)"System is now running.\r\n", strlen("System is now running.\r\n"));
    uart_SendBuffer((uint8_t*)"=================================================\r\n", strlen("=================================================\r\n"));
    uart_SendBuffer((uint8_t*)"     Welcome to UART application with driver\r\n", strlen("     Welcome to UART application with driver\r\n"));
    uart_SendBuffer((uint8_t*)"=================================================\r\n", strlen("=================================================\r\n"));
    while(1)
    {  
        if(flag == 0)
        {
        flag = 1;
        uart_SendBuffer(message,sizeof(message));
        ch = uart_receiveByte();
        if(ch == '1')
        {
            uart_SendBuffer(usage_msg, sizeof(usage_msg));
            receivedByte = uart_receiveByte();
            if(receivedByte == '1')
            {
                gpio_set_pin((GPIO_Port)gpio_portD, GPIO_PIN_2); //set PD2 HIGH
                uart_SendBuffer((uint8_t*)"\nLED is ON\r\n",strlen("\nLED is ON\r\n"));
                flag = 0;
            }
            else if(receivedByte == '0')
            {
                gpio_clear_pin((GPIO_Port)gpio_portD, GPIO_PIN_2); //Set PD2 low
                uart_SendBuffer((uint8_t*)"\nLED is OFF\r\n",strlen("\nLED is OFF\r\n"));
                flag = 0;
            }
        }
        if(ch == '2')
        {
            uart_SendBuffer((uint8_t*)"\ncheck pin status:\r\n",strlen("\ncheck pin status:\r\n"));
            pin_status = gpio_read_pin((GPIO_Port)gpio_portA,GPIO_PIN_2);
            if(pin_status == 1)
            {
                uart_SendBuffer((uint8_t*)"Pin status HIGH\r\n",strlen("Pin status HIGH\r\n"));
                flag = 0;
            }
            else if(pin_status == 0)
            {
                uart_SendBuffer((uint8_t*)"Pin status LOW\r\n",strlen("Pin status LOW\r\n"));
                flag = 0;
            }
        }
        if(ch == '3')
        {
            uart_SendBuffer((uint8_t*)"\n=====Echo message Demo=====\r\n",strlen("\n=====Echo message Demo=====\r\n"));
            uart_SendBuffer((uint8_t*)"Enter Message: ",strlen("Enter Message: "));
            uart_receiveData(bufferRx,sizeof(bufferRx));
            uart_SendBuffer((uint8_t*)"\r\n You Typed: ",strlen("\r\n You Typed: "));
            uart_SendBuffer(bufferRx,strlen((char*)bufferRx));
            uart_SendBuffer((uint8_t*)"\r\n\r\n",4);
            memset(bufferRx, 0, sizeof(bufferRx));
            flag = 0;
        }
        if(ch == '4')
        {
            uart_SendBuffer((uint8_t*)"\nApplication demo complete:\r\n",strlen("\nApplication demo complete:\r\n"));
            break;
        }
    }
    }
while(1);

return 0;
}
