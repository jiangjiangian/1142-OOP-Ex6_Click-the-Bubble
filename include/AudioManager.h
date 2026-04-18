// File: AudioManager.h — 音效管理器 (Singleton), 載入/播放音效
// ── AudioManager.h ─────────────────────────────────────────
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
