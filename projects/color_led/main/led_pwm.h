#ifndef LED_PWM_H
#define LED_PWM_H

#include <stdint.h>

void rgb_led_pwm_init(void);
void rgb_led_set_color(uint8_t red, uint8_t green, uint8_t blue);

#endif
