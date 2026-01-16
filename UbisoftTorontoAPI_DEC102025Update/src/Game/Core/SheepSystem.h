#pragma once
#include "../../System/ECS/ECS.h"

namespace SheepSystem {
    void InitSheep(EntityManager& registry, float startX, float startZ, int count);

    // 每帧更新 (在 EngineSystem::Update 中调用)
    void Update(EntityManager& registry, float dtMs);
}