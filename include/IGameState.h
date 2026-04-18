// File: IGameState.h — 抽象 State 介面 (onEnter/onExit/update/render)
// ── IGameState.h ───────────────────────────────────────────
//
// Each GameState (Menu, Playing, Paused) encapsulates its own:
//   - input handling
//   - update logic
//   - rendering
//   - enter/exit transitions
//
// GameManager is forward-declared to avoid circular includes.
// Concrete states include GameManager.h only in their .cpp.
//
// WHY this eliminates bad design:
//   BEFORE: GameEngine had if(state==PLAYING){...} if(state==PAUSED){...}
//           every method had the same switch — Rigidity + God Class
//   AFTER:  Each state IS the logic. Adding a new state = new class only.
// ──────────────────────────────────────────────────────────
#ifndef IGAMESTATE_H
#define IGAMESTATE_H

class GameManager;   // forward declaration — breaks circular dependency

class IGameState {
public:
    virtual ~IGameState() = default;

    virtual void onEnter (GameManager& gm)               = 0;
    virtual void onExit  (GameManager& gm)               = 0;
    virtual void handleClick(GameManager& gm,
                              float mx, float my)        = 0;
    virtual void update  (GameManager& gm, float dt)     = 0;
    virtual void render  (const GameManager& gm)   const = 0;
};

#endif // IGAMESTATE_H
