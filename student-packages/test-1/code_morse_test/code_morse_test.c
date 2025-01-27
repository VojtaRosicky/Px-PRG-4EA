#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h" 
#include "hardware/pwm.h" 
#include "hardware/gpio.h" 


#define POT_PIN 26          
#define ADC_CHAN 0 

#define LED_PIN 15        



// Timing definitions (in milliseconds)
#define DOT_DURATION 200
#define DASH_DURATION (DOT_DURATION * 3)
#define SYMBOL_SPACE (DOT_DURATION)
#define LETTER_SPACE (DOT_DURATION * 3)
#define WORD_SPACE (DOT_DURATION * 7)
#define MAX_INPUT 100

char znak;

// Morse code lookup table (DO NOT MODIFY)
const char *MORSE_CODE[] = {
    ".-",   // A
    "-...", // B
    "-.-.", // C
    "-..",  // D
    ".",    // E
    "..-.", // F
    "--.",  // G
    "....", // H
    "..",   // I
    ".---", // J
    "-.-",  // K
    ".-..", // L
    "--",   // M
    "-.",   // N
    "---",  // O
    ".--.", // P
    "--.-", // Q
    ".-.",  // R
    "...",  // S
    "-",    // T
    "..-",  // U
    "...-", // V
    ".--",  // W
    "-..-", // X
    "-.--", // Y
    "--.."  // Z
};

// Function declarations
void init_pwm(void);
void init_adc(void);
uint16_t read_brightness(void);
void set_led_brightness(uint16_t brightness);
void display_dot(void);
void display_dash(void);
void display_character(char c);
bool read_line(char *buffer, int max_length);
void clear_input_buffer(void);

uint16_t map_value(uint16_t value, uint16_t in_min, uint16_t in_max,
                   uint16_t out_min, uint16_t out_max) {
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


int main()
{
    stdio_init_all();
    sleep_ms(2000); // Wait for USB serial to initialize

    // TODO: Initialize hardware - implement init_pwm and init_adc functions
    init_pwm();
    init_adc();

    printf("Morse Code LED Controller\n");
    printf("Use potentiometer to control LED brightness\n\n");

    char input[MAX_INPUT];

    while (true)
    {
        printf("Enter text to convert to Morse code (max %d chars): ", MAX_INPUT - 1);

        if (read_line(input, MAX_INPUT))
        {
            printf("Converting: %s\n", input);

            for (int i = 0; input[i] != '\0'; i++)
            {
                display_character(input[i]);
            }

            printf("\nConversion complete!\n\n");
        }

        clear_input_buffer(); // Clear any remaining input
        sleep_ms(100);
    }

    return 0;
}


void init_pwm(void)
{
    gpio_init(LED_PIN);
    gpio_set_function(LED_PIN, GPIO_FUNC_PWM);

    uint slice_num = pwm_gpio_to_slice_num(LED_PIN);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 125); 
    pwm_config_set_wrap(&config, 1000); 
    pwm_init(slice_num, &config, true);

    pwm_set_gpio_level(LED_PIN, 0);
}


void init_adc(void)
{
    adc_init();
    adc_gpio_init(POT_PIN);
    adc_select_input(ADC_CHAN);
}


uint16_t read_brightness(void)
{
return map_value(adc_read(), 0, 4095, 0, 1000);
}


void set_led_brightness(uint16_t brightness)
{
    pwm_set_gpio_level(LED_PIN, brightness);
}


void display_dot(void)
{
   set_led_brightness(read_brightness());
   sleep_ms(DOT_DURATION);
   set_led_brightness(0);
   sleep_ms(SYMBOL_SPACE);
}


void display_dash(void)
{
   set_led_brightness(read_brightness());
   sleep_ms(DASH_DURATION);
   set_led_brightness(0);
   sleep_ms(SYMBOL_SPACE);
}


void display_character(char c)
{
    if (c == ' '){
        sleep_ms(WORD_SPACE);
        return;
    }

    if (c >= 'a' && c <= 'z'){
        c -= 'a' - 'A';
    }

    if (c >= 'A' && c <= 'Z'){
        char *pattern = MORSE_CODE[c - 'A'];
        for (int i = 0; pattern[i] != '\0'; i++)
        {
            switch (pattern[i])
            {
                case '.':
                    display_dot();
                    break;

                case '-':
                    display_dash();
                    break;
                
                default:
                    break;
            }
        }
        sleep_ms(LETTER_SPACE);
    }

 

    char *pattern = MORSE_CODE[c - 'A'];
    printf("%s ", pattern);
}


void clear_input_buffer(void)
{
    while (getchar_timeout_us(0) != PICO_ERROR_TIMEOUT)
    {
        clear_input_buffer();
    }
}


bool read_line(char *buffer, int max_length)
{
    int index = 0;
    int c;

    while (index < max_length - 1)
    {
        c = getchar_timeout_us(50000);

        if (c == PICO_ERROR_TIMEOUT)
        {
            if (index == 0)
                continue;
            break;
        }

        if (c == '\r' || c == '\n')
        {
            if (index > 0)
                break;
            continue;
        }

        putchar(c);
        buffer[index++] = (char)c;
    }

    buffer[index] = '\0';
    putchar('\n');
    return index > 0;
}