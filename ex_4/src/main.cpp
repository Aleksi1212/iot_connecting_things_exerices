#include <iostream>
#include <pico/stdlib.h>
#include <mqtt.hpp>
#include <led.hpp>
#include "pico/time.h"
#include <json.hpp>

using namespace std;

#define BUTTON 8

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

void handle_mqtt_message(char *msg, map<string, shared_ptr<Led>> &led_map)
{
    map<string, string> json_obj;
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
    cout << "\nBOOT\n" << endl;

    map<string, shared_ptr<Led>> led_map = {
        { "D1", make_shared<Led>(LED_1) },
        { "D2", make_shared<Led>(LED_2) },
        { "D3", make_shared<Led>(LED_3) }
    };
    
    init_button(BUTTON);
    queue_init(&button_q, sizeof(uint8_t), 10);
        
    auto mqtt = Mqtt::create();
    mqtt->connect();

    uint8_t q_data;
    T_MQTT_payload mqtt_payload{};
    while (true) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, (*mqtt)());
        if (!(*mqtt)()) continue;

        if (queue_try_remove(&button_q, &q_data)) {
            mqtt->send_message(MSG_TOPIC, "{\"topic\": \"%s\", \"msg\": \"%s\"}", MSG_TOPIC, "hello from pico :)");
        }
        if (mqtt->try_get_mqtt_msg(&mqtt_payload) && strcmp(mqtt_payload.topic, LED_TOPIC) == 0) {
            handle_mqtt_message(mqtt_payload.message, led_map);
        }
        mqtt->yield(100);
    }

    return 0;
}
