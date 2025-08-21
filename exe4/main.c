#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#ifndef GPIO_IRQ_LEVEL_LOW
#define GPIO_IRQ_LEVEL_LOW   (0x1u)
#endif
#ifndef GPIO_IRQ_LEVEL_HIGH
#define GPIO_IRQ_LEVEL_HIGH  (0x2u)
#endif
#ifndef GPIO_IRQ_EDGE_FALL
#define GPIO_IRQ_EDGE_FALL   (0x4u)
#endif
#ifndef GPIO_IRQ_EDGE_RISE
#define GPIO_IRQ_EDGE_RISE   (0x8u)
#endif

static const uint BTN_PIN_RED   = 28u; 
static const uint BTN_PIN_GREEN = 26u; 
static const uint LED_PIN_RED   = 4u;  
static const uint LED_PIN_GREEN = 6u;  

#define EV_RED_PRESS      (1u << 0)
#define EV_GREEN_RELEASE  (1u << 1)

static volatile uint8_t eventos_pendentes = 0;

static void botoes_isr(uint gpio, uint32_t events) {
    if ((gpio == BTN_PIN_RED) && (events & GPIO_IRQ_EDGE_FALL)) {
        eventos_pendentes |= EV_RED_PRESS;
    }
    if ((gpio == BTN_PIN_GREEN) && (events & GPIO_IRQ_EDGE_RISE)) {
        eventos_pendentes |= EV_GREEN_RELEASE;
    }
}

int main(void) {
    stdio_init_all();

    gpio_init(LED_PIN_RED);
    gpio_set_dir(LED_PIN_RED, GPIO_OUT);
    gpio_put(LED_PIN_RED, 0);

    gpio_init(LED_PIN_GREEN);
    gpio_set_dir(LED_PIN_GREEN, GPIO_OUT);
    gpio_put(LED_PIN_GREEN, 0);

    gpio_init(BTN_PIN_RED);
    gpio_set_dir(BTN_PIN_RED, GPIO_IN);
    gpio_pull_up(BTN_PIN_RED);

    gpio_init(BTN_PIN_GREEN);
    gpio_set_dir(BTN_PIN_GREEN, GPIO_IN);
    gpio_pull_up(BTN_PIN_GREEN);

    gpio_set_irq_enabled_with_callback(BTN_PIN_RED, GPIO_IRQ_EDGE_FALL, true, &botoes_isr);
    gpio_set_irq_enabled(BTN_PIN_GREEN, GPIO_IRQ_EDGE_RISE, true);

    const uint32_t DEBOUNCE_US = 30000u;
    uint64_t ultimo_red_us = 0, ultimo_green_us = 0;

    while (true) {
        uint8_t ev = eventos_pendentes;

        if (ev & EV_RED_PRESS) {
            eventos_pendentes &= ~EV_RED_PRESS;
            uint64_t agora = time_us_64();
            if (agora - ultimo_red_us >= DEBOUNCE_US) {
                gpio_xor_mask(1u << LED_PIN_RED);
                ultimo_red_us = agora;
            }
        }

        if (ev & EV_GREEN_RELEASE) {
            eventos_pendentes &= ~EV_GREEN_RELEASE;
            uint64_t agora = time_us_64();
            if (agora - ultimo_green_us >= DEBOUNCE_US) {
                gpio_xor_mask(1u << LED_PIN_GREEN);
                ultimo_green_us = agora;
            }
        }

        tight_loop_contents();
    }
}
