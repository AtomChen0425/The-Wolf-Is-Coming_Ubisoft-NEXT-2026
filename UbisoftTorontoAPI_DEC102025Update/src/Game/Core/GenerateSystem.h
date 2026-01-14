#pragma once
#include "../../System/ECS/ECS.h"

namespace GenerateSystem
{
	void CreatePlayer(EntityManager& registry);
	void CreatePlayer3D(EntityManager& registry);
	void SpawnEnemy(EntityManager& registry);
	void MapGenerationSystem(EntityManager& registry, float playerZ, float& nextSpawnZ);
	
	// Template-based map generation
	void GenerateMapFromTemplate(EntityManager& registry, const MapTemplate& mapTemplate, float startZ);
	MapTemplate CreateDefaultTemplate();
	MapTemplate CreateTestTemplate();
};