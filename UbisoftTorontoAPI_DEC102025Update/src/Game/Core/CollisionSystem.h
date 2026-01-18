#pragma once
#include "../../System/ECS/ECS.h"
#include "../../Game/Core/GameConfig.h"
namespace CollisionSystem
{
	void Update(EntityManager& registry);
	void OnMapGenerated(EntityManager& registry, const std::vector<EntityID>& newBlocks);
	void ResetCollisionGrid();
};