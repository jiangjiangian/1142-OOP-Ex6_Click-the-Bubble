// ── StateMachine.cpp ───────────────────────────────────────
#include "StateMachine.h"
#include "GameStates.h"
#include "GameManager.h"

StateMachine::StateMachine() {}

void StateMachine::registerState(GameState id,
                                  std::unique_ptr<IGameState> state) {
    states[(int)id] = std::move(state);
}

IGameState* StateMachine::lookup(GameState id) const {
    int idx = (int)id;
    if (idx < 0 || idx >= NUM_STATES) return nullptr;
    return states[idx].get();
}

void StateMachine::transition(GameState next, GameManager& gm) {
    if (current) current->onExit(gm);
    curId   = next;
    current = lookup(next);
    if (current) current->onEnter(gm);
}

void StateMachine::handleClick(GameManager& gm, float mx, float my) {
    if (current) current->handleClick(gm, mx, my);
}

void StateMachine::update(GameManager& gm, float dt) {
    if (current) current->update(gm, dt);
}

void StateMachine::render(const GameManager& gm) const {
    if (current) current->render(gm);
}
