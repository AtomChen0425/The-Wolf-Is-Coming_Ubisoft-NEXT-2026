#pragma once
#include "../System/ECS/ECS.h"
#include "../System/Component/Component.h"

namespace GenerateSystem
{
	void CreatePlayer(EntityManager& registry);
	void SpawnEnemy(EntityManager& registry);
	void SpawnEnemyOfType(EntityManager& registry, EnemyType type);
	void CreateBullet(EntityManager& registry, Vec2 position, Vec2 direction, float speed, float damage);
};