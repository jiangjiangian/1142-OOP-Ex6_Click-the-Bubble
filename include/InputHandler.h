// ── InputHandler.h ─────────────────────────────────────────
// Command Pattern — input mapping layer.
//
// InputHandler translates low-level mouse events into Command objects.
// It knows about click coordinates and the current GameState, but
// NEVER touches BubblePool, Player, or rendering directly.
//
// GameManager calls processClick() each frame when a click fires.
// The returned command is returned as a std::unique_ptr<ICommand>.
// The caller takes ownership, calls execute() and may optionally
// store the command for replay/history by moving the pointer.
//
// For a game recording / replay system, move the returned command into
// a history buffer before calling execute() so it can be replayed later.
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
