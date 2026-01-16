#pragma once
#include "../../System/ECS/ECS.h"

namespace WolfSystem {
    // Initialize wolves at a specific location
    void InitWolves(EntityManager& registry, float startX, float startZ, int count);

    // Update wolf behavior each frame (should be called from EngineSystem::Update)
    void Update(EntityManager& registry, float dtMs);
}
