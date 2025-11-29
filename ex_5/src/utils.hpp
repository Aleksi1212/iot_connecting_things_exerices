#ifndef UTILS_HPP
#define UTILS_HPP

#include "calc_consts.h"
#include <cstdint>
#include <iostream>
#include <sstream>
#include <vector>

#pragma once

template <typename MqttT>
int send_core_temp(MqttT *mqtt, const char *topic, float temp) {
    return mqtt->send_message(topic, "Core temp: %.2f C", temp);
}
float adc_to_celcius(uint16_t adc_reading);

std::string char_arr_to_str(char *c_arr, size_t size);
std::vector<std::string> split_str(std::string &str, const char c);

#endif