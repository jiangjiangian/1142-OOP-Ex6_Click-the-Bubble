// File: Powerup.h — 管理星星道具的生成、載入與行為 (延遲載入資源)
// ── Powerup.h ──────────────────────────────────────────────
// It is DECOUPLED from BubblePool intentionally:
//   - When the star is clicked, it tells Player to activatePowerup().
//   - Player publishes PowerupActivatedEvent.
//   - GameManager subscribes and calls pool effects.
// Powerup knows nothing about BubblePool — low coupling.
// ──────────────────────────────────────────────────────────
#ifndef POWERUP_H
#define POWERUP_H

#include "Star.h"
#include "raylib.h"

class Powerup {
public:
    Powerup();
    ~Powerup();

    void init();  // Call after InitWindow() to load texture
    void update(float dt);
    void draw()   const;

    // Returns true if the star was just collected (caller notifies Player)
    bool checkClicked(float mx, float my);

    bool  isStarVisible() const { return star.isAlive();  }
    float getStarX()      const { return star.getX();     }
    float getStarY()      const { return star.getY();     }

private:
    Star      star;
    float     spawnCooldown;   // counts up toward STAR_SPAWN_INTERVAL
    bool      texLoaded;
    Texture2D starTex;
};

#endif // POWERUP_H
