#pragma once
#include "../System/ECS/ECS.h"
namespace RenderSystem {
	void Render(EntityManager& registry);
	void Update(EntityManager& registry, const float deltaTimeMs);
};