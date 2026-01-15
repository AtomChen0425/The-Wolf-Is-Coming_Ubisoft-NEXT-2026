
#pragma once
#include "../../System/ECS/ECS.h"
#include "../../System/Component/Component.h"
#include "../../System/Math/Vec3.h"

namespace ParticleSystem {
    void Update(EntityManager& registry, float dt);

    void CreateExplosion(EntityManager& registry, Vec3 pos, int count, Vec3 color, float speed = 50.0f);
};