#pragma once
#include "../../System/ECS/ECS.h"
#include "../../System/ECSSystem.h"
#include "../../System/Component/Component.h"

namespace RenderSystem {
	void Render(EntityManager& registry);
	void Render(EntityManager& registry, Camera25D& camera);
	void Render(EntityManager& registry, Camera3D& camera);
	void Update(EntityManager& registry, const float deltaTimeMs);

	// Crosshair rendering
	void RenderCrosshair(EntityManager& registry, Camera3D& camera);

	void RenderSpriteByPos(Transform3D& transform, SpriteComponent& sprite, Camera3D& camera);
};