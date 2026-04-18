// File: Bubble.h — `Bubble` 類別介面 (組合元件: Entity/RenderComp/MetaComp)
// ── Bubble.h ───────────────────────────────────────────────
//
//  Entity (base)   = Transform component  (x,y,vx,vy,radius,alive)
//  RenderComp      = color, pop animation
//  MetaComp        = game-logic data (BubbleType, scoreValue)
//
// Why components?
//   RenderComp knows nothing about physics → easy to swap renderer.
//   MetaComp  knows nothing about drawing  → clean score/type queries.
//   Entity    owns all position math       → one place for wrap/cap.
//
// Collider logic (overlap test & response) is handled statically in
// BubblePool — it's a pool-level responsibility, not per-bubble.
// ──────────────────────────────────────────────────────────
#ifndef BUBBLE_H
#define BUBBLE_H

#include "Entity.h"
#include "Config.h"

class Bubble : public Entity {
public:
    // ── RenderComp — visual state & animation ───────────────
    struct RenderComp {
        Color color     = {255,255,255,255};
        float popScale  = 1.f;
        bool  popping   = false;

        // Advances pop animation; returns false when animation finishes
        bool  tickPop(float dt);
        void  draw(float x, float y, float radius,
                   BubbleType type, int scoreValue) const;
        void  reset(Color c);
    };

    // ── MetaComp — game-logic identity ─────────────────────
    struct MetaComp {
        BubbleType type       = BubbleType::NORMAL;
        int        scoreValue = 0;

        void reset(BubbleType t, int sv);
    };

    // ── Public API ──────────────────────────────────────────
    Bubble() = default;  // default ctor required for BubblePool array

    // init() replaces constructor — used by BubblePool for reuse
    void init(float x, float y, float radius, Color color,
              float vx, float vy, BubbleType type, int scoreValue);

    void update(float dt)                    override;
    void draw()                        const override;
    bool isClicked(float mx, float my) const override;

    // Queries
    BubbleType getType()       const { return meta.type;       }
    int        getScoreValue() const { return meta.scoreValue; }
    bool       isPopping()     const { return rend.popping;    }

    // Mutations
    void pop();                            // trigger pop animation
    void makeNormal(Color c, int sv);      // powerup: convert DANGER→NORMAL
    void boostSpeed(float factor);         // still caps at GLOBAL_SPEED_CAP

private:
    RenderComp rend;
    MetaComp   meta;

    static constexpr float POP_SPEED = 5.f;
};

#endif // BUBBLE_H
