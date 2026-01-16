#pragma once
#include "../../System/ECS/ECS.h"

namespace WolfSystem {
    // Spawn a single wolf outside player's view range
    void SpawnWolf(EntityManager& registry, float playerX, float playerZ, float spawnDistance);

    // Update wolf spawning timer and behavior (should be called from EngineSystem::Update)
    void Update(EntityManager& registry, float dtMs, float playerX, float playerZ);
}
