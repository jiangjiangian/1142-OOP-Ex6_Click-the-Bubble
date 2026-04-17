// ── InputHandler.h ─────────────────────────────────────────
// Command Pattern — input mapping layer.
//
// InputHandler translates low-level mouse events into Command objects.
// It knows about click coordinates and the current GameState, but
// NEVER touches BubblePool, Player, or rendering directly.
//
// GameManager calls processClick() each frame when a click fires.
// The returned ICommand* is executed immediately; ownership stays here
// (static storage — no heap allocation for simple commands).
//
// For a game recording / replay system, you would store the returned
// commands in a history buffer before calling execute().
// ──────────────────────────────────────────────────────────
#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include "Commands.h"
#include "Config.h"
#include <memory>

class InputHandler {
public:
    // Returns a command for the given click, or nullptr if nothing matched.
    // caller: cmd->execute(gm) then optionally push to history.
    std::unique_ptr<ICommand> processClick(float mx, float my,
                                            GameState currentState,
                                            bool starVisible,
                                            float starX, float starY) const;

private:
    bool clickedMenuButton(float mx, float my) const;
    bool clickedStar(float mx, float my,
                     float starX, float starY) const;
};

#endif // INPUTHANDLER_H
