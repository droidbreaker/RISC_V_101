#include <debug.h>


#ifndef GPIO_H
#define GPIO_H

/*
 * GPIO Configuration Structure
 * This structure represents the configuration of GPIO pins for ports A, C, and D.
 * Each pin can be configured with a specific mode and configuration (CNF) using the GPIOx_CFGLR structure.
 * The GPIOx_OUTDR structure allows for reading and writing the output data register for each pin.
 * The GPIOx_BSHR structure provides bit set/reset functionality for the GPIO pins.
 * The RCC_APB2PCENR and RCC_APB2PRSTR structures are used to enable and reset the GPIO ports in the RCC (Reset and Clock Control) peripheral.
 * The gpio_init function initializes the GPIO ports, while the gpio_set_pin, gpio_clear_pin, gpio_toggle_pin, and gpio_read_pin functions provide basic operations for manipulating the GPIO pins.
 */

#define RCC_APB2PCENR_REG ((RCC_APB2PCENR*)0x40021018)   // Base address for APB2PCENR register
#define RCC_APB2PRSTR_REG ((RCC_APB2PRSTR*)0x4002100C)   // Base address for APB2PRSTR register

#define gpioA ((Typedef_Gpio_Port*)0x40010800)          // Base address for GPIO port A
#define gpioC ((Typedef_Gpio_Port*)0x40011000)          // Base address for GPIO port C
#define gpioD ((Typedef_Gpio_Port*)0x40011400)          // Base address for GPIO port D

typedef struct{
    uint32_t volatile gpio_cfglr;    // Configuration register for GPIO pins (MODE and CNF)
    uint32_t volatile reserved;
    uint32_t volatile gpio_indr;     // Input data register for GPIO pins
    uint32_t volatile gpio_outdr;    // Output data register for GPIO pins
    uint32_t volatile gpio_bshr;     // Bit set/reset register for GPIO pins
    uint32_t volatile gpio_bcr;      // Bit reset register for GPIO pins
    uint32_t volatile gpio_lckr;     // Additional registers can be added here if needed
}Typedef_Gpio_Port;  

typedef enum {
    gpio_portA,
    gpio_portC,
    gpio_portD
}GPIO_Port;

typedef enum
{
    GPIO_PIN_0,
    GPIO_PIN_1,
    GPIO_PIN_2,
    GPIO_PIN_3,
    GPIO_PIN_4,
    GPIO_PIN_5,
    GPIO_PIN_6,
    GPIO_PIN_7

}GPIO_PIN;

typedef volatile struct{
    uint32_t AFIOEN:1;
    uint32_t reserved:1;
    uint32_t IOPAEN:1;                 //Port A clock enable
    uint32_t reserved2:1;
    uint32_t IOPCEN:1;                 //Port C clock enable
    uint32_t IOPDEN:1;                 //Port D clock enable
    uint32_t reserved3:3;
    uint32_t ADC1EN:1; 
    uint32_t reserved4:1;
    uint32_t TIM1EN:1;
    uint32_t SPI1EN:1;
    uint32_t reserved5:1;
    uint32_t USART1EN:1;                //USART1 clock enable
    uint32_t reserved6:17;
}RCC_APB2PCENR;

typedef volatile struct{
    uint32_t AFIORST:1;
    uint32_t reserved:1;
    uint32_t IOPARST:1;                //Port A reset
    uint32_t reserved2:1;
    uint32_t IOPCRST:1;                 //Port C reset
    uint32_t IOPDRST:1;                 //Port D reset
    uint32_t reserved3:3;
    uint32_t ADC1RST:1; 
    uint32_t reserved4:1;
    uint32_t TIM1RST:1;
    uint32_t SPI1RST:1;
    uint32_t reserved5:1;
    uint32_t USART1RST:1;                //USART1 reset
    uint32_t reserved6:17;
}RCC_APB2PRSTR;

void gpio_init(GPIO_Port port, uint8_t pin, uint8_t in_out_mode);
void gpio_config_output(GPIO_Port port, GPIO_PIN pin);
void gpio_set_pin(GPIO_Port port, uint8_t pin);
void gpio_clear_pin(GPIO_Port port, uint8_t pin);
void gpio_toggle_pin(GPIO_Port port, uint8_t pin);
uint8_t gpio_read_pin(GPIO_Port port, uint8_t pin);
void gpio_config_input(GPIO_Port port,GPIO_PIN pin);
void gpio_reset_all(void);


#endif /* GPIO_H */