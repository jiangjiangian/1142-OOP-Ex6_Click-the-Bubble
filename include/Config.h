// File: Config.h — 全域常數、遊戲參數與事件/enum 定義
// ── Config.h ───────────────────────────────────────────────
#ifndef CONFIG_H
#define CONFIG_H
#include "raylib.h"

// ── Screen ─────────────────────────────────────────────────
constexpr int   SCREEN_W   = 1024;
constexpr int   SCREEN_H   = 768;
constexpr int   TARGET_FPS = 60;

// ── Bubble geometry ─────────────────────────────────────────
constexpr float RADIUS_MIN       = 18.f;
constexpr float RADIUS_MAX       = 52.f;
constexpr float GLOBAL_SPEED_CAP = 500.f;

// ── Timers ──────────────────────────────────────────────────
constexpr float EVENT_INTERVAL        = 30.f;
constexpr float SLOW_SPAWN_INTERVAL   = 15.f;
constexpr float POP_SPAWN_INTERVAL    = 25.f;
constexpr float STAR_SPAWN_INTERVAL   = 40.f;
constexpr float STAR_POWERUP_DURATION =  5.f;
constexpr float STAR_LIFETIME         =  5.f;
constexpr float STAR_RADIUS           = 36.f;

// ── Effect multipliers ──────────────────────────────────────
constexpr float EVENT_SPEED_MULT = 0.50f;
constexpr float SLOW_SPEED_MULT  = 0.75f;

// ── Hell mode ───────────────────────────────────────────────
constexpr int   HELL_SCORE_THRESHOLD = 200;
constexpr float HELL_RED_RATIO       = 0.99f;
constexpr float HELL_RED_PER_SEC     = 3.f;
constexpr int   HELL_PENALTY         = 30;
constexpr int   HELL_MAX_BUBBLES     = 50;

// ── Phases ──────────────────────────────────────────────────
constexpr int PHASE2_SCORE = 50;
constexpr int PHASE3_SCORE = 100;

// ── Bubble color / score table ──────────────────────────────
constexpr int BUBBLE_SCORES[6] = {1, 2, 3, 5, 7, 10};
inline const Color NORMAL_COLORS[6] = {
    {255,230, 50,220}, { 80,220, 80,220}, { 50,210,220,220},
    { 80,130,255,220}, {255,155, 50,220}, {200, 80,255,220}
};
// DANGER replaces RED_BUBBLE to avoid raylib RED macro collision
inline const Color DANGER_COLOR = {230, 30,  30, 230};
inline const Color SLOW_COLOR   = { 30,200, 255, 255};
inline const Color POP_COLOR    = {255, 50, 255, 255};

// ── Enums ───────────────────────────────────────────────────
enum class GameState  { MENU, PLAYING, PAUSED };
enum class Difficulty { EASY, MEDIUM, HARD };
// DANGER (not RED) avoids raylib macro clash
enum class BubbleType { NORMAL, DANGER, SLOW, POP };

// ────────────────────────────────────────────────────────────
// Observer Pattern — event structs published through EventBus
// Subscribers receive const refs; no includes needed here.
// ────────────────────────────────────────────────────────────
struct ScoreChangedEvent    { int newScore; int delta;             };
struct BubblePoppedEvent    { BubbleType type; int scoreValue;     };
struct PowerupActivatedEvent{                                      };
struct PowerupExpiredEvent  {                                      };
struct HellModeStartedEvent {                                      };
struct PhaseChangedEvent    { int phase; /* 1/2/3 */              };

// ────────────────────────────────────────────────────────────
// Command Pattern — enum for InputHandler dispatch
// ────────────────────────────────────────────────────────────
enum class CommandType {
    POP_BUBBLE,
    ACTIVATE_POWERUP,
    OPEN_PAUSE_MENU,
    CLOSE_PAUSE_MENU,
    RESTART_GAME,
    CHANGE_MODE,
    EXIT_GAME,
    SHOW_HOW_TO_PLAY,
    NONE
};

// ── Phase config (pure function, no side effects) ────────────
struct PhaseConfig {
    float spawnInterval;
    float speedMin;
    float speedMax;
    float dangerRatio;
    int   dangerPenalty;
    int   maxBubbles;
    int   removeCount;
};

inline PhaseConfig getPhaseConfig(int score, Difficulty diff) {
    float sm, im;
    switch (diff) {
        case Difficulty::EASY: sm=0.6f; im=1.4f; break;
        case Difficulty::HARD: sm=1.3f; im=0.7f; break;
        default:               sm=1.0f; im=1.0f; break;
    }
    PhaseConfig c;
    if      (score < PHASE2_SCORE) c = {1.2f,30.f, 80.f,0.10f, 5,15, 1};
    else if (score < PHASE3_SCORE) c = {0.8f,50.f,120.f,0.20f,10,20, 5};
    else                           c = {0.5f,70.f,160.f,0.35f,20,30,10};
    c.spawnInterval *= im;
    c.speedMin      *= sm;
    c.speedMax      *= sm;
    return c;
}
#endif // CONFIG_H
