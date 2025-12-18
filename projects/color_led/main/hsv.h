#ifndef HSV_H
#define HSV_H

#include <stdint.h>

void hsv_to_rgb(uint16_t hue, uint8_t sat, uint8_t val,
                uint8_t *r, uint8_t *g, uint8_t *b);

#endif
