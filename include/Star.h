// ── Star.h ─────────────────────────────────────────────────
// Star is a power-up collectible that drifts across the screen.
// Lifetime: STAR_LIFETIME seconds, then auto-kills itself.
// Rendering shows a countdown arc so the player sees urgency.
// ──────────────────────────────────────────────────────────
#ifndef STAR_H
#define STAR_H

#include "Entity.h"

class Star : public Entity {
public:
    Star() = default;

    // Spawn at random position with random velocity
    void spawn();

    void update(float dt)  override;
    void draw()      const override;

    float getLifetimeRatio() const;   // [0,1] remaining fraction

private:
    float lifetime = 0.f;
    static constexpr float STAR_SPEED_RANGE_LO = 80.f;
    static constexpr float STAR_SPEED_RANGE_HI = 160.f;
};

#endif // STAR_H
