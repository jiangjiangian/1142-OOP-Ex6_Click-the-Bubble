// ── GameStates.cpp ─────────────────────────────────────────
#include "GameStates.h"
#include "GameManager.h"
#include "UIManager.h"
#include "AudioManager.h"
#include <cmath>
#include <cstdio>
#include <cstring>

// ════════════════════════════════════════════════════════════
//  Shared rule-text helper (used by both MenuState & PausedState)
// ════════════════════════════════════════════════════════════
static void drawRulesShared(int x, int y) {
    constexpr int FSH=19, FS=17, DY=22;
    Color hdr={255,210,50,255}, body={195,195,215,255};
    Color rd={230,80,80,255},   gld={255,185,50,255};

    DrawText("OBJECTIVE", x,y,FSH,hdr); y+=24;
    DrawText("Click bubbles to score. Avoid red ones!",x+12,y,FS,body); y+=DY+4;

    DrawText("BUBBLE SCORES", x,y,FSH,hdr); y+=24;
    struct { const char* l; Color c; } bs[6]={
        {"Yellow +1",{255,230,50,255}},  {"Green  +2",{80,220,80,255}},
        {"Cyan   +3",{50,210,220,255}},  {"Blue   +5",{80,130,255,255}},
        {"Orange +7",{255,155,50,255}},  {"Purple +10",{200,80,255,255}},
    };
    for (int i=0;i<6;i++){
        int col=i%2, row=i/2;
        int cx2=x+12+col*250, cy2=y+row*DY;
        DrawCircle(cx2+8,cy2+8,7,bs[i].c);
        DrawText(bs[i].l,cx2+22,cy2,FS,body);
    }
    y+=DY*3+4;
    DrawText("Red bubble: PENALTY (-5/-10/-20/-30 by phase)",x+12,y,FS,rd);   y+=DY;
    DrawText("SLOW: slows all bubbles.  POP: clears some.",  x+12,y,FS,{100,200,255,255}); y+=DY+6;

    DrawText("DIFFICULTIES", x,y,FSH,hdr); y+=24;
    DrawText("EASY   - slow, few reds",  x+12,y,FS,{80,220,80,255});  y+=DY;
    DrawText("MEDIUM - balanced",        x+12,y,FS,{80,130,255,255}); y+=DY;
    DrawText("HARD   - fast, many reds", x+12,y,FS,{220,80,80,255});  y+=DY+6;

    DrawText("PHASES", x,y,FSH,hdr); y+=24;
    DrawText("Phase 1   0-49  pts : slow,  red = -5",   x+12,y,FS,body); y+=DY;
    DrawText("Phase 2  50-99  pts : medium, red = -10",  x+12,y,FS,body); y+=DY;
    DrawText("Phase 3 100-199 pts : fast,  red = -20",   x+12,y,FS,body); y+=DY;
    DrawText("HELL    200+    pts : flood, red = -30!",  x+12,y,FS,rd);   y+=DY+6;

    DrawText("STAR POWER-UP", x,y,FSH,hdr); y+=24;
    DrawText("Appears every 40s, drifts 5s then vanishes.", x+12,y,FS,gld); y+=DY;
    DrawText("Click: convert reds, 2x speed, 5s duration.", x+12,y,FS,gld); y+=DY;
    DrawText("On expiry: ALL bubbles cleared from screen!",  x+12,y,FS,{255,140,0,255}); y+=DY+4;
    DrawText("Bubbles wrap around screen edges.", x,y,FS,{140,140,160,255});
    (void)y;
}

// ════════════════════════════════════════════════════════════
//  MenuState
// ════════════════════════════════════════════════════════════
void MenuState::onEnter(GameManager& /*gm*/) { showingHowToPlay = false; }
void MenuState::onExit (GameManager& /*gm*/) {}

