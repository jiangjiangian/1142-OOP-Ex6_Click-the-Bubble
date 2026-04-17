// ── StateMachine.h ─────────────────────────────────────────
// State Pattern — manages state lifetime and transitions.
//
// Owns all three IGameState instances.
// transition() calls onExit → swap pointer → onEnter.
// No if/else on GameState enum anywhere in GameManager.
// ──────────────────────────────────────────────────────────
#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "IGameState.h"
#include "Config.h"
#include <memory>

class StateMachine {
public:
    StateMachine();

    // Register the three concrete states (called once at construction)
    void registerState(GameState id, std::unique_ptr<IGameState> state);

    // Transition: onExit current → set new → onEnter new
    void transition(GameState next, GameManager& gm);

    void handleClick(GameManager& gm, float mx, float my);
    void update     (GameManager& gm, float dt);
    void render     (const GameManager& gm) const;

    GameState currentId() const { return curId; }

private:
    static constexpr int NUM_STATES = 3; // MENU=0, PLAYING=1, PAUSED=2
    std::unique_ptr<IGameState> states[NUM_STATES];
    IGameState* current = nullptr;
    GameState   curId   = GameState::MENU;

    IGameState* lookup(GameState id) const;
};

#endif // STATEMACHINE_H
