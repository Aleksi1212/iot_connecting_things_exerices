#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstdint>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

#define V_AT_27C 0.706f
#define TEMP_SLOPE 0.001721f
#define ADC_REF 3.3f
#define ADC_RESOLUTION 4096.0f

#define TEMP_INPUT 4

template <typename MqttT>
int send_core_temp(MqttT *mqtt, const char *topic, float temp) {
    return mqtt->send_message(topic, "Core temp: %.2f C", temp);
}
float adc_to_celcius(uint16_t adc_reading);

std::string char_arr_to_str(char *c_arr, size_t size);
std::vector<std::string> split_str(std::string &str, const char c);
void try_convert_str_to_float(std::string &str, float &res);
void to_upper(std::string &str);

#endif