void MenuState::handleClick(GameManager& gm, float mx, float my) {
    if (showingHowToPlay) {
        if (UIManager::hitRect(mx,my,
                {SCREEN_W/2.f-80.f, SCREEN_H-68.f, 160.f, 44.f}))
            showingHowToPlay = false;
        return;
    }
    float bw=180.f, bh=60.f, gap=30.f;
    float sx=SCREEN_W/2.f-(bw*3+gap*2)/2.f;
    // Center the difficulty buttons vertically on the main menu
    float by=SCREEN_H/2.f - bh/2.f;

    if      (UIManager::hitRect(mx,my,{sx,            by,bw,bh})) gm.startGame(Difficulty::EASY);
    else if (UIManager::hitRect(mx,my,{sx+bw+gap,     by,bw,bh})) gm.startGame(Difficulty::MEDIUM);
    else if (UIManager::hitRect(mx,my,{sx+(bw+gap)*2, by,bw,bh})) gm.startGame(Difficulty::HARD);
    else {
        float howY = by + bh + 34.f; // extra spacing under difficulty buttons
        if (UIManager::hitRect(mx,my,{SCREEN_W/2.f-90.f, howY, 180.f, 44.f}))
        showingHowToPlay = true;
    }
}

void MenuState::update(GameManager& /*gm*/, float /*dt*/) {}

void MenuState::render(const GameManager& gm) const {
    if (showingHowToPlay) {
        DrawRectangle(0,0,SCREEN_W,SCREEN_H,{10,10,25,248});
        int tfs=40, tw=gm.ui.measureText("How to Play",tfs);
        gm.ui.drawText("How to Play", SCREEN_W/2-tw/2, 28, tfs, WHITE);
        DrawLine(60,76,SCREEN_W-60,76,{80,80,130,180});
        drawRulesShared(60, 88);
        gm.ui.drawButton({SCREEN_W/2.f-80.f,SCREEN_H-68.f,160.f,44.f},
                          "Back",{80,80,140,230},22);
        return;
    }

    // Decorative floating bubbles
    float t = (float)GetTime();
    for (int i=0;i<14;i++){
        float a  = t*.25f + i*(2.f*PI/14.f);
        float cx = SCREEN_W/2.f+cosf(a)*340.f;
        float cy = SCREEN_H/2.f+sinf(a*.6f)*190.f;
        unsigned char al=(unsigned char)((0.4f+0.4f*sinf(t+i))*55.f);
        DrawCircle((int)cx,(int)cy, 22.f+8.f*sinf(t*.4f+i), {100,180,255,al});
    }
    const char* title="Click the Bubble!";
    int tfs=56;
    int tw=gm.ui.measureText(title,tfs);
    // layout: title above, "Select Difficulty" label, then buttons centered
    int titleY = (int)(SCREEN_H/2.f - 140.f);
    gm.ui.drawText(title, SCREEN_W/2 - tw/2, titleY, tfs, WHITE);
    int sw = gm.ui.measureText("Select Difficulty",26);
    int labelY = (int)(SCREEN_H/2.f - 80.f);
    gm.ui.drawText("Select Difficulty", SCREEN_W/2 - sw/2, labelY, 26, LIGHTGRAY);

    float bw=180.f, bh=60.f, gap=30.f;
    float sx2=SCREEN_W/2.f-(bw*3+gap*2)/2.f;
    float by=SCREEN_H/2.f - bh/2.f;
    struct { const char* lbl; Color c; const char* hint; } btns[3]={
        {"EASY",   {55,185,65,230},  "Slower, fewer reds"},
        {"MEDIUM", {60,125,255,230}, "Balanced"},
        {"HARD",   {215,45,45,230},  "Fast & more reds"},
    };
    for (int i=0;i<3;i++){
        float bx=sx2+i*(bw+gap);
        gm.ui.drawButton({bx,by,bw,bh}, btns[i].lbl, btns[i].c, 26);
        int hw=gm.ui.measureText(btns[i].hint,16);
        gm.ui.drawText(btns[i].hint,(int)(bx+bw/2-hw/2),(int)(by+bh+12),16,LIGHTGRAY);
    }
    float howY = by + bh + 34.f;
    gm.ui.drawButton({SCREEN_W/2.f-90.f,howY,180.f,44.f},
                      "How to Play",{70,70,110,220},20);
    int hw=gm.ui.measureText("press ESC to quit",17);
    gm.ui.drawText("press ESC to quit", SCREEN_W/2-hw/2, SCREEN_H-30, 17, {130,130,130,200});
}

// ════════════════════════════════════════════════════════════
//  PlayingState
// ════════════════════════════════════════════════════════════
void PlayingState::onEnter(GameManager& /*gm*/) {}
void PlayingState::onExit (GameManager& /*gm*/) {}

