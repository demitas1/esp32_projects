#include "color.hpp"

Rgb Hsv::toRgb() const
{
    // Achromatic case: saturation of 0 means a shade of gray.
    if (s == 0) {
        return Rgb{v, v, v};
    }

    const uint16_t region = h / 60;
    const uint16_t remainder = (h % 60) * 255 / 60;

    const uint8_t p = (v * (255 - s)) / 255;
    const uint8_t q = (v * (255 - (s * remainder) / 255)) / 255;
    const uint8_t t = (v * (255 - (s * (255 - remainder)) / 255)) / 255;

    switch (region) {
        case 0:  return Rgb{v, t, p};
        case 1:  return Rgb{q, v, p};
        case 2:  return Rgb{p, v, t};
        case 3:  return Rgb{p, q, v};
        case 4:  return Rgb{t, p, v};
        default: return Rgb{v, p, q};
    }
}
