// ── Commands.cpp ───────────────────────────────────────────
#include "Commands.h"
#include "GameManager.h"
#include "AudioManager.h"

// ── PopBubbleCommand ───────────────────────────────────────
void PopBubbleCommand::execute(GameManager& gm) {
    PhaseConfig cfg = gm.phaseManager.getConfig(gm.player.getScore());
    bool hell       = gm.phaseManager.isHell(gm.player.getScore());
    int  delta      = gm.bubblePool.handleClick(
                          mx, my, cfg, hell, gm.player.isPowerupActive());
    if (delta != 0) {
        gm.player.addScore(delta);   // publishes ScoreChangedEvent
        AudioManager::get().playPop();
    }
}

// ── ActivatePowerupCommand ─────────────────────────────────
void ActivatePowerupCommand::execute(GameManager& gm) {
    // Pool effects first, then activate (so EventBus subscribers see clean state)
    gm.bubblePool.convertAllDangerToNormal();
    gm.bubblePool.applySpeedMultiplier(2.f);
    gm.player.activatePowerup();   // publishes PowerupActivatedEvent
}

// ── OpenPauseMenuCommand ───────────────────────────────────
void OpenPauseMenuCommand::execute(GameManager& gm) {
    gm.stateMachine.transition(GameState::PAUSED, gm);
}

// ── StartGameCommand ───────────────────────────────────────
void StartGameCommand::execute(GameManager& gm) {
    gm.startGame(diff);
}

// ── ResetGameCommand ───────────────────────────────────────
void ResetGameCommand::execute(GameManager& gm) {
    gm.resetGame(keep);
}

// ── ExitGameCommand ────────────────────────────────────────
void ExitGameCommand::execute(GameManager& gm) {
    gm.exitFlag = true;
}
