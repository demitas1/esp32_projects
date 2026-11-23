#pragma once

#include "page.hpp"

// BMI270 accelerometer / gyroscope. B toggles CSV streaming over USB serial:
//   imu,<ms>,<ax>,<ay>,<az>,<gx>,<gy>,<gz>   (accel in g, gyro in deg/s)
class ImuPage : public Page {
public:
    const char* title() const override { return "IMU"; }
    const char* actionLabel() const override { return streaming_ ? "stop csv" : "csv"; }

    void exit() override { streaming_ = false; }

    void onAction() override { streaming_ = !streaming_; }

    void update(M5Canvas& canvas) override {
        if (M5.Imu.update()) data_ = M5.Imu.getImuData();
        const auto& a = data_.accel;
        const auto& g = data_.gyro;

        if (streaming_) {
            Serial.printf("imu,%lu,%.3f,%.3f,%.3f,%.1f,%.1f,%.1f\n", (unsigned long)millis(), a.x, a.y,
                          a.z, g.x, g.y, g.z);
        }

        const int top = layout::kHeaderHeight + 4;
        drawBar(canvas, top, "ax", a.x, kAccelRange, TFT_RED);
        drawBar(canvas, top + 14, "ay", a.y, kAccelRange, TFT_GREEN);
        drawBar(canvas, top + 28, "az", a.z, kAccelRange, TFT_BLUE);
        drawBar(canvas, top + 46, "gx", g.x, kGyroRange, TFT_ORANGE);
        drawBar(canvas, top + 60, "gy", g.y, kGyroRange, TFT_YELLOW);
        drawBar(canvas, top + 74, "gz", g.z, kGyroRange, TFT_CYAN);

        if (streaming_) {
            canvas.setTextColor(TFT_RED);
            canvas.drawString("CSV", canvas.width() - 22, top + 92);
        }
    }

private:
    static constexpr float kAccelRange = 2.0f;    // g
    static constexpr float kGyroRange = 500.0f;   // deg/s

    static void drawBar(M5Canvas& canvas, int y, const char* label, float value, float range,
                        uint16_t color) {
        constexpr int kLabelWidth = 84;
        const int bar_left = kLabelWidth;
        const int bar_width = canvas.width() - bar_left - 4;
        const int center = bar_left + bar_width / 2;
        const float ratio = constrain(value / range, -1.0f, 1.0f);
        const int len = (int)(ratio * bar_width / 2);

        canvas.setTextColor(TFT_WHITE);
        canvas.setCursor(4, y + 2);
        canvas.printf("%s %8.2f", label, value);
        canvas.drawRect(bar_left, y, bar_width, 10, TFT_DARKGREY);
        canvas.drawFastVLine(center, y, 10, TFT_WHITE);
        if (len >= 0) {
            canvas.fillRect(center, y + 1, len, 8, color);
        } else {
            canvas.fillRect(center + len, y + 1, -len, 8, color);
        }
    }

    m5::imu_data_t data_{};
    bool streaming_ = false;
};
