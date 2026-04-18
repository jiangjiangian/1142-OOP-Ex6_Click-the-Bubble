// File: BubbleFactory.h — 工廠 (Bubble 建構邏輯)，封裝顏色/分數/位置計算
// ── BubbleFactory.h ────────────────────────────────────────
// Centralises bubble construction logic.
// Eliminates the Long Parameter List smell:
//   BEFORE: spawnBubble(bool forceRed, const Player&, PhaseConfig, bool hell)
//   AFTER:  BubbleFactory::create(kind, cfg)  →  Bubble&  (pool slot)
//
// BubblePool calls the factory; it never knows the color table or score
// values. Those belong here.
// ──────────────────────────────────────────────────────────
#ifndef BUBBLEFACTORY_H
#define BUBBLEFACTORY_H

#include "Bubble.h"
#include "Config.h"

class BubbleFactory {
public:
    // Fills an existing (pool-owned) Bubble slot with new values.
    // kind         — NORMAL, DANGER, SLOW, POP
    // cfg          — current phase config (speed range, penalty)
    // hellMode     — adjusts DANGER penalty to HELL_PENALTY
    // powerupActive— doubles spawn speed for new bubbles
    static void create(Bubble& slot, BubbleType kind,
                       const PhaseConfig& cfg,
                       bool hellMode       = false,
                       bool powerupActive  = false);

    // Convenience: pick NORMAL or DANGER based on cfg.dangerRatio
    // Returns the chosen kind so caller can record it
    static BubbleType rolledKind(const PhaseConfig& cfg, bool powerupActive);

private:
    static float  randF(float lo, float hi);
    static void   spawnEdge(float r, float& outX, float& outY);
    static Color  randomNormalColor(int& outScore);
};

#endif // BUBBLEFACTORY_H
