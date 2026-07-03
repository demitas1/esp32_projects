#include <cstdio>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "color.hpp"
#include "ledc_rgb_led.hpp"
#include "rgb_led.hpp"

extern "C" void app_main(void)
{
    LedcRgbLed led(GPIO_NUM_5, GPIO_NUM_22, GPIO_NUM_23);
    IRgbLed& out = led;  // Depend on the abstraction, not the LEDC driver.

    printf("Rainbow LED Start\n");

    uint16_t hue = 0;
    while (true) {
        out.setColor(Hsv{hue, 255, 255}.toRgb());

        hue = (hue + 1) % 360;
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
