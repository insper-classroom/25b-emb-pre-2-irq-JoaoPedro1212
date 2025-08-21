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

enum { EV_FALL = 1u, EV_RISE = 2u };
static volatile uint8_t btn_events = 0;

static void btn_callback(uint gpio, uint32_t events) {
    (void)gpio; 
    if (events & GPIO_IRQ_EDGE_FALL) btn_events |= EV_FALL;
    if (events & GPIO_IRQ_EDGE_RISE) btn_events |= EV_RISE;
}

int main(void) {
    stdio_init_all();

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_set_irq_enabled_with_callback(
        BTN_PIN_R,
        GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
        true,
        &btn_callback
    );

    while (true) {
        uint8_t ev = btn_events;
        if (ev) {
            btn_events = 0;

            if (ev & EV_FALL) {
                printf("fall \n");
            }
            if (ev & EV_RISE) {
                printf("rise \n");
            }
        }

        tight_loop_contents();
    }
}

