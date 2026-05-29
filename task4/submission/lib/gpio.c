#include "../lib/gpio.h"


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
            return NULL;
    }
}

static inline Typedef_Gpio_Port* gpio(GPIO_Port port)
{
    return gpio_get_port(port);
}

static void gpio_enable_clock(GPIO_Port port)             // Helper function to enable the clock for the specified GPIO port
{
    switch(port)
    {
        case gpio_portA:
            RCC_APB2PCENR_REG->IOPAEN = 1;
            break;

        case gpio_portC:
            RCC_APB2PCENR_REG->IOPCEN = 1;
            break;

        case gpio_portD:
            RCC_APB2PCENR_REG->IOPDEN = 1;
            break;
    }
}


/*
 * @brief : This function configuring GPIOx PINx as OUTPUT Mode. 
 * By configuring MODE and CNF using "@CFGLR" register, and make the pin as output Pull-up mode. 
 * and clear that pin before the writting on it by clearing "@BCR" register corresponding pin bit.
 * @param : GPIO port : [GPIO A, GPIO C, GPIO D] , GPIO pin : [0 -7] pins.
 * @return : nothing to return.
 *
 */
void gpio_config_output(GPIO_Port port,GPIO_PIN pin)       // Helper function to configure the specified GPIO pin as output
{
    uint32_t shift = pin * 4;                              // Each pin has 4 bits in the configuration register (2 for MODE and 2 for CNF)
    gpio(port)->gpio_cfglr &= ~(0xF << shift);             // Clear the 4 bits corresponding to the pin

    gpio(port)->gpio_cfglr |= (0x3 << shift);              // Set MODE to 11b (50MHz)
    gpio(port)->gpio_cfglr |= (0x0 << (shift + 2));        // Set CNF to 00 (Push-pull)  
    gpio(port)->gpio_bcr = (0 << pin);                     // Clear the bit corresponding to the pin to 0, which sets the pin low.
}

/*
 * @brief : this function configuring GPIOx PINx as INPUT mode. By configuring MODE and CNF using "@CFGLR" register, 
 * and make the pin as input-pull up in "@OUTDR" register ,so that that pin acts as input mode.
 * @param : GPIO port : [GPIO A, GPIO C, GPIO D] , GPIO pin : [0 -7] pins.
 * @return : nothing to return.
 *
 */
void gpio_config_input(GPIO_Port port,GPIO_PIN pin)
{
    uint32_t shift = pin * 4;                              // Each pin has 4 bits in the configuration register (2 for MODE and 2 for CNF)
    gpio(port)->gpio_cfglr &= ~(0xF << shift);             // Clear the 4 bits corresponding to pin
    
    gpio(port)->gpio_cfglr |= (0x8 << shift);              // MODE = 00, CNF = 10  -> 0x8 1000b = 0x8 
    gpio(port)->gpio_outdr |= (1 << pin);                  // pin as input pull-up mode. 
}


static void gpio_reset(GPIO_Port port)                     // Helper function to reset the specified GPIO port
{
    switch(port)
    {
        case gpio_portA:
            RCC_APB2PRSTR_REG->IOPARST = 1;
            RCC_APB2PRSTR_REG->IOPARST = 0;
            break;

        case gpio_portC:
            RCC_APB2PRSTR_REG->IOPCRST = 1;
            RCC_APB2PRSTR_REG->IOPCRST = 0;
            break;

        case gpio_portD:
            RCC_APB2PRSTR_REG->IOPDRST = 1;
            RCC_APB2PRSTR_REG->IOPDRST = 0;
            break;
    }
}

/*
 * @brief : this function resets all the GPIO ports , so that its registers were fully cleaned before any application usage.
 * by using "@RCC_APB2PRSTR_REG" for the corresponding bit for PORTx. by resetting it to clear position.
 * @param : void there is no need for argument for this function as it resets all GPIO ports.
 * @return : nothing to return.
 *
 */

void gpio_reset_all(void)
{
    gpio_reset(gpio_portA);
    gpio_reset(gpio_portC);
    gpio_reset(gpio_portD);
}

/* 
 * @brief this function initializes GPIO Port and Pins for the requirement of input or output mode. 
 * Enabling the clock for required PORTx. "@RCC_APB2PCENR_REG" to turn on corresponding bit for PORTx.
 * @param port : [port A, port C , port D] ; pin : [0-7] pin ; in_out_mode : [1 (output) , 0 (input)].
 * @return nothing to return.
 */

void gpio_init(GPIO_Port port,uint8_t pin,uint8_t in_out_mode)       // Main function to initialize the specified GPIO port and pin as output
{
    gpio_enable_clock(port);  
    if(in_out_mode == 1){
    gpio_config_output(port, pin);
    }
    else{
        gpio_config_input(port,pin);
    }
}

/*
 * @brief : this function will set the GPIOx PINx to HIGH 
 * by setting the "@BSHR" registers corresponding bit to '1'.
 * @param : GPIO port : [GPIO A, GPIO C, GPIO D] , GPIO pin : [0 -7] pins.
 * @return : nothing to return.
 *
 */
void gpio_set_pin(GPIO_Port port, uint8_t pin)                     // Function to set the specified GPIO pin
{
    gpio(port)->gpio_bshr = (1 << pin);                            // Set the bit corresponding to the pin to 1, which sets the pin high
}

/*
 * @brief : This function will clear the GPIOx PINx to RESET, 
 * by setting "@BCR" register of that corresponding pin bit as 1.
 * @param : GPIO port : [GPIO A, GPIO C, GPIO D] , GPIO pin : [0 -7] pins.
 * @return : nothing to return.
 * 
 */
void gpio_clear_pin(GPIO_Port port, uint8_t pin)                   // Function to clear the specified GPIO pin
{
    gpio(port)->gpio_bcr = (1 << pin);
}

/*
 * @brief: This function will toggle the LED on that GPIOx and PINx, 
 * by "@BSHR" corresponding bit to `XOR` with '1'. 
 * @param : GPIO port : [GPIO A, GPIO C, GPIO D] , GPIO pin : [0 -7] pins.
 * @return : nothing to return.
 * 
 */
void gpio_toggle_pin(GPIO_Port port, uint8_t pin)                  // Function to toggle the specified GPIO pin
{
    gpio(port)->gpio_bshr ^= (1 << pin);
}

/*
 * @brief: This function will read the pin status for digital input of '1',
 * used to check the LED status if its `ON` or `OFF`.
 * also, by reading the "@INDR" register corresponding Pin bit. 
 * and checking by '&' with 1 so that we can get the proper reading. 
 * @param :  GPIO port : [GPIO A, GPIO C, GPIO D] , GPIO pin : [0 -7] pins.
 * @return : returning 'HIGH' and 'LOW' ['1' , '0'] value.
 * 
 */
uint8_t gpio_read_pin(GPIO_Port port, uint8_t pin)                 // Function to read the state of the specified GPIO pin
{
    return (gpio(port)->gpio_indr >> pin) & 0x01;                  // Return the state of the pin (0 or 1)
}