// ── InputHandler.cpp ───────────────────────────────────────
#include "InputHandler.h"
#include "UIManager.h"
#include <cmath>

bool InputHandler::clickedMenuButton(float mx, float my) const {
    return UIManager::hitRect(mx, my,
        {UIManager::MENU_BTN_X, UIManager::MENU_BTN_Y,
         UIManager::MENU_BTN_W, UIManager::MENU_BTN_H});
}

bool InputHandler::clickedStar(float mx, float my,
                                float sx, float sy) const {
    float dx = mx - sx, dy = my - sy;
    return (dx*dx + dy*dy) <= (STAR_RADIUS * STAR_RADIUS);
}

std::unique_ptr<ICommand>
InputHandler::processClick(float mx, float my,
                             GameState state,
                             bool starVisible,
                             float starX, float starY) const {
    // Only PLAYING state routes through InputHandler.
    // MENU and PAUSED clicks are handled directly in their State's handleClick().
    if (state != GameState::PLAYING) return nullptr;

    if (clickedMenuButton(mx, my))
        return std::make_unique<OpenPauseMenuCommand>();

    if (starVisible && clickedStar(mx, my, starX, starY))
        return std::make_unique<ActivatePowerupCommand>();

    return std::make_unique<PopBubbleCommand>(mx, my);
}
