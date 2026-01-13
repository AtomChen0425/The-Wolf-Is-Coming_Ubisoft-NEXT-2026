#pragma once
#include "../../System/ECS/ECS.h"
#include "../../System/ECSSystem.h"

namespace ControlSystem
{
	void Update(EntityManager& registry);
	void Update(EntityManager& registry, float dtms, Camera3D& camera, float& nextSpawnZ);
};