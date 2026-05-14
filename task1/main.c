#include <stdio.h>
#include "gpio.h"

#define LED_PIN 5
#define BTN_PIN 3

int main(void)
{
    printf("Starting firmware application\n");

    gpio_init(LED_PIN, GPIO_OUTPUT);
    gpio_init(BTN_PIN, GPIO_INPUT);

    gpio_write(LED_PIN, 1);

    int button_state = gpio_read(BTN_PIN);
    printf("Button state: %d\n", button_state);

    gpio_write(LED_PIN, 0);

    printf("Firmware application finished\n");

    return 0;
}
