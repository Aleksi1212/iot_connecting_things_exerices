add_library(common STATIC
    Countdown.cpp
    IPStack.cpp
    json.cpp
    led.cpp
    mqtt.cpp
)

# Export the include path
target_include_directories(common
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ../lib/paho.mqtt.embedded-c/MQTTPacket/src
        ../lib/paho.mqtt.embedded-c
)

# Link Pico SDK dependencies used by the common code
target_link_libraries(common
    PUBLIC            # important keyword
        pico_stdlib
        hardware_gpio
        hardware_pwm
        pico_cyw43_arch_lwip_poll
)
