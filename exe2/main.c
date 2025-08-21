#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#ifndef GPIO_IRQ_EDGE_FALL
#define GPIO_IRQ_EDGE_FALL (0x4u)
#endif
#ifndef GPIO_IRQ_EDGE_RISE
#define GPIO_IRQ_EDGE_RISE (0x8u)
#endif

#define LED_PIN     4u   
#define BTN_PIN     28u  

#define DEBOUNCE_US 30000u  

static volatile bool press_pendente = false;  

static void btn_isr(uint gpio, uint32_t events) {
    (void)gpio;
    if (events & GPIO_IRQ_EDGE_FALL) {
        press_pendente = true;
    }
}

int main(void) {

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    bool led_on = false;
    gpio_put(LED_PIN, led_on ? 1 : 0);

    gpio_init(BTN_PIN);
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_pull_up(BTN_PIN);

    gpio_set_irq_enabled_with_callback(BTN_PIN, GPIO_IRQ_EDGE_FALL, true, &btn_isr);

    uint64_t ultimo_toggle_us = 0;

    while (true) {
        if (press_pendente) {
            press_pendente = false;

            uint64_t agora = time_us_64();
            if (agora - ultimo_toggle_us >= DEBOUNCE_US) {
                led_on = !led_on;                 
                gpio_put(LED_PIN, led_on ? 1 : 0);
                ultimo_toggle_us = agora;
            }
        }
        tight_loop_contents();
    }
}
