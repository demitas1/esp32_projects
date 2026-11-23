#pragma once

#include "page.hpp"

// System and power information.
class InfoPage : public Page {
public:
    const char* title() const override { return "System"; }

    void update(M5Canvas& canvas) override {
        const uint32_t now = millis();
        if (now - last_power_read_ms_ >= kPowerReadIntervalMs || last_power_read_ms_ == 0) {
            battery_level_ = M5.Power.getBatteryLevel();
            battery_mv_ = M5.Power.getBatteryVoltage();
            charging_ = M5.Power.isCharging();
            last_power_read_ms_ = now;
        }

        const bool is_sticks3 = M5.getBoard() == m5::board_t::board_M5StickS3;
        canvas.setTextColor(TFT_WHITE);
        canvas.setCursor(4, layout::kHeaderHeight + 4);
        canvas.printf("Board : %s (%d)\n", is_sticks3 ? "M5StickS3" : "unknown", (int)M5.getBoard());
        canvas.printf(" Chip : %s rev%d %luMHz\n", ESP.getChipModel(), ESP.getChipRevision(),
                      (unsigned long)ESP.getCpuFreqMHz());
        canvas.printf(" Flash: %luMB\n", (unsigned long)(ESP.getFlashChipSize() >> 20));
        canvas.printf(" PSRAM: %luKB free / %luKB\n", (unsigned long)(ESP.getFreePsram() >> 10),
                      (unsigned long)(ESP.getPsramSize() >> 10));
        canvas.printf(" Heap : %luKB free\n", (unsigned long)(ESP.getFreeHeap() >> 10));
        canvas.printf(" Batt : %ld%% %dmV %s\n", (long)battery_level_, battery_mv_,
                      charging_ ? "charging" : "");
        canvas.printf(" Up   : %lus\n", (unsigned long)(now / 1000));
    }

private:
    static constexpr uint32_t kPowerReadIntervalMs = 1000;

    uint32_t last_power_read_ms_ = 0;
    int32_t battery_level_ = 0;
    int16_t battery_mv_ = 0;
    bool charging_ = false;
};
