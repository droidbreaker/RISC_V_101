#include "../lib/gpio.h"
     

void GPIO_Config(void)
{
GPIO_InitTypeDef GPIO_InitStructure = {0}; //structure variable used for the GPIO configuration

RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE); // to Enable the clock for Port D
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // to Enable the clock for Port A

GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; // Defines which Pin to configure
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // Defines Output Type
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // Defines speed
GPIO_Init(GPIOD, &GPIO_InitStructure);

}