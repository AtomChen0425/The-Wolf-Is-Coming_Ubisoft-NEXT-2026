#pragma once
#include "../System/ECS/ECS.h"
#include "../System/ECSSystem.h"
namespace RenderSystem {
	void Render(EntityManager& registry);
	void Render(EntityManager& registry, Camera25D& camera);
	void Update(EntityManager& registry, const float deltaTimeMs);
};