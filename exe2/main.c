#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

/* === AJUSTE AQUI CONFORME O DIAGRAMA === */
#define LED_PIN   4   // tente 16; se não piscar no start, teste 0u
#define BTN_PIN   28   // se não funcionar, teste 20u ou 21u
/* ======================================= */

/* Compat: evita erro do IntelliSense quando o SDK não é detectado */
#ifndef GPIO_IRQ_EDGE_FALL
#define GPIO_IRQ_EDGE_FALL  (0x4u)
#endif
#ifndef GPIO_IRQ_EDGE_RISE
#define GPIO_IRQ_EDGE_RISE  (0x8u)
#endif

static volatile bool press_pendente = false;

static void btn_isr(uint gpio, uint32_t events) {
    (void)gpio;
    if (events & GPIO_IRQ_EDGE_FALL) {
        press_pendente = true;   // ISR curtíssima
    }
}

int main(void) {
    // stdio_init_all(); // não é necessário para este exercício

    // LED como saída (inicia desligado)
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);

    // Auto-teste rápido: pisca 200 ms ao ligar para validar LED_PIN
    gpio_put(LED_PIN, 1);
    sleep_ms(200);
    gpio_put(LED_PIN, 0);

    // Botão com pull-up interno (ativo em nível baixo)
    gpio_init(BTN_PIN);
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_pull_up(BTN_PIN);

    // IRQ na borda de descida (press)
    gpio_set_irq_enabled_with_callback(BTN_PIN, GPIO_IRQ_EDGE_FALL, true, &btn_isr);

    const uint32_t DEBOUNCE_US = 30000u;
    uint64_t ultimo_toggle_us = 0;

    while (true) {
        if (press_pendente) {
            press_pendente = false;

            uint64_t agora = time_us_64();
            if (agora - ultimo_toggle_us >= DEBOUNCE_US) {
                // Alterna o LED
                gpio_xor_mask(1u << LED_PIN);
                ultimo_toggle_us = agora;
            }
        }
        tight_loop_contents();
    }
}
