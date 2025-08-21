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

static const uint BTN_PIN = 28u;

static const uint32_t DEBOUNCE_US   = 30000u;   
static const uint32_t LONG_PRESS_US = 800000u;  

#define EV_FALL (1u << 0)
#define EV_RISE (1u << 1)

static volatile uint8_t eventos_pendentes = 0;

static void btn_isr(uint gpio, uint32_t events) {
    (void)gpio;
    if (events & GPIO_IRQ_EDGE_FALL) eventos_pendentes |= EV_FALL;
    if (events & GPIO_IRQ_EDGE_RISE) eventos_pendentes |= EV_RISE;
}

int main(void) {
    stdio_init_all();

    gpio_init(BTN_PIN);
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_pull_up(BTN_PIN);

    gpio_set_irq_enabled_with_callback(
        BTN_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &btn_isr
    );

    uint64_t t_press_us = 0;       
    uint64_t ultimo_edge_us = 0;    

    while (true) {
        uint8_t ev = eventos_pendentes;
        if (ev) {
            eventos_pendentes = 0;

            uint64_t agora = time_us_64();

            if ((ev & EV_FALL) && (agora - ultimo_edge_us >= DEBOUNCE_US)) {
                t_press_us = agora;             
                ultimo_edge_us = agora;
            }

            if ((ev & EV_RISE) && (agora - ultimo_edge_us >= DEBOUNCE_US)) {
                uint64_t dur = agora - t_press_us;
                if (dur >= LONG_PRESS_US) {
                    printf("Aperto longo!\n");
                } else {
                    printf("Aperto curto!\n");
                }
                ultimo_edge_us = agora;
            }
        }

        tight_loop_contents();
    }
}
