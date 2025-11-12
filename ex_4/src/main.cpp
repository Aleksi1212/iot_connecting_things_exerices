#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "pico/cyw43_arch.h"
#include "wifi_info.h"

#define LED_1 20
#define LED_2 21
#define LED_3 22

void init_led(uint led) {
    gpio_init(led);
    gpio_set_dir(led, GPIO_OUT);
}

int main(void) {
    stdio_init_all();

    init_led(LED_1);
    init_led(LED_2);
    init_led(LED_3);

    gpio_put(LED_1, true);

    bool success = cyw43_arch_init() == 0;

    if (success) {
        cyw43_arch_enable_sta_mode();
        success = cyw43_arch_wifi_connect_timeout_ms(SSID, PW, CYW43_AUTH_WPA2_AES_PSK, 30000) == 0;
    }

    gpio_put(LED_1, false);

    while (true) {
        gpio_put(success ? LED_2 : LED_3, true);
        // gpio_put(led, true);
        // sleep_ms(1000);
        // gpio_put(led, false);
        // sleep_ms(1000);
    }

    return 0;
}