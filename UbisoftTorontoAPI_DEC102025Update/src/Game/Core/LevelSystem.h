#pragma once
#include "GameLevelData.h"

// LevelSystem: Manages round progression and level-related game logic
namespace LevelSystem {
    // Update level timer and check for round completion
    // Returns true if the round just completed (for triggering upgrade scene)
    bool Update(GameLevelData& levelData, float deltaTimeMs);
    
    // Check if game should end (all sheep dead)
    // Returns true if game over condition is met
    bool CheckGameOver(class EntityManager& registry);
    
    // Get count of alive sheep
    int GetSheepCount(class EntityManager& registry);
}