void PlayingState::handleClick(GameManager& gm, float mx, float my) {
    // NOTE: In the refactored system, PlayingState::handleClick is only
    // reached for MENU/PAUSED states via StateMachine. PLAYING input
    // goes through InputHandler → Command in GameManager::handleInput().
    // This override handles the rare direct-dispatch fallback.
    (void)gm; (void)mx; (void)my;
}

void PlayingState::update(GameManager& gm, float dt) {
    gm.player.update(dt);

    PhaseConfig cfg = gm.phaseManager.getConfig(gm.player.getScore());
    bool hell       = gm.phaseManager.isHell(gm.player.getScore());
    gm.bubblePool.update(dt, cfg, hell, gm.player.isPowerupActive());
    gm.powerup.update(dt);
}

void PlayingState::render(const GameManager& gm) const {
    gm.bubblePool.draw();
    gm.powerup.draw();

    bool hell = gm.phaseManager.isHell(gm.player.getScore());
    if (hell) gm.ui.drawHellOverlay();
    if (gm.player.isPowerupActive())
        gm.ui.drawPowerupBar(gm.player.getPowerupTimer());
    gm.ui.drawHUD(gm.player, gm.phaseManager.getDifficulty(), hell);
    gm.ui.drawFPS();
}

// ════════════════════════════════════════════════════════════
//  PausedState
// ════════════════════════════════════════════════════════════
void PausedState::onEnter(GameManager& /*gm*/) { showingHowToPlay = false; }
void PausedState::onExit (GameManager& /*gm*/) {}

void PausedState::handleClick(GameManager& gm, float mx, float my) {
    if (showingHowToPlay) {
        if (UIManager::hitRect(mx,my,
                {SCREEN_W/2.f-80.f, SCREEN_H-68.f, 160.f, 44.f}))
            showingHowToPlay = false;
        return;
    }
    float bw=240.f, bh=50.f;
    float bx=SCREEN_W/2.f-bw/2.f;
    float by=SCREEN_H/2.f-150.f;
    if      (UIManager::hitRect(mx,my,{bx,by,       bw,bh})) gm.stateMachine.transition(GameState::PLAYING,gm);
    else if (UIManager::hitRect(mx,my,{bx,by+60.f,  bw,bh})) showingHowToPlay = true;
    else if (UIManager::hitRect(mx,my,{bx,by+120.f, bw,bh})) gm.resetGame(true);
    else if (UIManager::hitRect(mx,my,{bx,by+180.f, bw,bh})) gm.resetGame(false);
    else if (UIManager::hitRect(mx,my,{bx,by+240.f, bw,bh})) gm.exitFlag = true;
}

void PausedState::render(const GameManager& gm) const {
    // Frozen game world behind overlay
    gm.bubblePool.draw();
    gm.powerup.draw();
    bool hell = gm.phaseManager.isHell(gm.player.getScore());
    gm.ui.drawHUD(gm.player, gm.phaseManager.getDifficulty(), hell);
    gm.ui.drawFPS();

    if (showingHowToPlay) {
        DrawRectangle(0,0,SCREEN_W,SCREEN_H,{0,0,0,220});
        int tfs=40, tw=gm.ui.measureText("How to Play",tfs);
        gm.ui.drawText("How to Play",SCREEN_W/2-tw/2,28,tfs,WHITE);
        DrawLine(60,76,SCREEN_W-60,76,{80,80,130,180});
        drawRulesShared(60,88);
        gm.ui.drawButton({SCREEN_W/2.f-80.f,SCREEN_H-68.f,160.f,44.f},
                          "Back",{80,80,140,230},22);
        return;
    }

    DrawRectangle(0,0,SCREEN_W,SCREEN_H,{0,0,0,210});
    int ptw=gm.ui.measureText("PAUSED",42);
    gm.ui.drawText("PAUSED",SCREEN_W/2-ptw/2,(int)(SCREEN_H/2.f-210.f),42,WHITE);

    float bw=240.f,bh=50.f;
    float bx=SCREEN_W/2.f-bw/2.f, by=SCREEN_H/2.f-150.f;
    struct { const char* lbl; Color c; } btns[5]={
        {"Continue",    {40,150,200,230}},
        {"How to Play", {200,150,40,230}},
        {"Restart",     {60,185,60,230}},
        {"Change Mode", {60,120,255,230}},
        {"Exit",        {210,45,45,230}},
    };
    for (int i=0;i<5;i++)
        gm.ui.drawButton({bx,by+i*60.f,bw,bh},btns[i].lbl,btns[i].c);
}
