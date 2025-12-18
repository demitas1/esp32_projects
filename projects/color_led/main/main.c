#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led_pwm.h"
#include "hsv.h"

void app_main(void)
{
    rgb_led_pwm_init();

    printf("Rainbow LED Start\n");

    uint16_t hue = 0;
    uint8_t r, g, b;

    while (1) {
        hsv_to_rgb(hue, 255, 255, &r, &g, &b);
        rgb_led_set_color(r, g, b);

        hue = (hue + 1) % 360;
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
