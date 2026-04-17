// ── AudioManager.h ─────────────────────────────────────────
// Singleton Pattern for audio.
// Responsibilities: load sounds, expose play methods.
// No other class holds a Sound handle — they call AudioManager::get().
//
// Why Singleton here?
//   Sound resources are global, exactly one device exists,
//   and callers (GameStates, BubblePool) must not carry
//   Sound references around — that creates coupling.
// ──────────────────────────────────────────────────────────
#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include "raylib.h"

class AudioManager {
public:
    static AudioManager& get() {
        static AudioManager inst;
        return inst;
    }

    // Delete copy / move (Singleton)
    AudioManager(const AudioManager&)            = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    // Call once after InitAudioDevice() — safe to call again (idempotent)
    void load();
    void unload();

    void playPop()  const;
    void setVolume(float v);   // 0.0 – 1.0

private:
    AudioManager() = default;

    Sound popSound   {};
    bool  popLoaded  = false;
    float masterVol  = 1.f;
};

#endif // AUDIOMANAGER_H
