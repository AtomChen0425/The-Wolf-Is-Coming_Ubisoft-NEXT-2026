#pragma once
#include "../../System/ECS/ECS.h"
#include "../../System/ECSSystem.h"

namespace ControlSystem
{
	void Update(EntityManager& registry, float dtms, float& nextSpawnZ, Camera3D& camera, const GameSettings& settings);
};