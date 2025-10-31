#include <iostream>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include <hardware.hpp>
#include <string.h>

#include "ssd1306.h"
#include "PicoI2CBus.h"
#include "PicoI2CDevice.h"
#include "mono_horiz.h"
#include "lovepill.h"

#define SECOND_MS 1000

void init_led(uint pin)
{
    gpio_init(pin);
    gpio_set_dir(pin, true);
}

void toggle_led(uint led, absolute_time_t start_time, uint16_t timeout)
{
    uint64_t led_elapsed_time_ms = absolute_time_diff_us(start_time, get_absolute_time()) / 1000;

    if ((led_elapsed_time_ms / timeout) % 2 == 0) gpio_put(led, true);
    else gpio_put(led, false);
}

int main(void)
{   
    stdio_init_all();

    init_led(LED_1);
    init_led(LED_2);
    init_led(LED_3);

    auto bus = std::make_shared<PicoI2CBus>(1, OLED_SDA_PIN, OLED_SCL_PIN);
    auto dev = std::make_shared<PicoI2CDevice>(bus, OLED_ADDR);
    ssd1306 display(dev);
    mono_vlsb lovepill(binary_data, 48, 48);

    absolute_time_t led1_start_time = get_absolute_time(),
        led2_start_time = get_absolute_time(),
        led3_start_time = get_absolute_time();

    absolute_time_t curr_time = get_absolute_time();
    uint64_t time_elapsed_s = 0;

    
    display.fill(0);
    display.text("Time: 0s", 0, 0);
    display.blit(lovepill, 0, 10);
    display.show();

    while (true) {
        if (absolute_time_diff_us(curr_time, get_absolute_time()) >= SECOND_MS * 1000) {
            time_elapsed_s++;
            curr_time = get_absolute_time();

            char msg_buf[256];
            snprintf(msg_buf, sizeof(msg_buf), "Time: %ds", (unsigned int)time_elapsed_s);

            display.fill(0);
            display.text(msg_buf, 0, 0);
            display.blit(lovepill, 0, 10);
            display.show();

            std::cout << msg_buf << std::endl;
        }

        toggle_led(LED_1, led1_start_time, SECOND_MS / 4);
        toggle_led(LED_2, led2_start_time, SECOND_MS / 2);
        toggle_led(LED_3, led3_start_time, SECOND_MS);
    }

    return 0;
}
