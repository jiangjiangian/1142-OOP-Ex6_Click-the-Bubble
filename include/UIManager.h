// File: UIManager.h — UI / HUD 繪製輔助（字型、按鈕、文字測量）
// ── UIManager.h ────────────────────────────────────────────
// UIManager owns NO state — all data comes in as parameters.
// High cohesion: only drawing code lives here.
// Low coupling: knows nothing about GameManager or BubblePool.
// ──────────────────────────────────────────────────────────
#ifndef UIMANAGER_H
#define UIMANAGER_H

#include "raylib.h"
#include "Config.h"
#include "Player.h"

class UIManager {
public:
    // ── Game HUD ────────────────────────────────────────────
    void drawHUD(const Player& player, Difficulty diff, bool hellMode) const;
    void drawHellOverlay()               const;
    void drawPowerupBar(float timer)     const;
    void drawFPS()                       const;

    // ── Shared primitives (also used by GameStates) ─────────
    void    drawButton(Rectangle r, const char* label,
                       Color bg, int fontSize = 22) const;
    static bool    hitRect(float px, float py, Rectangle r);

    // Font lifecycle / helpers
    void init();
    void shutdown();
    void drawText(const char* text, int x, int y, int fontSize, Color color) const;
    int  measureText(const char* text, int fontSize) const;

    // ── Layout constants (shared with GameStates for click tests) ─
    static constexpr float MENU_BTN_X = SCREEN_W - 100.f;
    static constexpr float MENU_BTN_Y = 10.f;
    static constexpr float MENU_BTN_W = 90.f;
    static constexpr float MENU_BTN_H = 36.f;
private:
    Font font;                  // active font used for DrawTextEx
    bool customFontLoaded = false;
};

#endif // UIMANAGER_H
