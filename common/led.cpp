#include <led.hpp>

Led::Led(uint _gpio) : gpio(_gpio)
{
    gpio_set_function(gpio, GPIO_FUNC_PWM);

    slice = pwm_gpio_to_slice_num(gpio);

    pwm_set_clkdiv(slice, CLOCK_DIVIDER);
    pwm_set_wrap(slice, MAX_LED_LEVEL-1);
    pwm_set_enabled(slice, true);
}

void Led::toggle(bool togg)
{
    uint16_t level = togg ? MAX_LED_LEVEL / 4 : 0;
    pwm_set_chan_level(slice, pwm_gpio_to_channel(gpio), level);
    on = togg;
}

bool Led::operator()() { return on; }
