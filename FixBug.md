# EX06 Bug 修復紀錄

---
title: EX06 Bug 修復紀錄
tags: OOP, C++, debugging
---

## 修復總覽

| # | 問題 | 嚴重性 | 影響檔案 |
|:--|:-----|:------:|:---------|
| 1 | 舊 `Game` 類別殘留，與新架構衝突 | 致命 | `Game.h`, `Game.cpp` |
| 2 | `Powerup.cpp` 缺少 `#include <cmath>` | 致命 | `Powerup.cpp` |
| 3 | `Star.cpp` 缺少 `#include <cstdio>` | 致命 | `Star.cpp` |
| 4 | `Powerup` 材質在 `InitWindow()` 前載入 → segfault | 致命 | `Powerup.h/cpp`, `GameManager.cpp` |
| 5 | 碰撞加速係數過高 → 速度失控 | 嚴重 | `BubblePool.cpp` |

---

## Bug 1 — 舊 Game 類別殘留

### 問題

上傳的檔案中同時存在舊版 `Game.h / Game.cpp`（553 行 God Class）和重構後的新架構，造成符號衝突。

### 修復

直接刪除這兩個檔案。新架構已由以下類別取代：

```
GameT（舊）→  GameManager + Player + BubblePool
              + Powerup + StateMachine
```

---

## Bug 2 — Powerup.cpp 缺少 `<cmath>`

### 問題

`sinf()` 未宣告：

```
error: 'sinf' was not declared in this scope
Powerup.cpp:39
```

### 修復

```cpp
// Powerup.cpp
#include "Powerup.h"
#include <cmath>   // ← 新增
```

---

## Bug 3 — Star.cpp 缺少 `<cstdio>`

### 問題

`snprintf()` 未宣告：

```
error: 'snprintf' was not declared in this scope
Star.cpp:58
```

### 修復

```cpp
// Star.cpp
#include "Star.h"
#include <cstdlib>
#include <cmath>
#include <cstdio>  // ← 新增
```

---

## Bug 4 — Powerup 材質在視窗建立前載入（segfault）

### 問題

編譯成功，但執行立即崩潰：

```
INFO: FILEIO: [resources/star.png] File loaded successfully
INFO: IMAGE: Data loaded successfully (360x360 | R8G8B8 | 1 mipmaps)
zsh: segmentation fault  ./build/OOP_Raylib_Lab
```

### 根本原因

C++ 成員變數在建構子主體**之前**就被初始化。`Powerup` 是 `GameManager` 的成員，因此 `Powerup::Powerup()` 在 `InitWindow()` 之前就被呼叫，此時 Raylib 的 OpenGL context 尚未建立，`LoadTexture()` 存取非法記憶體。

```
錯誤順序：
1. GameManager 建構開始
2. ├─ Powerup 成員初始化 → Powerup::Powerup()
3. │  └─ LoadTexture()  ← segfault！視窗未建立
4. ├─ InitWindow()
```

### 修復

將材質載入移到獨立的 `init()` 方法，在 `InitWindow()` 之後手動呼叫。

**Powerup.h**：
```cpp
class Powerup {
public:
    Powerup();
    ~Powerup();
    void init();   // ← 新增，必須在 InitWindow() 後呼叫
    void update(float dt);
    void draw() const;
    // ...
};
```

**Powerup.cpp**：
```cpp
Powerup::Powerup() : spawnCooldown(0.f), texLoaded(false) {
    // 不在這裡載入材質
}

void Powerup::init() {
    if (FileExists("resources/star.png")) {
        starTex   = LoadTexture("resources/star.png");
        texLoaded = true;
    }
}
```

**GameManager.cpp**：
```cpp
GameManager::GameManager() : phaseManager(Difficulty::MEDIUM) {
    InitWindow(SCREEN_W, SCREEN_H, "EX06 - Click the Bubble!");
    SetTargetFPS(TARGET_FPS);
    InitAudioDevice();

    AudioManager::get().load();
    powerup.init();  // ← 視窗建立後才呼叫
    // ...
}
```

```
正確順序：
1. GameManager 建構開始
2. ├─ Powerup 成員初始化（空建構子）
3. ├─ InitWindow()         ← 視窗建立
4. ├─ InitAudioDevice()
5. ├─ AudioManager::get().load()
6. └─ powerup.init()       ← 安全載入材質
```

:::info
**學習要點**：Raylib 所有資源函式（`LoadTexture`、`LoadSound` 等）必須在 `InitWindow()` 之後才能呼叫。成員變數若在建構子中載入 Raylib 資源，應改用 Two-Phase Initialization（建構子 + `init()` 方法）。
:::

---

## Bug 5 — 碰撞加速係數過高

### 問題

碰撞系統每次碰撞後將速度乘以 `1.05f`。在多泡泡場景下，速度以指數成長：

| 碰撞次數 | 速度倍率（5%） | 速度倍率（2%） |
|:--------:|:-------------:|:-------------:|
| 10 | 1.63× | 1.22× |
| 30 | 4.32× | 1.81× |
| 50 | 11.5× | 2.69× |

Phase 3 / Hell 模式下，數十顆泡泡可在數秒內產生數百次碰撞，速度迅速突破可玩範圍。

### 修復

**BubblePool.cpp**：
```cpp
// 舊
constexpr float BOOST = 1.05f;

// 新
constexpr float BOOST = 1.02f;   // ← 降為 2%
```

配合 `Entity::setVelocity()` 中的硬上限（`GLOBAL_SPEED_CAP = 500.f`），形成雙重保護：

```cpp
void Entity::setVelocity(float nvx, float nvy) {
    float spd = sqrtf(nvx*nvx + nvy*nvy);
    if (spd > GLOBAL_SPEED_CAP && spd > 0.f) {
        float s = GLOBAL_SPEED_CAP / spd;
        nvx *= s; nvy *= s;
    }
    vx = nvx; vy = nvy;
}
```

### P.S.: 最後改為進入 Hell 模式下不要碰撞，允許穿透
---

## 最終驗證

```bash
cmake -B build
cmake --build build
./build/OOP_Raylib_Lab
```

:::success
編譯無錯誤、無警告。遊戲正常啟動，Hell 模式可穩定執行 10 分鐘以上。
:::