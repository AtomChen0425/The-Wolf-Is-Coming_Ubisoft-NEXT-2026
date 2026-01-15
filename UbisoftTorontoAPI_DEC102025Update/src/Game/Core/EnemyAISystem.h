#pragma once
#include "../../System/ECS/ECS.h"

namespace EnemyAISystem
{
    // Update enemy movement toward player
    void UpdateEnemyMovement(EntityManager& registry, const float deltaTimeMs);
    
    // Update enemy shooting at player
    void UpdateEnemyShooting(EntityManager& registry, const float deltaTimeMs);
    
    // Update bullet movement and lifetime
    void UpdateBullets(EntityManager& registry, const float deltaTimeMs);
    
    // Main update function that calls all subsystems
    void Update(EntityManager& registry, const float deltaTimeMs);
};
