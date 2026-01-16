# Level System and Game Data Implementation Summary

## Overview
This implementation adds a comprehensive level/round progression system to the game, as requested in the problem statement. The system tracks game statistics, manages round progression, and provides dynamic difficulty scaling through decreasing wolf spawn intervals.

## New Files Created

### 1. GameLevelData.h
**Location:** `src/Game/Core/GameLevelData.h`

A data structure that tracks all level-related game statistics:
- **Total game time**: Cumulative time since game start
- **Current round time**: Time elapsed in the current round
- **Round duration**: Set to 60 seconds (1 minute) per round (configurable)
- **Current round number**: Starts at 1, increments after each round
- **Wolf spawn intervals**: 
  - Base interval: 5000ms (5 seconds) (configurable)
  - Decreases by 200ms each round (configurable)
  - Minimum interval: 1000ms (1 second) (configurable)

Key methods:
- `Initialize(config)`: Must be called after construction to load values from GameConfig
- `Reset()`: Resets all data for a new game
- `NextRound()`: Advances to the next round and adjusts spawn interval
- `IsRoundComplete()`: Checks if 60 seconds have elapsed
- `GetRemainingRoundTime()`: Returns remaining time in seconds

### 2. LevelSystem.h & LevelSystem.cpp
**Location:** `src/Game/Core/LevelSystem.h` and `.cpp`

A namespace containing functions for managing level progression:

- **`Update(GameLevelData& levelData, float deltaTimeMs)`**
  - Updates total game time and current round time
  - Returns `true` when a round completes (triggers upgrade scene)

- **`GetSheepCount(EntityManager& registry)`**
  - Counts all living sheep entities
  - Used for UI display and game over checking

- **`CheckGameOver(EntityManager& registry)`**
  - Returns `true` if there are zero sheep remaining
  - Triggers the game over screen

## Modified Files

### 3. GameConfig.h
**Changes:**
- Added level/round system configuration parameters:
  - `roundDurationMs`: Duration of each round (default 60000ms)
  - `baseWolfSpawnIntervalMs`: Initial spawn interval (default 5000ms)
  - `wolfSpawnReductionPerRound`: Interval reduction (default 200ms)
  - `minWolfSpawnIntervalMs`: Minimum interval (default 1000ms)
  - `sheepAddedPerUpgrade`: Sheep added with "Add Sheep" upgrade (default 10)
  - `sheepSpawnOffsetZ`: Z offset for spawning new sheep (default 100.0f)

### 4. ECSSystem.h
**Changes:**
- Added `#include "../Game/Core/GameLevelData.h"`
- Added `GameLevelData levelData;` member variable
- Added `GetLevelData()` accessor method

### 5. ECSSystem.cpp
**Changes:**
- Added `#include "../Game/Core/LevelSystem.h"`
- In constructor: Added `levelData.Initialize(config)` after loading config
- In `InitializeGame()`: Added `levelData.Reset()` to reset level data on new game
- In `Update()` during Playing state:
  - Added `LevelSystem::Update()` call to track round time
  - Added `LevelSystem::CheckGameOver()` to check if all sheep are dead
  - Modified wolf spawning to use `levelData.currentWolfSpawnIntervalMs` (dynamic difficulty)
  - Added round completion check that switches to UpgradeScene

### 6. GameScenes.h
**Changes:**
- Added new UI text pointers to `PlayingScene`:
  - `roundText`: Displays current round number
  - `timeText`: Displays remaining time in round
  - `sheepText`: Displays current sheep count
- Added `AddSheep` to `UpgradeScene::UpgradeType` enum

### 7. GameScenes.cpp
**Changes:**
- Added `#include "../../Game/Core/LevelSystem.h"`
- Added `#include "../../Game/Core/SheepSystem.h"`

**PlayingScene Updates:**
- `OnEnter()`: Creates UI elements for round, time, and sheep count
- `Update()`: 
  - Updates round number display
  - Updates time remaining (counts down from 60)
  - Updates sheep count display using `LevelSystem::GetSheepCount()`

**UpgradeScene Updates:**
- `Update()`: After selecting an upgrade, calls `levelData.NextRound()` before returning to playing
- `GenerateRandomUpgrades()`: Added `AddSheep` to the pool of available upgrades
- `ApplyUpgrade()`: 
  - Added handler for `AddSheep` type that spawns configurable number of sheep near the player
  - Uses `SheepSystem::InitSheep()` with player's position and config values
- `GetUpgradeName()`: Returns "Add Sheep" for the new upgrade type
- `GetUpgradeDescription()`: Returns dynamic description based on config value

## Game Flow

### Round Progression
1. Game starts at Round 1 with 50 sheep
2. Player plays for 60 seconds while wolves spawn every 5 seconds
3. After 60 seconds, game automatically switches to Upgrade Scene
4. Player chooses one of 3 random upgrades (including possible "Add Sheep")
5. Game advances to Round 2, wolves now spawn every 4.8 seconds (200ms faster)
6. Cycle repeats indefinitely

### Game Over Conditions
- **Sheep Count = 0**: All sheep have been killed by wolves
  - Game immediately switches to Game Over screen
  - Player can press 'R' to restart

### Difficulty Scaling
Each round increases difficulty by:
- Reducing wolf spawn interval by configurable amount (default 200ms)
- Minimum spawn interval caps at configurable value (default 1000ms)
- Example progression with defaults:
  - Round 1: 5000ms (5 seconds)
  - Round 2: 4800ms (4.8 seconds)
  - Round 3: 4600ms (4.6 seconds)
  - Round 20+: 1000ms (1 second, max difficulty)

## UI Display
During gameplay, the top-left corner shows:
```
Score: [score]
Round: [round number]
Time: [seconds remaining]s
Sheep: [current sheep count]
```

## Integration Notes
- The system integrates seamlessly with existing game systems
- Wolf spawning now uses dynamic intervals from `GameLevelData`
- Sheep counting leverages the existing ECS `View<SheepTag>` system
- Upgrade scene was extended to support the new "Add Sheep" option
- All timing uses the existing `deltaTimeMs` parameter

## Technical Details
- All time values stored in milliseconds for precision
- All configuration values loaded from GameConfig
- GameLevelData initialized from config after loading
- Consistent use of GetRegistry() for entity manager access

## Configuration
All parameters can be customized via `game_config.txt`:
```
roundDurationMs = 60000.0           # Round duration (60 seconds)
baseWolfSpawnIntervalMs = 5000.0    # Initial wolf spawn interval (5 seconds)
wolfSpawnReductionPerRound = 200.0  # Spawn interval reduction per round
minWolfSpawnIntervalMs = 1000.0     # Minimum wolf spawn interval (1 second)
sheepAddedPerUpgrade = 10           # Sheep added with "Add Sheep" upgrade
sheepSpawnOffsetZ = 100.0           # Z offset for spawning new sheep
```

## Future Enhancements (Not Implemented)
Potential improvements could include:
- Cache sheep count instead of querying every frame (optimization)
- Cache remaining time calculation (optimization)
- Use config parameter in constructor instead of Initialize() method (design improvement)
- Configurable round duration
- Different difficulty curves
- Score multipliers per round
- Pause functionality during rounds
- Save/load game state
- Statistics tracking (highest round reached, etc.)
