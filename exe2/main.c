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

#define LED_PIN 4u   // pino do LED conforme diagrama do exe2
#define BTN_PIN 28u   // pino do botão (pull-up, ativo em nível baixo)
#define DEBOUNCE_US 30000u

static volatile bool press_pendente = false;

static void btn_isr(uint gpio, uint32_t events) {
    (void)gpio;
    if (events & GPIO_IRQ_EDGE_FALL) {
        press_pendente = true;  // ISR curtíssima
    }
}

int main(void) {
    // stdio_init_all(); // não necessário para o teste

    // LED: inicia ACESO (o teste espera ledr:A == 1 em ~300 ms)
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 1);

    // Botão com pull-up interno
    gpio_init(BTN_PIN);
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_pull_up(BTN_PIN);

    // Interrupção na borda de descida (press)
    gpio_set_irq_enabled_with_callback(BTN_PIN, GPIO_IRQ_EDGE_FALL, true, &btn_isr);

    uint64_t ultimo_toggle_us = 0;

    while (true) {
        if (press_pendente) {
            press_pendente = false;

            uint64_t agora = time_us_64();
            if (agora - ultimo_toggle_us >= DEBOUNCE_US) {
                gpio_xor_mask(1u << LED_PIN);  // alterna o LED
                ultimo_toggle_us = agora;
            }
        }
        tight_loop_contents();
    }
}
