#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"

#define LED_PIN 6
#define POT_PIN 26

// Funkce pro mapování hodnoty
uint16_t map_value(uint16_t value, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max) {
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int main() {
    stdio_init_all();
    sleep_ms(2000);
    printf("Starting...\n");

    // Inicializace ADC
    adc_init();
    adc_gpio_init(POT_PIN);
    adc_select_input(0);  // Používáme kanál 0

    // Nastavení PWM pro LED
    gpio_set_function(LED_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(LED_PIN);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 125);  // Nastavení děličky pro PWM signál
    pwm_config_set_wrap(&config, 4095);  // Maximální hodnota pro PWM (12 bitů)
    pwm_init(slice_num, &config, true);

    while (true) {
        // Čtení hodnoty z potenciometru
        uint16_t result = adc_read();  // Hodnota od 0 do 4095

        // Mapování hodnoty ADC na PWM
        uint16_t pwm_value = map_value(result, 0, 4095, 0, 4095);

        // Nastavení úrovně PWM
        pwm_set_gpio_level(LED_PIN, pwm_value);

        // Krátké zpoždění, aby program nešel příliš rychle
        sleep_ms(10);
    }
}
