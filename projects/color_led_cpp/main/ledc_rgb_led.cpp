#include "ledc_rgb_led.hpp"

LedcRgbLed::LedcRgbLed(gpio_num_t red, gpio_num_t green, gpio_num_t blue)
    : channels_{{
          {LEDC_CHANNEL_0, red},
          {LEDC_CHANNEL_1, green},
          {LEDC_CHANNEL_2, blue},
      }}
{
    ledc_timer_config_t timer = {};
    timer.speed_mode = kSpeedMode;
    timer.duty_resolution = kDutyResolution;
    timer.timer_num = kTimer;
    timer.freq_hz = kFreqHz;
    ledc_timer_config(&timer);

    for (const Channel& ch : channels_) {
        ledc_channel_config_t channel = {};
        channel.gpio_num = ch.gpio;
        channel.speed_mode = kSpeedMode;
        channel.channel = ch.channel;
        // intr_type is deprecated; interrupts are handled by the driver.
        channel.timer_sel = kTimer;
        channel.duty = 0;
        channel.hpoint = 0;
        ledc_channel_config(&channel);
    }
}

void LedcRgbLed::setColor(const Rgb& color)
{
    applyDuty(channels_[0], color.r);
    applyDuty(channels_[1], color.g);
    applyDuty(channels_[2], color.b);
}

void LedcRgbLed::applyDuty(const Channel& ch, uint8_t duty)
{
    ledc_set_duty(kSpeedMode, ch.channel, duty);
    ledc_update_duty(kSpeedMode, ch.channel);
}
