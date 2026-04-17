// ── BubbleFactory.cpp ──────────────────────────────────────
#include "BubbleFactory.h"
#include <cstdlib>
#include <cmath>

float BubbleFactory::randF(float lo, float hi) {
    return lo + (float)rand() / (float)RAND_MAX * (hi - lo);
}

void BubbleFactory::spawnEdge(float r, float& outX, float& outY) {
    switch (rand() % 4) {
        case 0: outX = randF(r, SCREEN_W-r); outY = -r;                    break; // top
        case 1: outX = randF(r, SCREEN_W-r); outY = (float)SCREEN_H + r;   break; // bottom
        case 2: outX = -r;                   outY = randF(r, SCREEN_H-r);  break; // left
        default:outX = (float)SCREEN_W + r;  outY = randF(r, SCREEN_H-r);  break; // right
    }
}

Color BubbleFactory::randomNormalColor(int& outScore) {
    int idx  = rand() % 6;
    outScore = BUBBLE_SCORES[idx];
    return NORMAL_COLORS[idx];
}

BubbleType BubbleFactory::rolledKind(const PhaseConfig& cfg, bool powerupActive) {
    if (powerupActive) return BubbleType::NORMAL;  // powerup: only normals
    return (randF(0.f,1.f) < cfg.dangerRatio) ? BubbleType::DANGER : BubbleType::NORMAL;
}

// ── Main factory method ──────────────────────────────────────
// Fills an existing Bubble& (pool slot) — no heap allocation
void BubbleFactory::create(Bubble& slot, BubbleType kind,
                            const PhaseConfig& cfg,
                            bool hellMode, bool powerupActive) {
    float r = randF(RADIUS_MIN, RADIUS_MAX);
    float x, y;
    spawnEdge(r, x, y);

    float spd   = randF(cfg.speedMin, cfg.speedMax);
    if (powerupActive) spd *= 2.f;

    float angle = randF(0.f, 2.f * PI);
    float vx    = cosf(angle) * spd;
    float vy    = sinf(angle) * spd;

    switch (kind) {
        case BubbleType::DANGER: {
            int penalty = hellMode ? HELL_PENALTY : cfg.dangerPenalty;
            slot.init(x, y, r, DANGER_COLOR, vx, vy, BubbleType::DANGER, -penalty);
            break;
        }
        case BubbleType::SLOW:
            // SLOW bubbles spawn at a fixed internal position, not from edges
            slot.init(randF(r, SCREEN_W-r), randF(r, SCREEN_H-r),
                      30.f, SLOW_COLOR, 60.f, 60.f, BubbleType::SLOW, 0);
            break;
        case BubbleType::POP:
            slot.init(randF(r, SCREEN_W-r), randF(r, SCREEN_H-r),
                      30.f, POP_COLOR,  60.f, 60.f, BubbleType::POP, 0);
            break;
        default: {  // NORMAL
            int sv; Color c = randomNormalColor(sv);
            slot.init(x, y, r, c, vx, vy, BubbleType::NORMAL, sv);
            break;
        }
    }
}
