#pragma once
#include "../../System/ECS/ECS.h"

namespace SheepSystem {
    void InitSheep(EntityManager& registry, float startX, float startZ, int count);

    void Update(EntityManager& registry, float dtMs);
    void SheepShoot(EntityManager& registry, float dt);
}