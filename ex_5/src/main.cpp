#include <iostream>
#include <pico/stdlib.h>
#include <mqtt.hpp>
#include <led.hpp>
#include "pico/time.h"
#include "hardware/adc.h"
#include <algorithm>
#include <utils.hpp>

using namespace std;

#define BUTTON 12

static queue_t button_q;
void irq_callback(uint gpio, uint32_t event_mask)
{
    if (gpio == BUTTON && (event_mask & GPIO_IRQ_EDGE_RISE)) {
        uint8_t press = 1;
        queue_try_add(&button_q, &press);
    }
}

void init_button(uint button)
{
    gpio_init(button);
    gpio_set_dir(button, GPIO_IN);
    gpio_pull_up(button);

    gpio_set_irq_enabled_with_callback(button,
        GPIO_IRQ_EDGE_RISE, true, &irq_callback);
}

int main()
{
    stdio_init_all();

    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(TEMP_INPUT);

    Led led1(LED_1);
    Led led2(LED_2);
    Led led3(LED_3);

    init_button(BUTTON);
    queue_init(&button_q, sizeof(uint8_t), 10);
        
    auto mqtt = Mqtt::create();
    mqtt->connect();

    float low_temp_lim = 20.0f;
    float high_temp_lim = 30.0f;
    float core_temp = adc_to_celcius(adc_read());

    absolute_time_t start_temp_read_time = get_absolute_time();
    uint8_t q_data;
    T_MQTT_payload mqtt_payload{};

    while (true) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, (*mqtt)());
        led1.toggle(core_temp <= low_temp_lim);
        led3.toggle(core_temp >= high_temp_lim);

        if (absolute_time_diff_us(start_temp_read_time, get_absolute_time()) / 1000 >= 1000) {
            core_temp = adc_to_celcius(adc_read());
            start_temp_read_time = get_absolute_time();
            led2.toggle(!led2());
        }

        if (!(*mqtt)()) continue;
        if (queue_try_remove(&button_q, &q_data)) {
            send_core_temp(mqtt.get(), MSG_TOPIC, core_temp);
        }
        if (mqtt->try_get_mqtt_msg(&mqtt_payload) && strcmp(mqtt_payload.topic, TEMP_TOPIC) == 0) {
            string msg = char_arr_to_str(mqtt_payload.message, strlen(mqtt_payload.message));
            transform(msg.begin(), msg.end(), msg.begin(), ::toupper);
            vector<string> msg_split = split_str(msg, ';');

            if (msg_split[0] == "TEMP") send_core_temp(mqtt.get(), MSG_TOPIC, core_temp);
            if (msg_split[0] == "LOW") low_temp_lim = stof(msg_split[1]);
            if (msg_split[0] == "HIGH") high_temp_lim = stof(msg_split[1]);
        } 
        mqtt->yield(100);
    }

    return 0;
}
