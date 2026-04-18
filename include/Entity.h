// File: Entity.h — `Entity` 基底類別（位置/速度/生命週期與共用行為）
// ── Entity.h ───────────────────────────────────────────────
// Provides: position/velocity (Transform), lifecycle (alive/kill),
// and virtual dispatch for update/draw/isClicked.
//
// CHANGE from original:
//   update(float dt) — screenW/H removed; subclasses use Config constants directly.
//   revive() — added for Object Pool reuse without heap allocation.
// ──────────────────────────────────────────────────────────
#ifndef ENTITY_H
#define ENTITY_H

#include "Config.h"
#include "raylib.h"

class Entity {
public:
    Entity() = default;
    Entity(float x, float y, float r, float vx, float vy);
    virtual ~Entity() = default;

    virtual void update(float dt) = 0;
    virtual void draw()  const    = 0;
    virtual bool isClicked(float mx, float my) const;

    // Getters
    float getX()      const { return x;      }
    float getY()      const { return y;      }
    float getRadius() const { return radius; }
    float getVX()     const { return vx;     }
    float getVY()     const { return vy;     }
    bool  isAlive()   const { return alive;  }

    // Mutators — setVelocity enforces GLOBAL_SPEED_CAP
    void setVelocity(float nvx, float nvy);
    void nudge(float dx, float dy);
    void kill();

    // For Object Pool: reset all fields without deallocating
    void revive(float nx, float ny, float nr, float nvx, float nvy);

protected:
    float x = 0, y = 0;
    float radius = 1.f;
    float vx = 0, vy = 0;
    bool  alive = false;

    // Wrap-around uses SCREEN_W / SCREEN_H from Config (no parameter needed)
    void wrapAround();
};

#endif // ENTITY_H
