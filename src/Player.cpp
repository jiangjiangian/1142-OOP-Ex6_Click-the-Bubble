// ── Player.cpp ─────────────────────────────────────────────
#include "Player.h"
#include "Config.h"
#include "EventBus.h"

Player::Player() : score(0), powerupActive(false), powerupTimer(0.f) {}

void Player::update(float dt) {
    if (!powerupActive) return;
    powerupTimer -= dt;
    if (powerupTimer <= 0.f) deactivatePowerup();
}

void Player::addScore(int delta) {
    int prev = score;
    score += delta;
    if (score < 0) score = 0;

    // Observer: broadcast score change — HUD and PhaseManager subscribe
    EventBus::get().publish(ScoreChangedEvent{score, score - prev});

    // Detect phase transitions and publish
    auto phase = [](int s) {
        if (s >= PHASE3_SCORE) return 3;
        if (s >= PHASE2_SCORE) return 2;
        return 1;
    };
    if (phase(score) != phase(prev))
        EventBus::get().publish(PhaseChangedEvent{phase(score)});

    // Hell mode trigger
    if (prev < HELL_SCORE_THRESHOLD && score >= HELL_SCORE_THRESHOLD)
        EventBus::get().publish(HellModeStartedEvent{});
}

void Player::activatePowerup() {
    powerupActive = true;
    powerupTimer  = STAR_POWERUP_DURATION;
    EventBus::get().publish(PowerupActivatedEvent{});
}

void Player::deactivatePowerup() {
    powerupActive = false;
    powerupTimer  = 0.f;
    // Observer: GameManager subscribes and clears ALL bubbles as bonus effect
    EventBus::get().publish(PowerupExpiredEvent{});
}

void Player::reset() {
    score         = 0;
    powerupActive = false;
    powerupTimer  = 0.f;
}
