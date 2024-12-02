// Pico SDK libraries
#include "pico/stdlib.h"
#include "pico/time.h"

// Standard libraries
#include <stdio.h>
#include <stdbool.h>

#define LED_PIN 6
#define BUTTON_PIN_1 19
#define BUTTON_PIN_2 20

uint32_t counter = 0;

void handler(uint gpio, uint32_t events);
uint32_t millis();

uint32_t lastIrgTime = 0;

int main()
{
    // Initialize all standard I/O
    stdio_init_all();
    // Wait for serial monitor to reconnect
    sleep_ms(2000);
    printf("Starting...\n");

    // Initialize GPIO pins
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    gpio_init(BUTTON_PIN_1);
    gpio_pull_up(BUTTON_PIN_1);
    gpio_set_dir(BUTTON_PIN_1, GPIO_IN);

    gpio_init(BUTTON_PIN_2);
    gpio_pull_up(BUTTON_PIN_2);
    gpio_set_dir(BUTTON_PIN_2, GPIO_IN);

    // Enable interrupts for button pin
    gpio_set_irq_enabled_with_callback(BUTTON_PIN_1, GPIO_IRQ_EDGE_RISE, true, &handler);
    gpio_set_irq_enabled_with_callback(BUTTON_PIN_2, GPIO_IRQ_EDGE_RISE, true, &handler);

    // Infinite loop
    while (1)
    {
        printf("\rRun...\n");
        printf("Hodnota counter:%d\n", counter);
        sleep_ms(500);
    
    }
    return 0;
}

void handler(uint gpio, uint32_t events)
{
    if ((millis() - lastIrgTime) >= 200) {
        gpio_put(LED_PIN, !gpio_get(LED_PIN));
        if(gpio == BUTTON_PIN_1){
            counter++;
        }else if(gpio == BUTTON_PIN_2){
            counter--;
        }
        lastIrgTime = millis();
    }
}

uint32_t millis() {
    return to_ms_since_boot(get_absolute_time());
}