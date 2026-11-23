#pragma once

#include <cmath>
#include <memory>

#include <esp_heap_caps.h>

#include "audio_route.hpp"
#include "page.hpp"

// Microphone level meter. B records a short clip into PSRAM and plays it back.
class MicPage : public Page {
public:
    const char* title() const override { return "Mic"; }
    const char* actionLabel() const override { return state_ == State::Monitor ? "rec 2s" : nullptr; }

    void enter() override {
        if (!clip_) {
            clip_.reset(static_cast<int16_t*>(
                heap_caps_malloc(kClipSamples * sizeof(int16_t), MALLOC_CAP_SPIRAM)));
        }
        audio_route::useMic();
        state_ = State::Monitor;
    }

    void exit() override {
        M5.Speaker.stop();
        audio_route::off();
    }

    void onAction() override {
        if (state_ != State::Monitor || !clip_) return;
        while (M5.Mic.isRecording()) delay(1);
        if (M5.Mic.record(clip_.get(), kClipSamples, kSampleRate)) {
            state_ = State::Recording;
            state_start_ms_ = millis();
        }
    }

    void update(M5Canvas& canvas) override {
        switch (state_) {
            case State::Monitor:
                measureLevel();
                break;
            case State::Recording:
                if (!M5.Mic.isRecording()) startPlayback();
                break;
            case State::Playing:
                if (!M5.Speaker.isPlaying()) {
                    audio_route::useMic();
                    state_ = State::Monitor;
                }
                break;
        }
        draw(canvas);
    }

private:
    enum class State { Monitor, Recording, Playing };

    struct PsramDeleter {
        void operator()(int16_t* p) const { heap_caps_free(p); }
    };

    static constexpr uint32_t kSampleRate = 16000;
    static constexpr size_t kClipSamples = kSampleRate * 2;  // 2 seconds
    static constexpr size_t kChunkSamples = 256;             // 16 ms per meter update

    void measureLevel() {
        if (!M5.Mic.record(chunk_, kChunkSamples, kSampleRate)) return;
        while (M5.Mic.isRecording()) delay(1);

        int64_t sum_sq = 0;
        int peak = 0;
        for (size_t i = 0; i < kChunkSamples; ++i) {
            const int s = chunk_[i];
            sum_sq += (int64_t)s * s;
            peak = max(peak, abs(s));
        }
        rms_ = std::sqrt((double)sum_sq / kChunkSamples);
        peak_ = peak;
    }

    void startPlayback() {
        audio_route::useSpeaker();
        M5.Speaker.setVolume(200);
        M5.Speaker.playRaw(clip_.get(), kClipSamples, kSampleRate);
        state_ = State::Playing;
        state_start_ms_ = millis();
    }

    void draw(M5Canvas& canvas) {
        const int top = layout::kHeaderHeight + 4;
        canvas.setTextColor(TFT_WHITE);
        canvas.setCursor(4, top);

        if (!clip_) {
            canvas.setTextColor(TFT_RED);
            canvas.print("PSRAM alloc failed");
            return;
        }
        if (state_ == State::Recording) {
            canvas.printf("Recording... %.1fs", (millis() - state_start_ms_) / 1000.0f);
            return;
        }
        if (state_ == State::Playing) {
            canvas.printf("Playing... %.1fs", (millis() - state_start_ms_) / 1000.0f);
            return;
        }

        // dBFS of RMS level, -90..0 mapped onto the bar
        const float dbfs = rms_ > 0 ? 20.0f * std::log10(rms_ / 32768.0f) : -90.0f;
        canvas.printf("RMS %6.0f  peak %5d\n", rms_, peak_);
        canvas.printf(" level %.1f dBFS", dbfs);

        const int bar_y = top + 24;
        const int bar_w = canvas.width() - 8;
        const int len = (int)(constrain((dbfs + 90.0f) / 90.0f, 0.0f, 1.0f) * bar_w);
        canvas.drawRect(4, bar_y, bar_w, 12, TFT_DARKGREY);
        canvas.fillRect(4, bar_y + 1, len, 10, dbfs > -20 ? TFT_RED : TFT_GREEN);

        // Waveform of the latest chunk
        const int wave_mid = bar_y + 44;
        const int step = max(1, (int)(kChunkSamples / bar_w));
        for (int x = 0; x < bar_w && (size_t)(x * step) < kChunkSamples; ++x) {
            const int y = wave_mid - chunk_[x * step] * 24 / 32768;
            canvas.drawPixel(4 + x, y, TFT_CYAN);
        }
    }

    std::unique_ptr<int16_t, PsramDeleter> clip_;
    int16_t chunk_[kChunkSamples] = {};
    State state_ = State::Monitor;
    uint32_t state_start_ms_ = 0;
    float rms_ = 0;
    int peak_ = 0;
};
