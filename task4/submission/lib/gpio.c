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



void gpio_config_output(GPIO_Port port,GPIO_PIN pin)       // Helper function to configure the specified GPIO pin as output
{
    uint32_t shift = pin * 4;               // Each pin has 4 bits in the configuration register (2 for MODE and 2 for CNF)
    gpio(port)->gpio_cfglr &= ~(0xF << shift);    // Clear the 4 bits corresponding to the pin

    gpio(port)->gpio_cfglr |= (0x3 << shift);       // Set MODE to 11b (50MHz)
    gpio(port)->gpio_cfglr |= (0x0 << (shift + 2)); // Set CNF to 00 (Push-pull)  
    gpio(port)->gpio_bcr = (0 << pin);              // Clear the bit corresponding to the pin to 0, which sets the pin low.
}


void gpio_config_input(GPIO_Port port,GPIO_PIN pin)
{
    uint32_t shift = pin * 4;                 // Each pin has 4 bits in the configuration register (2 for MODE and 2 for CNF)
    gpio(port)->gpio_cfglr &= ~(0xF << shift);      // Clear the 4 bits corresponding to pin
    
    gpio(port)->gpio_cfglr |= (0x8 << shift);     // MODE = 00, CNF = 10  -> 0x8 1000b = 0x8 
    gpio(port)->gpio_outdr |= (1 << pin);            // pin as input pull-up mode. 
}


static void gpio_reset(GPIO_Port port)            // Helper function to reset the specified GPIO port
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

void gpio_reset_all(void)
{
    gpio_reset(gpio_portA);
    gpio_reset(gpio_portC);
    gpio_reset(gpio_portD);
}

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


void gpio_set_pin(GPIO_Port port, uint8_t pin)    // Function to set the specified GPIO pin
{
    gpio(port)->gpio_bshr = (1 << pin);       // Set the bit corresponding to the pin to 1, which sets the pin high
}


void gpio_clear_pin(GPIO_Port port, uint8_t pin)   // Function to clear the specified GPIO pin
{
    gpio(port)->gpio_bcr = (1 << pin);
}

void gpio_toggle_pin(GPIO_Port port, uint8_t pin)   // Function to toggle the specified GPIO pin
{
    gpio(port)->gpio_bshr ^= (1 << pin);
}

uint8_t gpio_read_pin(GPIO_Port port, uint8_t pin)   // Function to read the state of the specified GPIO pin
{
    return (gpio(port)->gpio_indr >> pin) & 0x01;                   // Return the state of the pin (0 or 1)
}