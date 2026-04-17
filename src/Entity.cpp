// ── Entity.cpp ─────────────────────────────────────────────
#include "Entity.h"
#include <cmath>

Entity::Entity(float x, float y, float r, float vx, float vy)
    : x(x), y(y), radius(r), vx(vx), vy(vy), alive(true) {}

bool Entity::isClicked(float mx, float my) const {
    if (!alive) return false;
    float dx = mx - x, dy = my - y;
    return (dx*dx + dy*dy) <= (radius * radius);
}

// Always enforces GLOBAL_SPEED_CAP — prevents collision cascade runaway
void Entity::setVelocity(float nvx, float nvy) {
    float spd = sqrtf(nvx*nvx + nvy*nvy);
    if (spd > GLOBAL_SPEED_CAP && spd > 0.f) {
        float s = GLOBAL_SPEED_CAP / spd;
        nvx *= s; nvy *= s;
    }
    vx = nvx; vy = nvy;
}

void Entity::nudge(float dx, float dy) { x += dx; y += dy; }
void Entity::kill()                    { alive = false; }

void Entity::revive(float nx, float ny, float nr, float nvx, float nvy) {
    x = nx; y = ny; radius = nr; alive = true;
    setVelocity(nvx, nvy);  // still cap speed
}

void Entity::wrapAround() {
    if      (x < -radius)             x = SCREEN_W + radius;
    else if (x >  SCREEN_W + radius)  x = -radius;
    if      (y < -radius)             y = SCREEN_H + radius;
    else if (y >  SCREEN_H + radius)  y = -radius;
}
