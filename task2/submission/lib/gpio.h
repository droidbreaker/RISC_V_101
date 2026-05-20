#ifndef GPIO_H
#define GPIO_H

#include <ch32v00x.h>
#include <ch32v00x_gpio.h>
#include <ch32v00x_rcc.h>
#include <debug.h>

void GPIO_Config(void);                                                           // Function to configure GPIO pins for output
void GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_InitStruct);           // Function to initialize GPIO pins with specified parameters
void GPIO_SetBits(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);                        // Function to set specified GPIO pins high
void GPIO_ResetBits(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);                      // Function to set specified GPIO pins low

#endif // GPIO_H
