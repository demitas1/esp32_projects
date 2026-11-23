#pragma once

#include <M5Unified.h>

// One screen of the bringup app. Each page checks one piece of hardware.
class Page {
public:
    virtual ~Page() = default;

    virtual const char* title() const = 0;
    // Hint for the B button shown in the footer (nullptr = no action).
    virtual const char* actionLabel() const { return nullptr; }

    virtual void enter() {}
    virtual void exit() {}
    // Called every frame. Draw into the canvas below the header.
    virtual void update(M5Canvas& canvas) = 0;
    // B button clicked.
    virtual void onAction() {}
};

namespace layout {
constexpr int kHeaderHeight = 16;
constexpr int kFooterHeight = 12;
}  // namespace layout
