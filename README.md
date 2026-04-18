# EX06 - Click the Bubble

---
### 系級：心理三
### 學號：112702005
---

## 專案檔案樹 (Project Tree)

```
CMakeLists.txt
FixBug.md
README.md
include/
	AudioManager.h
	Bubble.h
	BubbleFactory.h
	BubblePool.h
	Commands.h
	Config.h
	Entity.h
	EventBus.h
	GameManager.h
	GameStates.h
	IGameState.h
	InputHandler.h
	PhaseManager.h
	Player.h
	Powerup.h
	README.md
	Star.h
	StateMachine.h
	UIManager.h
resources/
	README.md
src/
	AudioManager.cpp
	Bubble.cpp
	BubbleFactory.cpp
	BubblePool.cpp
	Commands.cpp
	Entity.cpp
	GameManager.cpp
	GameStates.cpp
	InputHandler.cpp
	main.cpp
	Player.cpp
	Powerup.cpp
	README.md
	Star.cpp
	StateMachine.cpp
	UIManager.cpp
```


## 專案概述

使用 **C++17 + Raylib** 實作的泡泡點擊遊戲，將原始 553 行單體 `GameT` 重構為 **21 個類別 + 7 種設計模式**。

| 項目 | 數值 |
|:-----|:-----|
| 標頭檔 | 18 個 |
| 實作檔 | 15 個 |
| 類別數 | 21 個 |
| 設計模式 | 7 種 |
| 事件類型 | 6 種 |

---

## 專案結構

```
Ex6/
├── include/          # 18 個 .h
├── src/              # 15 個 .cpp
├── resources/        # pop.wav, star.png（選用）
└── CMakeLists.txt
```

---

## 設計模式

### Singleton
`AudioManager`、`GameManager`、`EventBus` 各自只允許一個實例，透過 `::get()` 靜態方法存取。呼叫者不持有指標，消除跨模組的資源耦合。

### State
`StateMachine` 持有三個 `IGameState` 實例。`transition()` 呼叫舊狀態的 `onExit` 再呼叫新狀態的 `onEnter`，消除了原本散落在每個方法裡的 `if (state == PLAYING)` 判斷。

```
IGameState
├── MenuState     — 難度選擇 + 說明頁
├── PlayingState  — 主遊戲邏輯
└── PausedState   — 凍結畫面的暫停覆層
```

### Observer
`EventBus` 是型別安全的模板 Singleton。發布者與訂閱者互不持有對方的指標，只透過事件結構體溝通。

| 事件 | 發布者 | 訂閱者 |
|:-----|:-------|:-------|
| `ScoreChangedEvent` | `Player` | `UIManager`、`PhaseManager` |
| `PowerupExpiredEvent` | `Player` | `GameManager` → 清空 `BubblePool` |
| `HellModeStartedEvent` | `Player` | `GameManager` |
| `PhaseChangedEvent` | `Player` | `GameManager` |

### Object Pool
`BubblePool` 預分配 200 個 `Bubble`，用 `freeStack` + `activeList` 雙陣列管理。`acquire()` / `release()` 均為 O(1)，遊戲執行期間**零 heap 分配**。

### Factory
`BubbleFactory::create(slot, kind, cfg)` 統一負責顏色、分數、速度、生成位置的計算，消除了原本的 Long Parameter List 臭味。

### Component
`Bubble` 組合兩個內部元件：

- `RenderComp`：顏色、爆破動畫，不知道分數與類型
- `MetaComp`：`BubbleType`、分數值，不知道如何繪製

### Command
`InputHandler` 將滑鼠點擊映射為 `ICommand` 物件，GameManager 呼叫 `execute()`。每個指令只持有自己需要的資料，不持有 `GameManager` 指標。

```
ICommand
├── PopBubbleCommand(mx, my)
├── ActivatePowerupCommand
├── OpenPauseMenuCommand
├── StartGameCommand(difficulty)
├── ResetGameCommand(keepDifficulty)
└── ExitGameCommand
```

---

## 遊戲規則

### 泡泡分數

| 顏色 | 分數 | 顏色 | 分數 |
|:-----|:----:|:-----|:----:|
| 黃色 | +1 | 橙色 | +7 |
| 綠色 | +2 | 紫色 | +10 |
| 青色 | +3 | **紅色** | **扣分** |
| 藍色 | +5 | | |

### 階段系統

| 階段 | 分數 | 最大數量 | 紅色比例 | 紅色扣分 |
|:-----|:----:|:--------:|:--------:|:--------:|
| Phase 1 | 0–49 | 15 | 10% | -5 |
| Phase 2 | 50–99 | 20 | 20% | -10 |
| Phase 3 | 100–199 | 30 | 35% | -20 |
| **HELL** | 200+ | 50 | 99% | **-30** |

難度修正：Easy 速度 ×0.6、Hard 速度 ×1.3，生成間隔同比例縮放。

### 特殊泡泡

- **SLOW**（天藍環）：點擊後全體速度乘以 0.75
- **POP**（紫色環）：點擊後依當前階段清除 1/5/10 顆泡泡

### 星星道具

每 40 秒生成，漂浮 5 秒後自動消失。點擊後：

1. 所有紅色泡泡轉為隨機正分顏色
2. 全體速度乘以 2
3. 持續 5 秒
4. **效果結束時清空全螢幕所有泡泡**

### 30 秒定期事件

每 30 秒觸發一次 Clean Sweep：清除 N 顆泡泡並將剩餘速度乘以 0.5。

---

## 編譯與執行

### 環境需求

- C++17 編譯器（g++ 7+ 或 clang++ 5+）
- CMake 3.14+
- Raylib 5.0+（不需預裝，CMake 會自動從 GitHub 下載）

### 步驟

```bash
cmake -B build
cmake --build build
./build/OOP_Raylib_Lab
```

### 資源檔案（選用）

| 檔案 | 用途 | 缺少時 |
|:-----|:-----|:-------|
| `resources/pop.wav` | 泡泡破裂音效 | 靜音執行 |
| `resources/star.png` | 星星材質 | 黃色圓圈替代 |

---
