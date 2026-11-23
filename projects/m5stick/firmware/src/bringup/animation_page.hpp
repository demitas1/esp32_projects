#pragma once

#include <array>

#include "page.hpp"

// Bouncing balls drawn into an off-screen canvas: checks display throughput.
class AnimationPage : public Page {
public:
    const char* title() const override { return "Animation"; }
    const char* actionLabel() const override { return "balls"; }

    void enter() override {
        if (!initialized_) {
            for (auto& ball : balls_) respawn(ball);
            initialized_ = true;
        }
        frame_count_ = 0;
        fps_window_start_ms_ = millis();
    }

    void onAction() override {
        active_count_ = (active_count_ >= kMaxBalls) ? kMinBalls : active_count_ * 2;
    }

    void update(M5Canvas& canvas) override {
        const int top = layout::kHeaderHeight;
        const int bottom = canvas.height() - layout::kFooterHeight;
        const int right = canvas.width();

        for (size_t i = 0; i < active_count_; ++i) {
            Ball& b = balls_[i];
            b.x += b.vx;
            b.y += b.vy;
            if (b.x < b.r || b.x > right - b.r) b.vx = -b.vx;
            if (b.y < top + b.r || b.y > bottom - b.r) b.vy = -b.vy;
            canvas.fillCircle((int)b.x, (int)b.y, b.r, b.color);
        }

        countFrame();
        canvas.setTextColor(TFT_WHITE);
        canvas.setCursor(4, top + 2);
        canvas.printf("%u balls  %.1f fps", (unsigned)active_count_, fps_);
    }

private:
    struct Ball {
        float x, y, vx, vy;
        int r;
        uint16_t color;
    };

    static constexpr size_t kMinBalls = 8;
    static constexpr size_t kMaxBalls = 64;

    static void respawn(Ball& b) {
        b.r = random(3, 9);
        b.x = random(20, 220);
        b.y = random(30, 100);
        b.vx = random(-30, 31) / 10.0f;
        b.vy = random(-30, 31) / 10.0f;
        if (b.vx == 0) b.vx = 1.0f;
        b.color = M5.Display.color565(random(64, 256), random(64, 256), random(64, 256));
    }

    void countFrame() {
        ++frame_count_;
        const uint32_t elapsed = millis() - fps_window_start_ms_;
        if (elapsed >= 1000) {
            fps_ = frame_count_ * 1000.0f / elapsed;
            frame_count_ = 0;
            fps_window_start_ms_ = millis();
        }
    }

    std::array<Ball, kMaxBalls> balls_{};
    size_t active_count_ = kMinBalls;
    bool initialized_ = false;
    uint32_t frame_count_ = 0;
    uint32_t fps_window_start_ms_ = 0;
    float fps_ = 0;
};
