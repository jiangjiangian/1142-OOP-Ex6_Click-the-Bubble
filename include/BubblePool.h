// ── BubblePool.h ───────────────────────────────────────────
// Object Pool Pattern.
//
// Pre-allocates POOL_SIZE Bubble objects.  During gameplay, acquire()
// takes a slot from the free stack and inits it; release() marks it
// available again.  Zero heap allocation after construction.
//
// Also owns the World-level spawn timers and collision logic that were
// previously scattered in World::update().  High cohesion: everything
// about bubble lifecycle lives here.
// ──────────────────────────────────────────────────────────
#ifndef BUBBLEPOOL_H
#define BUBBLEPOOL_H

#include "Bubble.h"
#include "BubbleFactory.h"
#include "Config.h"

class BubblePool {
public:
    static constexpr int POOL_SIZE = 200;

    BubblePool();

    // ── Per-frame ───────────────────────────────────────────
    void update(float dt, const PhaseConfig& cfg,
                bool hellMode, bool powerupActive);
    void draw() const;

    // ── Input — returns score delta (0 = miss) ──────────────
    int  handleClick(float mx, float my,
                     const PhaseConfig& cfg, bool hellMode, bool powerupActive);

    // ── Powerup effects ─────────────────────────────────────
    void convertAllDangerToNormal();
    void applySpeedMultiplier(float mult);

    // ── State queries ────────────────────────────────────────
    int  count()          const;
    bool isHellSaturated()const;

    // ── Lifecycle ────────────────────────────────────────────
    void reset();   // returns all slots to free list

private:
    // ── Pool storage ─────────────────────────────────────────
    Bubble pool[POOL_SIZE];
    int    freeStack[POOL_SIZE];   // indices of available slots
    int    freeTop;                // points one past last free entry
    int    activeList[POOL_SIZE];  // indices of live bubbles
    int    activeCount;

    // ── Spawn timers ─────────────────────────────────────────
    float spawnTimer;
    float hellRedAccum;
    float eventTimer;
    float slowTimer;
    float popTimer;

    // ── Pool management ──────────────────────────────────────
    Bubble* acquire();           // pop from freeStack, add to activeList
    void    release(int listIdx);// mark slot free, remove from activeList
    void    pruneInactive();     // bulk release dead bubbles

    // ── Spawn helpers ─────────────────────────────────────────
    void spawnOne(BubbleType kind, const PhaseConfig& cfg,
                  bool hellMode, bool powerupActive);
    void triggerCleanSweep(int count, float speedMult);

    // ── Physics ───────────────────────────────────────────────
    void checkCollisions();   // elastic + 5% boost, skipped in hell
};

#endif // BUBBLEPOOL_H
