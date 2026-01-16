#include "LevelSystem.h"
#include "../../System/ECS/ECS.h"
#include "../../System/Component/Component.h"

namespace LevelSystem {
    
    bool Update(GameLevelData& levelData, float deltaTimeMs) {
        // Update timers
        levelData.totalGameTimeMs += deltaTimeMs;
        levelData.currentRoundTimeMs += deltaTimeMs;
        
        // Check if round is complete
        if (levelData.IsRoundComplete()) {
            return true;  // Signal that round is complete
        }
        
        return false;
    }
    
    int GetSheepCount(EntityManager& registry) {
        int count = 0;
        View<SheepTag> sheepView(registry);
        for (auto id : sheepView) {
            count++;
        }
        return count;
    }
    
    bool CheckGameOver(EntityManager& registry) {
        // Game is over if there are no sheep left
        return GetSheepCount(registry) == 0;
    }
}
