# ALL API REFERENCES : 
 * USED GPIO, UART, TIMER DRIVER API's in the APPLICATION.
---

## GPIO DRIVER API : 

```c
/**
 * @brief this function initializes GPIO Port and Pins for the requirement of input or output mode. 
 * Enabling the clock for required PORTx. "@RCC_APB2PCENR_REG" to turn on corresponding bit for PORTx.
 * @param port : [port A, port C , port D] ; pin : [0-7] pin ; in_out_mode : [1 (output) , 0 (input)].
 * @return nothing to return.
 */
void gpio_init(GPIO_Port port, uint8_t pin, uint8_t in_out_mode);
```

```c
/**
 * @brief : this function resets all the GPIO ports , so that its registers were fully cleaned before any application usage.
 * by using "@RCC_APB2PRSTR_REG" for the corresponding bit for PORTx. by resetting it to clear position.
 * @param : void there is no need for argument for this function as it resets all GPIO ports.
 * @return : nothing to return.
 *
 */
void gpio_reset_all(void);
```

```c
/**
 * @brief : This function configuring GPIOx PINx as OUTPUT Mode. By configuring MODE and CNF using "@CFGLR" register, and make the pin as output Pull-up mode. and clear that pin before the writting on it by clearing "@BCR" register corresponding pin bit.
 * @param : GPIO port : [GPIO A, GPIO C, GPIO D] , GPIO pin : [0 -7] pins.
 * @return : nothing to return.
 *
 */
void gpio_config_output(GPIO_Port port, GPIO_PIN pin);
```

```c
/**
 * @brief : this function configuring GPIOx PINx as INPUT mode. By configuring MODE and CNF using "@CFGLR" register, and make the pin as input-pull up in "@OUTDR" register ,so that that pin acts as input mode.
 * @param : GPIO port : [GPIO A, GPIO C, GPIO D] , GPIO pin : [0 -7] pins.
 * @return : nothing to return.
 *
 */
void gpio_config_input(GPIO_Port port,GPIO_PIN pin);
```

```c
/**
 * @brief : this function will set the GPIOx PINx to HIGH by setting the "@BSHR" registers corresponding bit to '1'.
 * @param : GPIO port : [GPIO A, GPIO C, GPIO D] , GPIO pin : [0 -7] pins.
 * @return : nothing to return.
 *
 */
void gpio_set_pin(GPIO_Port port, uint8_t pin);
```

```c
/**
 * @brief : This function will clear the GPIOx PINx to RESET, 
 * by setting "@BCR" register of that corresponding pin bit as 1.
 * @param : GPIO port : [GPIO A, GPIO C, GPIO D] , GPIO pin : [0 -7] pins.
 * @return : nothing to return.
 * 
 */
void gpio_clear_pin(GPIO_Port port, uint8_t pin);
```

```c
/**
 * @brief: This function will toggle the LED on that GPIOx and PINx, 
 * by "@BSHR" corresponding bit to `XOR` with '1'. 
 * @param : GPIO port : [GPIO A, GPIO C, GPIO D] , GPIO pin : [0 -7] pins.
 * @return : nothing to return.
 * 
 */
void gpio_toggle_pin(GPIO_Port port, uint8_t pin); 
```

```c
/**
 * @brief: This function will read the pin status for digital input of '1',
 * used to check the LED status if its `ON` or `OFF`.
 * also, by reading the "@INDR" register corresponding Pin bit. 
 * and checking by '&' with 1 so that we can get the proper reading. 
 * @param :  GPIO port : [GPIO A, GPIO C, GPIO D] , GPIO pin : [0 -7] pins.
 * @return : returning 'HIGH' and 'LOW' ['1' , '0'] value.
 * 
 */
uint8_t gpio_read_pin(GPIO_Port port, uint8_t pin)
```


---

## UART DRIVER API :

```c
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
void uart_init(GPIO_Port port, GPIO_PIN tx_pin, GPIO_PIN rx_pin);
```

```c
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
void uart_SendBuffer(uint8_t *buffer, uint16_t length);
```

```c
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
uint8_t* uart_sendReceive(const uint8_t* tx_buf,uint8_t* rx_buf,uint16_t  length);
```

```c
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
uint8_t* uart_receiveData_from_User_input(uint8_t *buffer, uint16_t maxLength);
```

```c
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
uint8_t uart_receiveByte();
```

---

## TIMER DRIVER API :

```c
/**
 * @brief : Initializes TIM2 as a 1ms timebase.
 *
 * RCC_APB1PCENR.TIM2EN = 1   — enables TIM2 peripheral clock.
 * TIM2->CTLR1 = 0            — stops the timer before configuration.
 * TIM2->PSC   = 24000 - 1    — divides 24MHz APB1 clock → 1kHz (1 tick = 1ms).
 * TIM2->ATRLR = 0xFFFF       — sets counter ceiling to 65535ms (~65.5s).
 * TIM2->SWEVGR |= UG (bit0)  — forces PSC/ATRLR shadow register update.
 * TIM2->CNT   = 0            — clears counter to start from known state.
 * TIM2->INTFR = 0            — clears UIF flag raised by UG event.
 * TIM2->CTLR1 |= CEN (bit0)  — starts the counter.
 *
 * @param : Nothing as parameter/ function arguments.
 * @return : Nothing to return.
 */
void tim2_init(void);
```

```c
/**
 * @brief : Resets the TIM2 counter to zero.
 * TIM2->CNT = 0  — directly overwrites the Counter Register without
 *                  stopping the timer or altering any other register.
 *
 * @param : No parameter argument
 * @return : Nothing to return.
 */
void tim2_CNT_reset(void);
```

```c
/**
 * @brief : Returns the current TIM2 counter value in milliseconds.
 *
 * TIM2->CNT (read) — raw 16-bit counter value equals elapsed ms
 *                    directly, since PSC is set for 1 tick per ms.
 *
 * @param : None
 * @return : Current TIM2->CNT value (0 – 65535 ms).
 */
uint16_t tim2_get_ms(void);
```

```c
/** 
 * @brief : Checks whether a timeout has elapsed since a given start time.
 * Only one counter rollover is handled correctly (max 65535ms).
 * TIM2->CNT (read via tim2_get_ms()) — sampled into 'now'.
 * elapsed = (uint16_t)(now - start_ms) — unsigned subtraction handles
 *           a single CNT rollover (0xFFFF → 0x0000) naturally.
 * Returns 1 if elapsed >= timeout_ms, otherwise 0.
 *
 * @param  "@start_ms"    TIM2->CNT value captured at interval start.
 *  ,@"timeout_ms"  Desired timeout in milliseconds (1 – 65535).
 *
 * @return : 1  Timeout reached or exceeded.
 * @return : 0  Timeout not yet elapsed.
 *   
 */
uint8_t tim2_elapsed(uint16_t start_ms, uint16_t timeout_ms);
```