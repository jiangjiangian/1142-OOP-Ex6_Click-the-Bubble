// ── main.cpp ───────────────────────────────────────────────
// Single responsibility: obtain the Singleton and run.
// All initialisation lives in GameManager's constructor.
// ──────────────────────────────────────────────────────────
#include "GameManager.h"

int main() {
    GameManager::get().run();
    return 0;
}
