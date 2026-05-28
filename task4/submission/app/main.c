#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "../lib/gpio.h"
#include "../lib/uart.h"
#include "../lib/timer2.h"

// GPIO input output mode config macro.
#define INPUT_MODE 0
#define OUTPUT_MODE 1

void test_GPIO(void);
void test_UART(void);
void test_timer(void);
uint8_t result_flag;
uint8_t result_gpio_fail_flg;
uint8_t result_uart_fail_flg;
uint8_t result_timer_fail_flg;


int main ()
{
    Delay_Init();
    tim2_init();
    gpio_reset_all();
    uart_init((GPIO_Port)gpio_portD, GPIO_PIN_5, GPIO_PIN_6); // Initialize UART with PD5 (TX) and PD6 (RX)
    
    uint32_t booting_time_start = tim2_get_ms();
    uart_SendBuffer((uint8_t*)"=======================================\r\n", strlen("=======================================\r\n"));                                    // buffer to store the message to be sent over USART
    uart_SendBuffer((uint8_t*)"      Board Name: CH32V003\r\n", strlen("      Board Name: CH32V003\r\n"));
    uart_SendBuffer((uint8_t*)"     Welcome to RISC-V embedded systems\r\n", strlen("     Welcome to RISC-V embedded systems\r\n"));
    uart_SendBuffer((uint8_t*)"     Firmware Version: 1.2\r\n", strlen("     Firmware Version: 1.2\r\n"));
    uart_SendBuffer((uint8_t*)"=======================================\r\n", strlen("=======================================\r\n"));
    uart_SendBuffer((uint8_t*)"=================================================================\r\n", 
                    strlen("=================================================================\r\n"));
    uart_SendBuffer((uint8_t*)"     POWER ON SELF TEST INITIALIZING FOR GPIO, USART, TIMER \r\n", 
                    strlen("     POWER ON SELF TEST INITIALIZING FOR GPIO, USART, TIMER \r\n"));
    uart_SendBuffer((uint8_t*)"=================================================================\r\n", 
                    strlen("=================================================================\r\n"));


    uart_SendBuffer("\n================= PERIPHERAL TESTING ================\r\n",
                    strlen("\n================= PERIPHERAL TESTING ================\r\n"));
    result_flag = 0;
    uint8_t buffer[50];
    uint8_t i = 10;
    //==================================================
    char str_gpio_test[] = "================= GPIO TEST START =====================\r\n";
    char str_gpio_done[] = "================= GPIO TEST DONE ======================\r\n\r\n";
    uint32_t final_len1 = strlen(str_gpio_done);
    uint32_t len1 = strlen(str_gpio_test);
    uart_SendBuffer((uint8_t*)str_gpio_test,len1);
    result_gpio_fail_flg = 0;
    test_GPIO();
    uart_SendBuffer((uint8_t*)str_gpio_done,final_len1);
    Delay_Ms(500);
    //==================================================
    char str_uart_test[] = "================= UART TEST START =====================\r\n";
    char str_uart_done[] = "================= UART TEST DONE ======================\r\n\r\n";
    uint32_t final_len2 = strlen(str_uart_done);
    uint32_t len2 = strlen(str_uart_test);
    uart_SendBuffer((uint8_t*)str_uart_test,len2);
    result_uart_fail_flg = 0;
    test_UART();
    uart_SendBuffer((uint8_t*)str_uart_done,final_len2);
    Delay_Ms(500);
    //===================================================
    char str_timer_test[] = "================= TIMER TEST START =====================\r\n";
    char str_timer_done[] = "================= TIMER TEST DONE ======================\r\n\r\n";
    uint32_t final_len3 = strlen(str_timer_done);
    uint32_t len3 = strlen(str_timer_test);
    uart_SendBuffer((uint8_t*)str_timer_test,len3);
    result_timer_fail_flg = 0;
    test_timer();
    uart_SendBuffer((uint8_t*)str_timer_done,final_len3);
    Delay_Ms(500);
    //===================================================

    if(result_flag != 0 && result_gpio_fail_flg == 0 && result_uart_fail_flg == 0 && result_timer_fail_flg == 0 ){
    uart_SendBuffer("\n=================== ALL TEST PASS ================\r\n",
                    strlen("\n=================== ALL TEST PASS ================\r\n"));
    }
    else if(result_uart_fail_flg == 1)
    {
        uart_SendBuffer("\n=================== UART TEST FAIL ================\r\n",
                    strlen("\n=================== UART TEST FAIL ================\r\n"));
    }
    else if(result_gpio_fail_flg == 1)
    {
        uart_SendBuffer("\n=================== GPIO TEST FAIL ================\r\n",
                    strlen("\n=================== GPIO TEST FAIL ================\r\n"));
    }
    else if(result_timer_fail_flg == 1)
    {
        uart_SendBuffer("\n=================== TIMER TEST FAIL ================\r\n",
                    strlen("\n=================== TIMER TEST FAIL ================\r\n"));
    }
    
    while(i != 0)
    {
        sprintf((char*)buffer, "Booting in  %ld seconds\r\n", i--);    // Format the booting message with the current countdown value 
        uart_SendBuffer((uint8_t *)buffer, strlen((char*)buffer));      // send the message over USART
        Delay_Ms(1000); // delay for 1 second
    }
    uart_SendBuffer((uint8_t*)"Booting complete!\r\n", strlen("Booting complete!\r\n"));
    uart_SendBuffer((uint8_t*)"System is now running.\r\n", strlen("System is now running.\r\n"));
    
    uint32_t booting_time_end = tim2_get_ms();
    uint8_t time_buffer[50];
    snprintf(time_buffer,sizeof(time_buffer),"System booted in %ld seconds",((booting_time_end - booting_time_start)/1000));
    uart_SendBuffer((uint8_t*)time_buffer,strlen(time_buffer));

    while(1);
}

