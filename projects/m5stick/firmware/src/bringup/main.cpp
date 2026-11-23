// StickS3 bringup: checks display, buttons, IMU, speaker and microphone.
//   BtnA (front): next page
//   BtnB (side) : page action

#include <M5Unified.h>

#include <array>

#include "animation_page.hpp"
#include "imu_page.hpp"
#include "info_page.hpp"
#include "mic_page.hpp"
#include "page.hpp"
#include "speaker_page.hpp"

namespace {

constexpr uint32_t kFrameIntervalMs = 33;  // ~30 fps

InfoPage info_page;
AnimationPage animation_page;
ImuPage imu_page;
SpeakerPage speaker_page;
MicPage mic_page;

std::array<Page*, 5> pages = {&info_page, &animation_page, &imu_page, &speaker_page, &mic_page};
size_t current = 0;

M5Canvas canvas(&M5.Display);
uint32_t last_frame_ms = 0;

void switchTo(size_t index) {
    pages[current]->exit();
    current = index % pages.size();
    pages[current]->enter();
    Serial.printf("page: %s\n", pages[current]->title());
}

void drawFrame(Page& page) {
    canvas.fillScreen(TFT_BLACK);
    page.update(canvas);

    // Header
    canvas.fillRect(0, 0, canvas.width(), layout::kHeaderHeight, TFT_NAVY);
    canvas.setTextColor(TFT_WHITE);
    canvas.setCursor(4, 4);
    canvas.printf("%s", page.title());
    canvas.setCursor(canvas.width() - 30, 4);
    canvas.printf("%u/%u", (unsigned)(current + 1), (unsigned)pages.size());

    // Footer: button hints
    const int footer_y = canvas.height() - layout::kFooterHeight;
    canvas.fillRect(0, footer_y, canvas.width(), layout::kFooterHeight, TFT_DARKGREY);
    canvas.setCursor(4, footer_y + 2);
    const char* action = page.actionLabel();
    canvas.printf("A:next  B:%s", action ? action : "-");

    canvas.pushSprite(0, 0);
}

}  // namespace

void setup() {
    // HWCDC (ARDUINO_USB_MODE=1) is not started automatically and
    // M5Unified 0.3.0 defaults serial_baudrate to 0, so start it explicitly.
    Serial.begin(115200);

    auto cfg = M5.config();
    M5.begin(cfg);
    M5.Display.setRotation(1);  // landscape 240x135

    canvas.setColorDepth(16);
    canvas.createSprite(M5.Display.width(), M5.Display.height());
    canvas.setTextSize(1);

    Serial.printf("StickS3 bringup: board=%d imu=%d\n", (int)M5.getBoard(), (int)M5.Imu.getType());
    pages[current]->enter();
}

void loop() {
    M5.update();
    if (M5.BtnA.wasClicked()) switchTo(current + 1);
    if (M5.BtnB.wasClicked()) pages[current]->onAction();

    const uint32_t now = millis();
    if (now - last_frame_ms < kFrameIntervalMs) {
        delay(1);
        return;
    }
    last_frame_ms = now;
    drawFrame(*pages[current]);
}
