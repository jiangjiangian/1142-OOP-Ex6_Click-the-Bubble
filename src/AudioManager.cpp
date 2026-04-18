// ── AudioManager.cpp ───────────────────────────────────────
#include "AudioManager.h"
#include "Log.h"
#include <cstdio>

void AudioManager::load() {
    if (popLoaded) return;   // idempotent

    // BUG FIX: guard with IsAudioDeviceReady() so LoadSound
    // is never called before InitAudioDevice() or when it failed.
    if (!IsAudioDeviceReady()) {
        Log::warn("[AudioManager] device not ready — pop.wav skipped");
        return;
    }
    // Try several candidate relative paths to be robust against different
    // working directories (project root vs build folder).
    const char* candidates[] = {
        "resources/pop.wav",
        "../resources/pop.wav",
        "pop.wav",
        nullptr
    };
    const char* used = nullptr;
    for (int i=0; candidates[i]; ++i) {
        if (FileExists(candidates[i])) { used = candidates[i]; break; }
    }
    if (used) {
        popSound = LoadSound(used);
        SetSoundVolume(popSound, masterVol);
        popLoaded = true;
        Log::info("[AudioManager] loaded pop sound: %s", used);
    } else {
        Log::warn("[AudioManager] pop.wav not found in expected paths");
    }
}

void AudioManager::unload() {
    if (popLoaded) {
        UnloadSound(popSound);
        popLoaded = false;
    }
}

void AudioManager::playPop() const {
    if (popLoaded) PlaySound(popSound);
}

void AudioManager::setVolume(float v) {
    masterVol = v;
    if (popLoaded) SetSoundVolume(popSound, v);
}
