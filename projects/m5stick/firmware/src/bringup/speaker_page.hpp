#pragma once

#include "audio_route.hpp"
#include "page.hpp"

// Speaker (ES8311 + amp). B plays a C-major scale.
class SpeakerPage : public Page {
public:
    const char* title() const override { return "Speaker"; }
    const char* actionLabel() const override { return "play"; }

    void enter() override {
        audio_route::useSpeaker();
        M5.Speaker.setVolume(kVolume);
        note_index_ = kNoteCount;  // idle
    }

    void exit() override {
        M5.Speaker.stop();
        audio_route::off();
    }

    void onAction() override { note_index_ = 0; }

    void update(M5Canvas& canvas) override {
        if (note_index_ < kNoteCount && !M5.Speaker.isPlaying()) {
            M5.Speaker.tone(kNotesHz[note_index_], kNoteMs);
            ++note_index_;
        }

        canvas.setTextColor(TFT_WHITE);
        canvas.setCursor(4, layout::kHeaderHeight + 4);
        canvas.printf("Volume : %u / 255\n", (unsigned)M5.Speaker.getVolume());
        canvas.printf(" Status : %s\n", M5.Speaker.isPlaying() ? "playing" : "idle");
        if (note_index_ > 0 && note_index_ <= kNoteCount) {
            canvas.printf(" Note   : %d Hz\n", (int)kNotesHz[note_index_ - 1]);
        }
    }

private:
    static constexpr uint8_t kVolume = 128;
    static constexpr uint32_t kNoteMs = 200;
    static constexpr float kNotesHz[] = {523.3f, 587.3f, 659.3f, 698.5f, 784.0f, 880.0f, 987.8f, 1046.5f};
    static constexpr size_t kNoteCount = sizeof(kNotesHz) / sizeof(kNotesHz[0]);

    size_t note_index_ = kNoteCount;
};