void test_GPIO(void)
{
    uint8_t pin_x = 0;          //this will go from 0-7 [enum variable]
    uint8_t portx = 0;          //this will go from 0-2 [0.gpio_PortA 1.gpio_PortC 2.gpio_PortD] Enum variable
    char bufferTx[32];
    const char* port_names[] = {"GPIOA", "GPIOC", "GPIOD"};
    //GPIOA only has 2 pins: PIN 1 and PIN 2

    for(portx = 0 ; portx <= 0 ; portx++)
    {

        for(pin_x = 1; pin_x <= 2; pin_x++)
        {
            gpio_init(portx,pin_x,OUTPUT_MODE);
            gpio_set_pin(portx,pin_x);
            if(gpio_read_pin(portx,pin_x))
            {
                result_flag = 1;
                snprintf((char*)bufferTx,sizeof(bufferTx),"Portx = %s ,pin = %d PASS \r\n",port_names[portx],pin_x);
            }
            else{
                result_gpio_fail_flg = 1;
                snprintf((char*)bufferTx,sizeof(bufferTx),"Portx = %s ,pin = %d FAIL \r\n",port_names[portx],pin_x);
            }
        }
        uart_SendBuffer((uint8_t*)bufferTx, sizeof(bufferTx));
        gpio_clear_pin(portx, pin_x);        
        memset((uint8_t*)bufferTx, 0, sizeof(bufferTx));
        Delay_Ms(500);
    }
    for(portx = 1; portx <= 2; portx++)
    {
        for(pin_x = 0; pin_x <= 7; pin_x++)
        {
            if(portx == 2 && (pin_x == 5 || pin_x == 6))                       // USART RX and TX PINS. 
            {
                snprintf(bufferTx, sizeof(bufferTx),"GPIOD pin %d USART Rx/Tx\r\n", pin_x);
                uart_SendBuffer(bufferTx, sizeof(bufferTx));
                continue;
            }
            gpio_init(portx,pin_x,OUTPUT_MODE);
            gpio_set_pin(portx,pin_x);
            if(gpio_read_pin(portx,pin_x))
            {
                result_flag = 1;
                snprintf((char*)bufferTx,sizeof(bufferTx),"Portx = %s ,pin = %d PASS \r\n",port_names[portx],pin_x);
            }
            else{
                result_gpio_fail_flg = 1;
                snprintf((char*)bufferTx,sizeof(bufferTx),"Portx = %s ,pin = %d FAIL \r\n",port_names[portx],pin_x);
            }
            uart_SendBuffer((uint8_t*)bufferTx, sizeof(bufferTx));
            gpio_clear_pin(portx, pin_x);        
            memset((uint8_t*)bufferTx, 0, sizeof(bufferTx));
            Delay_Ms(500);
        }
    }
    uart_SendBuffer((uint8_t*)"\r\n================= GPIO TEST PASS ====================\r\n",
                    strlen("\r\n================= GPIO TEST PASS ====================\r\n"));
    
}

void test_UART(void)
{
    const uint8_t* test_str = "UART_TEST_STRING";
    uint16_t    test_len  = strlen(test_str);
    uint8_t     bufferRx[100];
    memset(bufferRx, 0, sizeof(bufferRx));
    
    uart_flushRx();                            // first flush
    uint16_t t = tim2_get_ms();
    while(!tim2_elapsed(t, 5));                // 5ms  any noise/reflection clears
    uart_flushRx();                            // final flush 

    uint8_t* result = uart_sendReceive((const uint8_t*)test_str,bufferRx,test_len);
    uart_waitTxComplete();
    if(result == NULL)
    {
        uart_SendBuffer((uint8_t*)"\r\nUART FAIL - timeout\r\n",strlen("\r\nUART FAIL - timeout\r\n"));
        return;
    }
    if(strcmp((char*)bufferRx, test_str) == 0){
        result_flag = 1;
        uart_SendBuffer((uint8_t*)"\r\nUART                        PASS\r\n",
                        strlen("\r\nUART                        PASS\r\n"));
        uart_SendBuffer((uint8_t*)"\r\n================= UART TEST PASS ====================\r\n",
                        strlen("\r\n================= UART TEST PASS ====================\r\n"));
    }
    else
    {
        result_uart_fail_flg = 1;
        char fail_report[60];
        snprintf(fail_report, sizeof(fail_report), "\r\nUART FAIL got:[%s]\r\n", bufferRx);
        uart_SendBuffer((uint8_t*)fail_report, strlen(fail_report));
    }
}

void test_timer(void)
{
    uint16_t t1 = tim2_get_ms();
    Delay_Ms(1000);                  //it will give 1 second delay 
    uint16_t t2 = tim2_get_ms();

    char status_str[50];
    snprintf(status_str, sizeof(status_str), "TIM2 t1=%u t2=%u\r\n", t1, t2);
    uart_SendBuffer((uint8_t*)status_str, strlen(status_str));
    if((t2-t1) > 950 && (t2-t1) < 1050)   //this is for +- 50ms tolerance
    {
        result_flag = 1;
        uart_SendBuffer((uint8_t*)"\r\nTIMER                      PASS \r\n",
                        strlen("\r\nTIMER                      PASS \r\n"));
        uart_SendBuffer((uint8_t*)"\r\n================= TIMER TEST PASS ====================\r\n",
                        strlen("\r\n================= TIMER TEST PASS ====================\r\n"));
    }
    else{
        result_timer_fail_flg = 1;
        uart_SendBuffer((uint8_t*)"\nTimer TEST FAIL\r\n", strlen("\nTimer TEST FAIL\r\n"));
    }
}

