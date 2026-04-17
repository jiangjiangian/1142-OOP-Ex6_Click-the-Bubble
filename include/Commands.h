// ── Commands.h ─────────────────────────────────────────────
// Command Pattern.
//
// Encapsulates player actions as objects.
// Benefits:
//   1. InputHandler maps clicks → Command objects (decoupled from GameManager)
//   2. execute() is the single dispatch point — easy to add logging / replay
//   3. Future: keep a command history for replay / undo
//
// Each command holds only what it needs (mx,my for clicks; diff for start).
// Commands NEVER hold raw pointers to GameManager — they receive it in execute().
// ──────────────────────────────────────────────────────────
#ifndef COMMANDS_H
#define COMMANDS_H

#include "Config.h"

class GameManager;   // forward declaration

// ── Abstract base ─────────────────────────────────────────
class ICommand {
public:
    virtual ~ICommand() = default;
    virtual void execute(GameManager& gm) = 0;
};

// ── PopBubbleCommand ──────────────────────────────────────
// Issued when the player clicks in PLAYING state (not on star/menu)
class PopBubbleCommand : public ICommand {
public:
    PopBubbleCommand(float mx, float my) : mx(mx), my(my) {}
    void execute(GameManager& gm) override;
private:
    float mx, my;
};

// ── ActivatePowerupCommand ────────────────────────────────
// Issued when the player clicks the star
class ActivatePowerupCommand : public ICommand {
public:
    void execute(GameManager& gm) override;
};

// ── OpenPauseMenuCommand ──────────────────────────────────
class OpenPauseMenuCommand : public ICommand {
public:
    void execute(GameManager& gm) override;
};

// ── StartGameCommand ──────────────────────────────────────
class StartGameCommand : public ICommand {
public:
    explicit StartGameCommand(Difficulty d) : diff(d) {}
    void execute(GameManager& gm) override;
private:
    Difficulty diff;
};

// ── ResetGameCommand ─────────────────────────────────────
class ResetGameCommand : public ICommand {
public:
    explicit ResetGameCommand(bool keepDifficulty) : keep(keepDifficulty) {}
    void execute(GameManager& gm) override;
private:
    bool keep;
};

// ── ExitGameCommand ──────────────────────────────────────
class ExitGameCommand : public ICommand {
public:
    void execute(GameManager& gm) override;
};

#endif // COMMANDS_H
