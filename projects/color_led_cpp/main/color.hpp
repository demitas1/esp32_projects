#ifndef COLOR_HPP
#define COLOR_HPP

#include <cstdint>

// RGB color value (0..255 per channel).
struct Rgb {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

// HSV color value. hue is 0..359 degrees, sat/val are 0..255.
struct Hsv {
    uint16_t h;
    uint8_t s;
    uint8_t v;

    // Convert this HSV value to RGB (pure function, no side effects).
    Rgb toRgb() const;
};

#endif  // COLOR_HPP
