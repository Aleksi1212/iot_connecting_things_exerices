#include <utils.hpp>

float adc_to_celcius(uint16_t adc_reading)
{
    float voltage = adc_reading * ADC_REF / ADC_RESOLUTION;
    return 27.0f - (voltage - V_AT_27C) / TEMP_SLOPE;
}
std::string char_arr_to_str(char *c_arr, size_t size)
{
    std::string str = "";
    for (size_t i = 0; i < size; ++i) {
        str += c_arr[i];
    }
    return str;
}
std::vector<std::string> split_str(std::string &str, const char c)
{
    std::stringstream ss(str);
    std::string seg;
    std::vector<std::string> seg_vec;

    while (std::getline(ss, seg, c)) seg_vec.push_back(seg);
    return seg_vec;
}
