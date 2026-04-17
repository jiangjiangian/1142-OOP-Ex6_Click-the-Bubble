// ── Star.cpp ───────────────────────────────────────────────
#include "Star.h"
#include <cstdlib>
#include <cmath>
#include <cstdio>

static float rF(float lo, float hi) {
    return lo + (float)rand() / (float)RAND_MAX * (hi - lo);
}

void Star::spawn() {
    float spd   = rF(STAR_SPEED_RANGE_LO, STAR_SPEED_RANGE_HI);
    float angle = rF(0.f, 2.f * PI);
    revive(rF(STAR_RADIUS + 20.f, SCREEN_W - STAR_RADIUS - 20.f),
           rF(STAR_RADIUS + 60.f, SCREEN_H - STAR_RADIUS - 20.f),
           STAR_RADIUS,
           cosf(angle) * spd, sinf(angle) * spd);
    lifetime = STAR_LIFETIME;
}

void Star::update(float dt) {
    if (!alive) return;
    lifetime -= dt;
    if (lifetime <= 0.f) { kill(); return; }

    x += vx * dt;
    y += vy * dt;
    wrapAround();  // Entity::wrapAround uses SCREEN_W/H from Config
}

void Star::draw() const {
    if (!alive) return;
    float t     = (float)GetTime();
    float pulse = 1.f + .12f * sinf(t * 3.5f);
    float r     = radius * pulse;
    int   cx    = (int)x, cy = (int)y;

    DrawCircle(cx, cy, r,       {255, 215,  0, 210});
    DrawCircle(cx, cy, r * .6f, {255, 245, 80, 240});

    const char* s = "STAR";
    int tw = MeasureText(s, 14);
    DrawText(s, cx - tw/2, cy - 7, 14, {80, 60, 0, 255});

    // Animated glow rings
    unsigned char ra = (unsigned char)(100 + 100 * sinf(t * 4.f));
    DrawCircleLines(cx, cy, r + 5.f,  {255, 215, 0, ra});
    DrawCircleLines(cx, cy, r + 10.f, {255, 215, 0, (unsigned char)(ra / 3)});

    // Countdown arc — shrinks clockwise as lifetime drains
    float ratio = getLifetimeRatio();
    if (ratio > 0.f)
        DrawCircleSector({x, y}, r + 14.f,
                         -90.f, -90.f + 360.f * ratio,
                         30, {255, 255, 255, 60});

    // Remaining seconds
    char buf[8];
    snprintf(buf, sizeof(buf), "%.1fs", lifetime);
    int bw = MeasureText(buf, 13);
    DrawText(buf, cx - bw/2, cy + (int)(r + 17), 13, GOLD);
}

float Star::getLifetimeRatio() const {
    return alive ? (lifetime / STAR_LIFETIME) : 0.f;
}
