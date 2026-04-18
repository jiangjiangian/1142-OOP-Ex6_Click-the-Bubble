// File: Player.h — 玩家狀態（分數、道具計時與事件發佈）
// ── Player.h ───────────────────────────────────────────────
// High cohesion: only player-state logic here.
// Low coupling: communicates outward ONLY via EventBus events,
//   never holds references to BubblePool, UIManager, etc.
//
// Events published:
//   ScoreChangedEvent    — on addScore()
//   PowerupActivatedEvent— on activatePowerup()
//   PowerupExpiredEvent  — inside update() when timer reaches 0
// ──────────────────────────────────────────────────────────
#ifndef PLAYER_H
#define PLAYER_H

class Player {
public:
    Player();

    void update(float dt);
    void addScore(int delta);
    void activatePowerup();
    void reset();

    int   getScore()        const { return score;         }
    bool  isPowerupActive() const { return powerupActive; }
    float getPowerupTimer() const { return powerupTimer;  }

private:
    int   score;
    bool  powerupActive;
    float powerupTimer;

    void deactivatePowerup();  // internal — publishes PowerupExpiredEvent
};

#endif // PLAYER_H
