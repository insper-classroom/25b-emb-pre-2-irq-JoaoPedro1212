#include <stdio.h>
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

static const uint BTN_PIN_R = 28u;
static const uint BTN_PIN_G = 26u;

static volatile bool red_fall_pending = false;
static volatile bool green_fall_pending = false;

static void btn_callback(uint gpio, uint32_t events) {
    if (events & GPIO_IRQ_EDGE_FALL) {
        if (gpio == BTN_PIN_R) {
            red_fall_pending = true;
        } else if (gpio == BTN_PIN_G) {
            green_fall_pending = true;
        }
    }
}

int main(void) {
    stdio_init_all();

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);

    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true, &btn_callback);
    gpio_set_irq_enabled(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true);

    while (true) {
        if (red_fall_pending) {
            red_fall_pending = false;
            printf("fall red\n");
        }
        if (green_fall_pending) {
            green_fall_pending = false;
            printf("fall green\n");
        }

        tight_loop_contents();
    }
}
