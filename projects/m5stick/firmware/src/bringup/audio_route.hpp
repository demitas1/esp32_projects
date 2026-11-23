#pragma once

#include <M5Unified.h>

// The speaker and microphone share the ES8311 codec and I2S bus,
// so only one of them can run at a time.
namespace audio_route {

inline void useSpeaker() {
    if (M5.Mic.isRunning()) M5.Mic.end();
    if (!M5.Speaker.isRunning()) M5.Speaker.begin();
}

inline void useMic() {
    if (M5.Speaker.isRunning()) M5.Speaker.end();
    if (!M5.Mic.isRunning()) M5.Mic.begin();
}

inline void off() {
    if (M5.Speaker.isRunning()) M5.Speaker.end();
    if (M5.Mic.isRunning()) M5.Mic.end();
}

}  // namespace audio_route
