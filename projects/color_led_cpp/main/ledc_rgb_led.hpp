#ifndef LEDC_RGB_LED_HPP
#define LEDC_RGB_LED_HPP

#include <array>

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "rgb_led.hpp"

// RGB LED driven by the ESP32 LEDC (PWM) peripheral. The constructor configures
// the shared timer and one channel per color, so a constructed instance is
// always ready to use (RAII).
class LedcRgbLed : public IRgbLed {
public:
    LedcRgbLed(gpio_num_t red, gpio_num_t green, gpio_num_t blue);

    void setColor(const Rgb& color) override;

private:
    // LEDC timer/channel parameters shared by all color channels.
    static constexpr ledc_mode_t kSpeedMode = LEDC_HIGH_SPEED_MODE;
    static constexpr ledc_timer_t kTimer = LEDC_TIMER_0;
    static constexpr ledc_timer_bit_t kDutyResolution = LEDC_TIMER_8_BIT;
    static constexpr uint32_t kFreqHz = 100;

    struct Channel {
        ledc_channel_t channel;
        gpio_num_t gpio;
    };

    // Index 0 = red, 1 = green, 2 = blue.
    std::array<Channel, 3> channels_;

    void applyDuty(const Channel& ch, uint8_t duty);
};

#endif  // LEDC_RGB_LED_HPP
