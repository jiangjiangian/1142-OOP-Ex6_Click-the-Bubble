// File: PhaseManager.h — 依得分與難度計算階段參數的輔助工具
// ── PhaseManager.h ─────────────────────────────────────────
// the chosen difficulty.  It never renders, never spawns, never
// touches EventBus — it only answers "what config applies now?"
//
// Header-only: all methods are trivially short.
// ──────────────────────────────────────────────────────────
#ifndef PHASEMANAGER_H
#define PHASEMANAGER_H

#include "Config.h"
#include <cstring>

class PhaseManager {
public:
    explicit PhaseManager(Difficulty d = Difficulty::MEDIUM) : diff(d) {}

    void        setDifficulty(Difficulty d) { diff = d; }
    Difficulty  getDifficulty()       const { return diff; }

    PhaseConfig getConfig(int score)  const {
        return getPhaseConfig(score, diff);
    }

    bool        isHell(int score)     const {
        return score >= HELL_SCORE_THRESHOLD;
    }

    int         getPhase(int score)   const {
        if (score >= PHASE3_SCORE) return 3;
        if (score >= PHASE2_SCORE) return 2;
        return 1;
    }

    const char* getModeName(int score) const {
        if (isHell(score))             return "HELL";
        if (score >= PHASE3_SCORE)     return "EXTREME";
        if (score >= PHASE2_SCORE)     return "INTENSE";
        switch (diff) {
            case Difficulty::EASY: return "EASY";
            case Difficulty::HARD: return "HARD";
            default:               return "MEDIUM";
        }
    }

private:
    Difficulty diff;
};

#endif // PHASEMANAGER_H
