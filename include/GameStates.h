// File: GameStates.h — 三個具體 State 的宣告 (Menu/Playing/Paused)
// ── GameStates.h ───────────────────────────────────────────
//   MenuState    — difficulty select + how-to-play screen
//   PlayingState — all gameplay (phases 1-3 + hell via hellMode flag)
//   PausedState  — pause overlay on top of frozen game world
// ──────────────────────────────────────────────────────────
#ifndef GAMESTATES_H
#define GAMESTATES_H

#include "IGameState.h"

// ── MenuState ───────────────────────────────────────────────
class MenuState : public IGameState {
public:
    void onEnter   (GameManager& gm)                override;
    void onExit    (GameManager& gm)                override;
    void handleClick(GameManager& gm,
                     float mx, float my)            override;
    void update    (GameManager& gm, float dt)      override;
    void render    (const GameManager& gm)    const override;

private:
    bool showingHowToPlay = false;
    void drawDifficultySelect(const GameManager& gm) const;
    void drawHowToPlay()                             const;
    void drawRules(int x, int y)                     const;
};

// ── PlayingState ─────────────────────────────────────────────
class PlayingState : public IGameState {
public:
    void onEnter   (GameManager& gm)                override;
    void onExit    (GameManager& gm)                override;
    void handleClick(GameManager& gm,
                     float mx, float my)            override;
    void update    (GameManager& gm, float dt)      override;
    void render    (const GameManager& gm)    const override;
};

// ── PausedState ──────────────────────────────────────────────
class PausedState : public IGameState {
public:
    void onEnter   (GameManager& gm)                override;
    void onExit    (GameManager& gm)                override;
    void handleClick(GameManager& gm,
                     float mx, float my)            override;
    void update    (GameManager& gm, float dt)      override {}  // frozen
    void render    (const GameManager& gm)    const override;

private:
    bool showingHowToPlay = false;
    void drawRules(int x, int y) const;
};

#endif // GAMESTATES_H
