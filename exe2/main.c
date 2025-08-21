#include <stdio.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

static const uint BTN_PIN = 28u;   
static const uint LED_PIN = 16u;   
static const uint32_t DEBOUNCE_US = 30000u;

static volatile bool btn_press_pending = false;

static void btn_irq_callback(uint gpio, uint32_t events) {
    (void)gpio;
    if (events & GPIO_IRQ_EDGE_FALL) {
        btn_press_pending = true;
    }
}

int main(void) {
    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    bool led_state = false;
    gpio_put(LED_PIN, led_state);

    gpio_init(BTN_PIN);
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_pull_up(BTN_PIN);

    gpio_set_irq_enabled_with_callback(
        BTN_PIN, GPIO_IRQ_EDGE_FALL, true, &btn_irq_callback
    );

    uint64_t ultima_troca_us = 0;

    while (true) {
        if (btn_press_pending) {
            btn_press_pending = false;

            uint64_t agora = time_us_64();
            if (agora - ultima_troca_us >= DEBOUNCE_US) {
                led_state = !led_state;       
                gpio_put(LED_PIN, led_state); 
                ultima_troca_us = agora;
            }
        }

        tight_loop_contents();
    }
}
