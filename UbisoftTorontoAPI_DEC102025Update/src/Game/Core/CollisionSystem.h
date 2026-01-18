#pragma once
#include "../../System/ECS/ECS.h"
#include "../../Game/Core/GameConfig.h"
namespace CollisionSystem
{
	void Update(EntityManager& registry, const GameConfig& config);
};