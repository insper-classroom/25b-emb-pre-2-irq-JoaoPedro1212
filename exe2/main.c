#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

/* Fallbacks para o IntelliSense quando o SDK não é detectado */
#ifndef GPIO_IRQ_EDGE_FALL
#define GPIO_IRQ_EDGE_FALL (0x4u)
#endif
#ifndef GPIO_IRQ_EDGE_RISE
#define GPIO_IRQ_EDGE_RISE (0x8u)
#endif

/* AJUSTE CONFORME O DIAGRAMA DO EXE2 */
#define LED_PIN     4u   /* seu LED está no GP4 */
#define BTN_PIN     28u  /* botão com pull-up, ativo em nível baixo */

#define DEBOUNCE_US 30000u  /* ~30 ms */

static volatile bool press_pendente = false;  /* única global compartilhada com a ISR */

/* ISR curtíssima: apenas sinaliza a borda de descida (aperto) */
static void btn_isr(uint gpio, uint32_t events) {
    (void)gpio;
    if (events & GPIO_IRQ_EDGE_FALL) {
        press_pendente = true;
    }
}

int main(void) {
    // stdio_init_all();  // não é necessário para este exercício

    /* LED começa APAGADO, como no gif do enunciado */
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    bool led_on = false;
    gpio_put(LED_PIN, led_on ? 1 : 0);

    /* Botão com pull-up interno (ativo em nível baixo) */
    gpio_init(BTN_PIN);
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_pull_up(BTN_PIN);

    /* Interrupção na borda de descida (press) */
    gpio_set_irq_enabled_with_callback(BTN_PIN, GPIO_IRQ_EDGE_FALL, true, &btn_isr);

    uint64_t ultimo_toggle_us = 0;

    while (true) {
        if (press_pendente) {
            press_pendente = false;

            uint64_t agora = time_us_64();
            if (agora - ultimo_toggle_us >= DEBOUNCE_US) {
                led_on = !led_on;                 /* alterna estado salvo */
                gpio_put(LED_PIN, led_on ? 1 : 0);/* aplica ao pino */
                ultimo_toggle_us = agora;
            }
        }
        tight_loop_contents();
    }
}
