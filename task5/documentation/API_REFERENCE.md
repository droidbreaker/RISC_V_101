# ALL API REFERENCES : 

---

## GPIO DRIVER API : 

```c
/* 
 * @brief this function initializes GPIO Port and Pins for the requirement of input or output mode. 
 * Enabling the clock for required PORTx. "@RCC_APB2PCENR_REG" to turn on corresponding bit for PORTx.
 * @param port : [port A, port C , port D] ; pin : [0-7] pin ; in_out_mode : [1 (output) , 0 (input)].
 * @return nothing to return.
 */
void gpio_init(GPIO_Port port, uint8_t pin, uint8_t in_out_mode);
```

```c
/*
 * @brief : this function resets all the GPIO ports , so that its registers were fully cleaned before any application usage.
 * by using "@RCC_APB2PRSTR_REG" for the corresponding bit for PORTx. by resetting it to clear position.
 * @param : void there is no need for argument for this function as it resets all GPIO ports.
 * @return : nothing to return.
 *
 */
void gpio_reset_all(void);
```

```c
/*
 * @brief : This function configuring GPIOx PINx as OUTPUT Mode. By configuring MODE and CNF using "@CFGLR" register, and make the pin as output Pull-up mode. and clear that pin before the writting on it by clearing "@BCR" register corresponding pin bit.
 * @param : GPIO port : [GPIO A, GPIO C, GPIO D] , GPIO pin : [0 -7] pins.
 * @return : nothing to return.
 *
 */
void gpio_config_output(GPIO_Port port, GPIO_PIN pin);
```

```c
/*
 * @brief : this function configuring GPIOx PINx as INPUT mode. By configuring MODE and CNF using "@CFGLR" register, and make the pin as input-pull up in "@OUTDR" register ,so that that pin acts as input mode.
 * @param : GPIO port : [GPIO A, GPIO C, GPIO D] , GPIO pin : [0 -7] pins.
 * @return : nothing to return.
 *
 */
void gpio_config_input(GPIO_Port port,GPIO_PIN pin);
```

```c
/*
 * @brief : this function will set the GPIOx PINx to HIGH by setting the "@BSHR" registers corresponding bit to '1'.
 * @param : GPIO port : [GPIO A, GPIO C, GPIO D] , GPIO pin : [0 -7] pins.
 * @return : nothing to return.
 *
 */
void gpio_set_pin(GPIO_Port port, uint8_t pin);
```

```c
/*
 * @brief : This function will clear the GPIOx PINx to RESET, 
 * by setting "@BCR" register of that corresponding pin bit as 1.
 * @param : GPIO port : [GPIO A, GPIO C, GPIO D] , GPIO pin : [0 -7] pins.
 * @return : nothing to return.
 * 
 */
void gpio_clear_pin(GPIO_Port port, uint8_t pin);
```

```c
/*
 * @brief: This function will toggle the LED on that GPIOx and PINx, 
 * by "@BSHR" corresponding bit to `XOR` with '1'. 
 * @param : GPIO port : [GPIO A, GPIO C, GPIO D] , GPIO pin : [0 -7] pins.
 * @return : nothing to return.
 * 
 */
void gpio_toggle_pin(GPIO_Port port, uint8_t pin); 
```

```c
/*
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



---

## TIMER DRIVER API :


