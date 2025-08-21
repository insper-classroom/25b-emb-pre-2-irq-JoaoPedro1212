#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define LED_PIN       4u      // LED vermelho do teste (ledr)
#define BTN_PIN       28u      // botão com pull-up (ativo em nível baixo)
#define DEBOUNCE_US   30000u   // ~30 ms

static volatile bool press_pendente = false;

/* ISR curtíssima: apenas sinaliza a borda de descida (press) */
static void btn_isr(uint gpio, uint32_t events) {
    (void)gpio;
    if (events & GPIO_IRQ_EDGE_FALL) {
        press_pendente = true;
    }
}

int main(void) {
    // stdio_init_all(); // não necessário para o teste

    // LED começa ACESO (o teste exige ledr:A == 1 no início)
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 1);

    // Botão com pull-up interno
    gpio_init(BTN_PIN);
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_pull_up(BTN_PIN);

    // Interrupção na borda de descida
    gpio_set_irq_enabled_with_callback(BTN_PIN, GPIO_IRQ_EDGE_FALL, true, &btn_isr);

    uint64_t ultimo_toggle = 0;

    while (true) {
        if (press_pendente) {
            press_pendente = false;

            uint64_t agora = time_us_64();
            if (agora - ultimo_toggle >= DEBOUNCE_US) {
                gpio_xor_mask(1u << LED_PIN);  // alterna o LED
                ultimo_toggle = agora;
            }
        }
        tight_loop_contents();
    }
}
