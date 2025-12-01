#include <iostream>
#include <pico/stdlib.h>
#include <mqtt.hpp>
#include <led.hpp>
#include "pico/time.h"
#include "hardware/adc.h"
#include <algorithm>
#include <utils.hpp>
#include <json.hpp>


#define BUTTON_1 8
#define BUTTON_2 12

static queue_t button_q;
void irq_callback(uint gpio, uint32_t event_mask)
{
    if (gpio == BUTTON_1 || gpio == BUTTON_2 && (event_mask & GPIO_IRQ_EDGE_RISE)) {
        queue_try_add(&button_q, &gpio);
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

void handle_led_topic_message(char *msg, std::map<std::string, std::shared_ptr<Led>> &led_map)
{
    std::map<std::string, std::string> json_obj;
    if (!parse_json_str(msg, json_obj)) return;

    auto led_it = led_map.find(json_obj.find("LED")->second.c_str());
    if (led_it != led_map.end()) {
        auto cmd = json_obj.find("CMD")->second;
        bool led_val = cmd == "TOGG" ? !(*led_it->second)() : cmd == "ON";
        led_it->second->toggle(led_val);
    }
}

int main()
{
    stdio_init_all();

    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(TEMP_INPUT);

    std::map<std::string, std::shared_ptr<Led>> led_map = {
        { "D1", std::make_shared<Led>(LED_1) },
        { "D2", std::make_shared<Led>(LED_2) },
        { "D3", std::make_shared<Led>(LED_3) }
    };

    init_button(BUTTON_1);
    init_button(BUTTON_2);
    queue_init(&button_q, sizeof(uint8_t), 10);

    auto mqtt = Mqtt::create();
    mqtt->connect();

    float low_temp_lim = 20.0f;
    float high_temp_lim = 30.0f;
    float core_temp = adc_to_celcius(adc_read());

    absolute_time_t start_temp_read_time = get_absolute_time();
    uint8_t q_data;
    T_MQTT_payload mqtt_payload{};

    auto led1 = led_map.find("D1")->second;
    auto led2 = led_map.find("D2")->second;
    auto led3 = led_map.find("D3")->second;

    while (true)
    {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, (*mqtt)());
        led1->toggle(core_temp <= low_temp_lim);
        led3->toggle(core_temp >= high_temp_lim);

        if (absolute_time_diff_us(start_temp_read_time, get_absolute_time()) / 1000 >= 1000)
        {
            core_temp = adc_to_celcius(adc_read());
            start_temp_read_time = get_absolute_time();
            led2->toggle(!(*led2)());
        }

        if (!(*mqtt)()) continue;
        if (queue_try_remove(&button_q, &q_data))
        {
            if (q_data == BUTTON_1) mqtt->send_message(MSG_TOPIC, "Pico says hello :)");
            if (q_data == BUTTON_2) send_core_temp(mqtt.get(), MSG_TOPIC, core_temp);
        }
        
        bool mqtt_message = mqtt->try_get_mqtt_msg(&mqtt_payload);
        if (mqtt_message && !strcmp(mqtt_payload.topic, LED_TOPIC)) {
            handle_led_topic_message(mqtt_payload.message, led_map);
        }
        if (mqtt_message && !strcmp(mqtt_payload.topic, TEMP_TOPIC))
        {
            std::string payload_msg = char_arr_to_str(mqtt_payload.message, strlen(mqtt_payload.message));
            to_upper(payload_msg);
            std::vector<std::string> msg_parts = split_str(payload_msg, ';');

            if (msg_parts[0] == "TEMP") {
                send_core_temp(mqtt.get(), MSG_TOPIC, core_temp);
            }
            if (msg_parts[0] == "LOW") {
                try_convert_str_to_float(msg_parts[1], low_temp_lim);
            }
            if (msg_parts[0] == "HIGH") {
                try_convert_str_to_float(msg_parts[1], high_temp_lim);
            }
        }

        mqtt->yield(100);
    }

    return 0;
}
