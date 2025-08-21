#include <stdio.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

static const uint BTN_PIN_R = 28u;

static volatile bool btn_fall_flag = false;

static void btn_callback(uint gpio, uint32_t events) {
    (void)gpio;
    if (events & GPIO_IRQ_EDGE_FALL) {
        btn_fall_flag = true;
    }
}

int main(void) {
    stdio_init_all();

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true, &btn_callback);

    bool capture_flag = false;

    while (true) {
        if (btn_fall_flag) {
            btn_fall_flag = false;

            printf("btn pressed \n");

            while (!gpio_get(BTN_PIN_R)) {
                tight_loop_contents();
            }

            sleep_ms(1);

            printf("btn released \n");

            capture_flag = true;
        }

        if (capture_flag) {
            capture_flag = false;
        }
        tight_loop_contents();
    }
}
