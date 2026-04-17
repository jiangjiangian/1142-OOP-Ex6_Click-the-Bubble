// ── Powerup.cpp ────────────────────────────────────────────
#include "Powerup.h"
#include <cmath>

Powerup::Powerup() : spawnCooldown(0.f), texLoaded(false) {
    // DO NOT load texture here - window not yet initialized!
    // Texture loading happens in GameManager after InitWindow()
}

void Powerup::init() {
    if (FileExists("resources/star.png")) {
        starTex   = LoadTexture("resources/star.png");
        texLoaded = true;
    }
}

Powerup::~Powerup() {
    if (texLoaded) UnloadTexture(starTex);
}

void Powerup::update(float dt) {
    if (star.isAlive()) {
        star.update(dt);
        // Star auto-kills itself in update() when lifetime expires
    } else {
        spawnCooldown += dt;
        if (spawnCooldown >= STAR_SPAWN_INTERVAL) {
            spawnCooldown = 0.f;
            star.spawn();
        }
    }
}

void Powerup::draw() const {
    if (!star.isAlive()) return;

    if (texLoaded && starTex.width > 0) {
        // Draw user-provided texture (e.g. the bunny image) scaled to STAR_RADIUS*2
        float scale = (STAR_RADIUS * 2.f) / (float)starTex.width;
        DrawTextureEx(starTex,
                      {star.getX() - STAR_RADIUS, star.getY() - STAR_RADIUS},
                      0.f, scale, WHITE);
        // Still draw the glow rings on top for visibility
        float t  = (float)GetTime();
        unsigned char ra = (unsigned char)(100 + 100 * sinf(t * 4.f));
        DrawCircleLines((int)star.getX(), (int)star.getY(),
                        STAR_RADIUS + 8.f, {255, 215, 0, ra});
    } else {
        star.draw();  // fallback golden circle
    }
}

bool Powerup::checkClicked(float mx, float my) {
    if (!star.isAlive()) return false;
    if (star.isClicked(mx, my)) {
        star.kill();
        spawnCooldown = 0.f;  // reset cooldown on collection
        return true;
    }
    return false;
}
