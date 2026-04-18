// File: GameManager.h — 遊戲總控 (orchestrator)；擁有各子系統並管理主迴圈
// ── GameManager.h ──────────────────────────────────────────
//
// GameManager owns:
//   - All subsystem instances (Player, BubblePool, Powerup, …)
//   - StateMachine (State Pattern)
//   - EventBus subscriptions wired in the constructor
//   - InputHandler (Command Pattern)
//
// Design note — member visibility:
//   State objects (PlayingState, PausedState, MenuState) need direct access
//   to pool/player/etc. to call their methods.  Rather than expose 20+ getters,
//   those classes are declared friends so they can read public members.
//   All mutating paths still go through the public API (startGame, resetGame).
//
// Coupling map:
//   GameManager → knows all subsystems (unavoidable for an orchestrator)
//   Subsystems  → know ONLY EventBus + their direct dependencies
//   States      → know GameManager (friend), call subsystem methods
// ──────────────────────────────────────────────────────────
#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "Player.h"
#include "PhaseManager.h"
#include "BubblePool.h"
#include "Powerup.h"
#include "UIManager.h"
#include "AudioManager.h"
#include "StateMachine.h"
#include "InputHandler.h"
#include "EventBus.h"
#include "raylib.h"

class GameManager {
public:
    // ── Singleton ────────────────────────────────────────────
    static GameManager& get() {
        static GameManager inst;
        return inst;
    }
    GameManager(const GameManager&)            = delete;
    GameManager& operator=(const GameManager&) = delete;

    // ── Main loop API ────────────────────────────────────────
    void run();   // owns the raylib window + game loop

    // ── Public mutation (called by Commands and States) ──────
    void startGame(Difficulty d);
    void resetGame(bool keepDifficulty);

    // ── Background colour for ClearBackground ───────────────
    Color getBackgroundColor() const;

    bool exitFlag = false;

    // ── Subsystems (public for State friend access) ──────────
    // All are value members — no raw heap ownership.
    Player       player;
    PhaseManager phaseManager;
    BubblePool   bubblePool;
    Powerup      powerup;
    UIManager    ui;
    StateMachine stateMachine;
    InputHandler inputHandler;

private:
    GameManager();

    // ── EventBus subscriptions ───────────────────────────────
    void wireEvents();

    // ── Per-frame ────────────────────────────────────────────
    void update(float dt);
    void draw()  const;
    void handleInput();
};

#endif // GAMEMANAGER_H
