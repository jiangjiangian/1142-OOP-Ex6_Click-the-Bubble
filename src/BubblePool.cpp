// ── BubblePool.cpp ─────────────────────────────────────────
#include "BubblePool.h"
#include <cmath>
#include <cstring>

// ════════════════════════════════════════════════════════════
//  Construction / Reset
// ════════════════════════════════════════════════════════════
BubblePool::BubblePool()
    : freeTop(0), activeCount(0),
      spawnTimer(0.f), hellRedAccum(0.f),
      eventTimer(0.f), slowTimer(0.f), popTimer(0.f)
{
    // Pre-populate free stack with all indices
    for (int i = 0; i < POOL_SIZE; ++i) {
        freeStack[i] = i;
        pool[i].kill();  // mark all as dead
    }
    freeTop = POOL_SIZE;
}

void BubblePool::reset() {
    for (int i = 0; i < activeCount; ++i) {
        pool[activeList[i]].kill();
        freeStack[freeTop++] = activeList[i];
    }
    activeCount   = 0;
    spawnTimer    = 0.f;
    hellRedAccum  = 0.f;
    eventTimer    = 0.f;
    slowTimer     = 0.f;
    popTimer      = 0.f;
}

// ════════════════════════════════════════════════════════════
//  Pool management (private)
// ════════════════════════════════════════════════════════════
Bubble* BubblePool::acquire() {
    if (freeTop == 0) return nullptr;  // pool exhausted
    int idx              = freeStack[--freeTop];
    activeList[activeCount++] = idx;
    return &pool[idx];
}

void BubblePool::release(int listIdx) {
    int poolIdx = activeList[listIdx];
    pool[poolIdx].kill();
    freeStack[freeTop++] = poolIdx;
    // Swap-remove from activeList (O(1), order doesn't matter)
    activeList[listIdx] = activeList[--activeCount];
}

void BubblePool::pruneInactive() {
    int i = 0;
    while (i < activeCount) {
        if (!pool[activeList[i]].isAlive())
            release(i);   // swap-remove; do NOT increment i
        else
            ++i;
    }
}

// ════════════════════════════════════════════════════════════
//  Spawn helpers
// ════════════════════════════════════════════════════════════
void BubblePool::spawnOne(BubbleType kind, const PhaseConfig& cfg,
                           bool hellMode, bool powerupActive) {
    Bubble* slot = acquire();
    if (!slot) return;
    BubbleFactory::create(*slot, kind, cfg, hellMode, powerupActive);
}

void BubblePool::triggerCleanSweep(int removeCount, float speedMult) {
    // Pop up to removeCount alive, non-popping bubbles
    int removed = 0;
    for (int i = 0; i < activeCount && removed < removeCount; ++i) {
        Bubble& b = pool[activeList[i]];
        if (b.isAlive() && !b.isPopping()) {
            b.pop();
            ++removed;
        }
    }
    // Optionally slow all remaining bubbles
    if (speedMult < 1.f) applySpeedMultiplier(speedMult);
}

// ════════════════════════════════════════════════════════════
//  Update
// ════════════════════════════════════════════════════════════
void BubblePool::update(float dt, const PhaseConfig& cfg,
                         bool hellMode, bool powerupActive) {
    int maxB = hellMode ? HELL_MAX_BUBBLES : cfg.maxBubbles;

    // ── 30s clean event ──────────────────────────────────────
    eventTimer += dt;
    if (eventTimer >= EVENT_INTERVAL) {
        eventTimer = 0.f;
        triggerCleanSweep(cfg.removeCount, EVENT_SPEED_MULT);
    }

    // ── SLOW special bubble ──────────────────────────────────
    slowTimer += dt;
    if (slowTimer >= SLOW_SPAWN_INTERVAL) {
        slowTimer = 0.f;
        spawnOne(BubbleType::SLOW, cfg, false, false);
    }

    // ── POP special bubble ───────────────────────────────────
    popTimer += dt;
    if (popTimer >= POP_SPAWN_INTERVAL) {
        popTimer = 0.f;
        spawnOne(BubbleType::POP, cfg, false, false);
    }

    // ── Normal / hell spawn ──────────────────────────────────
    if (!hellMode) {
        float interval = powerupActive ? cfg.spawnInterval * 0.5f
                                       : cfg.spawnInterval;
        spawnTimer += dt;
        if (spawnTimer >= interval && count() < maxB) {
            spawnTimer = 0.f;
            BubbleType kind = BubbleFactory::rolledKind(cfg, powerupActive);
            spawnOne(kind, cfg, false, powerupActive);
        }
    } else {
        // Hell: 3 DANGER / sec until 99% saturation
        hellRedAccum += HELL_RED_PER_SEC * dt;
        while (hellRedAccum >= 1.f && count() < maxB) {
            hellRedAccum -= 1.f;
            if (!isHellSaturated()) spawnOne(BubbleType::DANGER, cfg, true, false);
        }
        // Keep 1% normal alive
        spawnTimer += dt;
        if (spawnTimer >= 3.f) {
            spawnTimer = 0.f;
            spawnOne(BubbleType::NORMAL, cfg, true, false);
        }
    }

    // ── Update all active bubbles ────────────────────────────
    for (int i = 0; i < activeCount; ++i)
        pool[activeList[i]].update(dt);

    // ── Collision (skip in hell — too many bubbles) ──────────
    if (!hellMode && activeCount <= 80)
        checkCollisions();

    pruneInactive();
}

