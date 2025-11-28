#ifndef LED_HPP
#define LED_HPP

#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include <memory>

#define LED_1 22
#define LED_2 21
#define LED_3 20

#define CLOCK_DIVIDER 125.0f
#define MAX_LED_LEVEL 1000

class Led {
private:
    uint gpio;
    uint slice;
    bool on = false;

public:
    Led(uint _gpio);
    void toggle(bool togg);
    bool operator()();
};

#endif