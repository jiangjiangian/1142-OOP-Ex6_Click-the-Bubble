// ── UIManager.cpp ──────────────────────────────────────────
#include "UIManager.h"
#include "Log.h"
#include <cstdio>
#include <cmath>
#include <cstring>

// helper for font path probing
static const char* FONT_CANDIDATES[] = {
    "resources/font.ttf",
    "resources/Roboto-Regular.ttf",
    "resources/DejaVuSans.ttf",
    "../resources/font.ttf",
    "../resources/Roboto-Regular.ttf",
    nullptr
};

// ── Shared primitives ──────────────────────────────────────
bool UIManager::hitRect(float px, float py, Rectangle r) {
    return px >= r.x && px <= r.x + r.width &&
           py >= r.y && py <= r.y + r.height;
}

void UIManager::init() {
    // Try to load a bundled font if present; otherwise keep default
    for (int i=0; FONT_CANDIDATES[i]; ++i) {
        const char* p = FONT_CANDIDATES[i];
        if (FileExists(p)) {
                Font f = LoadFont(p);
                if (f.texture.id != 0) {
                    font = f;
                    customFontLoaded = true;
                    Log::info("[UIManager] loaded font: %s", p);
                    return;
                }
            }
    }
    // fallback
    font = GetFontDefault();
    customFontLoaded = false;
}

void UIManager::shutdown() {
    if (customFontLoaded) {
        UnloadFont(font);
        customFontLoaded = false;
    }
    font = GetFontDefault();
}

int UIManager::measureText(const char* text, int fontSize) const {
    Vector2 ext = MeasureTextEx(font, text, (float)fontSize, 1.0f);
    return (int)ext.x;
}

void UIManager::drawText(const char* text, int x, int y, int fontSize, Color color) const {
    Vector2 pos = {(float)x, (float)y};
    DrawTextEx(font, text, pos, (float)fontSize, 1.0f, color);
}

void UIManager::drawButton(Rectangle r, const char* label,
                            Color bg, int fs) const {
    DrawRectangleRounded(r, 0.25f, 8, bg);
    int tw = measureText(label, fs);
    drawText(label,
             (int)(r.x + r.width /2.f - tw/2.f),
             (int)(r.y + r.height/2.f - fs/2.f),
             fs, WHITE);
}

// ── HUD ────────────────────────────────────────────────────
void UIManager::drawHUD(const Player& player, Difficulty diff,
                         bool hellMode) const {
    // Score
    char buf[32];
    snprintf(buf, sizeof(buf), "Score: %d", player.getScore());
    drawText(buf, 20, 18, 36, WHITE);

    // Mode label with colour coding
    const char* modeStr;
    Color       modeColor = WHITE;
    int         sc        = player.getScore();

    if (hellMode) {
        float fl = 0.5f + 0.5f * sinf((float)GetTime() * 4.f);
        modeColor = {255, (unsigned char)(25 + 55*fl), 25, 255};
        modeStr   = "Mode: HELL";
    } else if (sc >= PHASE3_SCORE) {
        modeColor = {255, 110, 35, 255};
        modeStr   = "Mode: EXTREME";
    } else if (sc >= PHASE2_SCORE) {
        modeColor = {255, 215, 35, 255};
        modeStr   = "Mode: INTENSE";
    } else {
        switch (diff) {
            case Difficulty::EASY: modeStr = "Mode: EASY";   break;
            case Difficulty::HARD: modeStr = "Mode: HARD";   break;
            default:               modeStr = "Mode: MEDIUM"; break;
        }
    }
    drawText(modeStr, 20, 60, 24, modeColor);

    // Powerup active banner
    if (player.isPowerupActive()) {
        snprintf(buf, sizeof(buf), "★ STAR POWER!  %.1fs",
                 player.getPowerupTimer());
        int nw = measureText(buf, 28);
        drawText(buf, SCREEN_W/2 - nw/2, 18, 28, GOLD);
    }

    // Menu button (top-right)
    drawButton({MENU_BTN_X, MENU_BTN_Y, MENU_BTN_W, MENU_BTN_H},
               "Menu", {50, 50, 80, 210}, 20);
}

void UIManager::drawHellOverlay() const {
    float t  = (float)GetTime();
    unsigned char va = (unsigned char)(25 + 20 * sinf(t * 2.f));
    DrawRectangleLinesEx({0,0,(float)SCREEN_W,(float)SCREEN_H},
                         28.f, {180, 0, 0, va});
    float fl = 0.5f + 0.5f * sinf(t * 3.5f);
    Color fc = {255, (unsigned char)(20*fl), (unsigned char)(20*fl), 255};
    const char* msg = "!!! HELL MODE !!!";
    int mw = measureText(msg, 30);
    drawText(msg, SCREEN_W/2 - mw/2, SCREEN_H - 50, 30, fc);
}

void UIManager::drawPowerupBar(float timer) const {
    float ratio = timer / STAR_POWERUP_DURATION;
    DrawRectangle(0, SCREEN_H - 10, SCREEN_W, 10, {60, 50, 0, 150});
    DrawRectangle(0, SCREEN_H - 10, (int)(SCREEN_W * ratio), 10, GOLD);
}

void UIManager::drawFPS() const {
    DrawFPS(SCREEN_W - 100, SCREEN_H - 30);
}