// ════════════════════════════════════════════════════════════
//  Draw
// ════════════════════════════════════════════════════════════
void BubblePool::draw() const {
    for (int i = 0; i < activeCount; ++i)
        pool[activeList[i]].draw();
}

// ════════════════════════════════════════════════════════════
//  Click handling — returns score delta
// ════════════════════════════════════════════════════════════
int BubblePool::handleClick(float mx, float my,
                              const PhaseConfig& cfg,
                              bool hellMode, bool powerupActive) {
    // Iterate reverse (topmost bubble = last in activeList)
    for (int i = activeCount - 1; i >= 0; --i) {
        Bubble& b = pool[activeList[i]];
        if (!b.isClicked(mx, my)) continue;

        BubbleType type = b.getType();
        int sv          = b.getScoreValue();
        b.pop();

        // Special bubble side-effects
        if (type == BubbleType::SLOW)
            applySpeedMultiplier(SLOW_SPEED_MULT);
        else if (type == BubbleType::POP)
            triggerCleanSweep(cfg.removeCount, 1.f);

        return sv;  // caller adds to score
    }
    return 0;   // miss
}

// ════════════════════════════════════════════════════════════
//  Powerup effects
// ════════════════════════════════════════════════════════════
void BubblePool::convertAllDangerToNormal() {
    static auto pickColor = [](int& sv) -> Color {
        int idx = rand() % 6;
        sv = BUBBLE_SCORES[idx];
        return NORMAL_COLORS[idx];
    };
    for (int i = 0; i < activeCount; ++i) {
        Bubble& b = pool[activeList[i]];
        if (b.getType() == BubbleType::DANGER) {
            int sv; Color c = pickColor(sv);
            b.makeNormal(c, sv);
        }
    }
}

void BubblePool::applySpeedMultiplier(float mult) {
    for (int i = 0; i < activeCount; ++i) {
        Bubble& b = pool[activeList[i]];
        b.setVelocity(b.getVX() * mult, b.getVY() * mult);
    }
}

// ════════════════════════════════════════════════════════════
//  Queries
// ════════════════════════════════════════════════════════════
int BubblePool::count() const { return activeCount; }

bool BubblePool::isHellSaturated() const {
    if (activeCount == 0) return false;
    int dangerCnt = 0;
    for (int i = 0; i < activeCount; ++i)
        if (pool[activeList[i]].getType() == BubbleType::DANGER) ++dangerCnt;
    return (float)dangerCnt / (float)activeCount >= HELL_RED_RATIO;
}

// ════════════════════════════════════════════════════════════
//  Collision — elastic + 5% boost, always capped by setVelocity
// ════════════════════════════════════════════════════════════
void BubblePool::checkCollisions() {
    for (int ai = 0; ai < activeCount; ++ai) {
        Bubble& A = pool[activeList[ai]];
        if (!A.isAlive() || A.isPopping()) continue;

        for (int aj = ai + 1; aj < activeCount; ++aj) {
            Bubble& B = pool[activeList[aj]];
            if (!B.isAlive() || B.isPopping()) continue;

            float dx   = B.getX() - A.getX();
            float dy   = B.getY() - A.getY();
            float dist = sqrtf(dx*dx + dy*dy);
            float minD = A.getRadius() + B.getRadius();
            if (dist >= minD || dist < 0.001f) continue;

            float nx  = dx/dist, ny = dy/dist;
            float rvx = B.getVX() - A.getVX();
            float rvy = B.getVY() - A.getVY();
            float dot = rvx*nx + rvy*ny;
            if (dot >= 0.f) continue;   // separating — skip

            constexpr float BOOST = 1.02f;  // 2% boost — was 5%, caused runaway cascade
            A.setVelocity((A.getVX() + dot*nx) * BOOST,
                          (A.getVY() + dot*ny) * BOOST);
            B.setVelocity((B.getVX() - dot*nx) * BOOST,
                          (B.getVY() - dot*ny) * BOOST);

            // Separate overlapping centres
            float sep = (minD - dist) * 0.5f;
            A.nudge(-nx*sep, -ny*sep);
            B.nudge( nx*sep,  ny*sep);
        }
    }
}
