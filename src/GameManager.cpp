// ── GameManager.cpp ────────────────────────────────────────
#include "GameManager.h"
#include "GameStates.h"
#include "Log.h"
#include <cstdio>

// ════════════════════════════════════════════════════════════
//  Constructor — Singleton (called once)
// ════════════════════════════════════════════════════════════
GameManager::GameManager()
    : phaseManager(Difficulty::MEDIUM)
{
    // Window + audio must be up before any subsystem loads assets
    InitWindow(SCREEN_W, SCREEN_H, "EX06 - Click the Bubble!");
    SetTargetFPS(TARGET_FPS);
    InitAudioDevice();
    ui.init();
    AudioManager::get().load();
    powerup.init();  // Load star texture after InitWindow()

    // Register concrete states with the State Machine
    stateMachine.registerState(GameState::MENU,
        std::make_unique<MenuState>());
    stateMachine.registerState(GameState::PLAYING,
        std::make_unique<PlayingState>());
    stateMachine.registerState(GameState::PAUSED,
        std::make_unique<PausedState>());

    // Subscribe to events (Observer Pattern)
    wireEvents();

    // Enter initial state
    stateMachine.transition(GameState::MENU, *this);
}

// ════════════════════════════════════════════════════════════
//  Observer wiring
//  Each lambda captures only the minimum it needs.
//  Subsystems publishing events never know who listens.
// ════════════════════════════════════════════════════════════
void GameManager::wireEvents() {
    auto& bus = EventBus::get();

    // ── PowerupExpiredEvent → clear ALL bubbles ──────────────
    // This implements the "star clears screen on expiry" feature.
    // Player publishes; GameManager reacts; BubblePool is ignorant of Player.
    bus.subscribe<PowerupExpiredEvent>([this](const PowerupExpiredEvent&) {
        bubblePool.reset();
        // Small visual cue: briefly flash the powerup bar area handled by draw()
        Log::info("[Observer] PowerupExpired → bubblePool.reset()");
    });

    // ── HellModeStartedEvent → log / future: play jingle ────
    bus.subscribe<HellModeStartedEvent>([](const HellModeStartedEvent&) {
        Log::info("[Observer] Hell mode started!");
    });

    // ── PhaseChangedEvent → log (hook for sound / effects) ──
    bus.subscribe<PhaseChangedEvent>([](const PhaseChangedEvent& e) {
        Log::info("[Observer] Phase changed → %d", e.phase);
    });
}

// ════════════════════════════════════════════════════════════
//  Main loop
// ════════════════════════════════════════════════════════════
void GameManager::run() {
    while (!WindowShouldClose() && !exitFlag) {
        float dt = GetFrameTime();
        // Clamp dt to prevent spiral of death on lag spikes
        if (dt > 0.05f) dt = 0.05f;

        handleInput();
        update(dt);
        draw();
    }
    AudioManager::get().unload();
    ui.shutdown();
    CloseAudioDevice();
    CloseWindow();
}

// ════════════════════════════════════════════════════════════
//  Input — Command Pattern dispatch
// ════════════════════════════════════════════════════════════
void GameManager::handleInput() {
    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) return;
    Vector2 m = GetMousePosition();

    GameState curState = stateMachine.currentId();

    if (curState == GameState::PLAYING) {
        // PLAYING: delegate to InputHandler → Command
        // Get star position (defaults to far off-screen if not visible)
        float sx = powerup.getStarX();
        float sy = powerup.getStarY();
        auto cmd = inputHandler.processClick(
                       m.x, m.y, curState,
                       powerup.isStarVisible(), sx, sy);
        if (cmd) cmd->execute(*this);
    } else {
        // MENU / PAUSED: delegate to state's own handleClick
        stateMachine.handleClick(*this, m.x, m.y);
    }
}

// ════════════════════════════════════════════════════════════
//  Update / Draw — pure delegation to StateMachine
// ════════════════════════════════════════════════════════════
void GameManager::update(float dt) {
    stateMachine.update(*this, dt);
}

void GameManager::draw() const {
    BeginDrawing();
    ClearBackground(getBackgroundColor());
    stateMachine.render(*this);
    EndDrawing();
}

Color GameManager::getBackgroundColor() const {
    bool hell = (stateMachine.currentId() == GameState::PLAYING ||
                 stateMachine.currentId() == GameState::PAUSED)
                && phaseManager.isHell(player.getScore());
    return hell ? Color{30, 5, 5, 255} : Color{15, 15, 35, 255};
}

// ════════════════════════════════════════════════════════════
//  startGame / resetGame
// ════════════════════════════════════════════════════════════
void GameManager::startGame(Difficulty d) {
    phaseManager.setDifficulty(d);
    player.reset();
    bubblePool.reset();
    EventBus::get().clear();   // clear stale subscriptions before re-wiring
    wireEvents();              // re-subscribe with fresh lambdas
    stateMachine.transition(GameState::PLAYING, *this);
}

void GameManager::resetGame(bool keepDifficulty) {
    player.reset();
    bubblePool.reset();
    EventBus::get().clear();
    wireEvents();
    if (keepDifficulty)
        stateMachine.transition(GameState::PLAYING, *this);
    else
        stateMachine.transition(GameState::MENU, *this);
}
