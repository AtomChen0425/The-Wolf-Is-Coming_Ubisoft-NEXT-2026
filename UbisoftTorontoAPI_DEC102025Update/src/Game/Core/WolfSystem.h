#pragma once
#include "../../System/ECS/ECS.h"
#include "../../System/Component/Component.h"
#include "GameConfig.h"

namespace WolfSystem {
    // Initialize wolves at a specific location
    void InitWolves(EntityManager& registry, float startX, float startZ, int count, const GameConfig& config);
    
    // Initialize a single wolf of a specific type
    void InitWolfOfType(EntityManager& registry, float x, float z, WolfType type, const GameConfig& config);

    // Update wolf behavior each frame (should be called from EngineSystem::Update)
    void Update(EntityManager& registry, float dtMs);
}
