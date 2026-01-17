#pragma once
#include "../../System/ECS/ECS.h"
#include "../../System/ECSSystem.h"

namespace RenderSystem {
	void Render(EntityManager& registry);
	void Render(EntityManager& registry, Camera25D& camera);
	void Render(EntityManager& registry, Camera3D& camera);
	void Update(EntityManager& registry, const float deltaTimeMs);
	
	// Sprite rendering functions for 3D entities with proper depth sorting
	// These can be used to replace cube rendering for wolves, player, and sheep
	void RenderWolvesSprite(EntityManager& registry, Camera3D& camera);
	void RenderPlayerSprite(EntityManager& registry, Camera3D& camera);
	void RenderSheepSprite(EntityManager& registry, Camera3D& camera);
};