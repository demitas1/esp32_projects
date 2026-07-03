#ifndef RGB_LED_HPP
#define RGB_LED_HPP

#include "color.hpp"

// Abstraction of an RGB LED output device. Callers depend on this interface
// rather than a concrete driver (Dependency Inversion Principle), so the
// underlying hardware (LEDC PWM, addressable LEDs, etc.) can be swapped freely.
class IRgbLed {
public:
    virtual ~IRgbLed() = default;

    // Drive the LED to the given color.
    virtual void setColor(const Rgb& color) = 0;
};

#endif  // RGB_LED_HPP
