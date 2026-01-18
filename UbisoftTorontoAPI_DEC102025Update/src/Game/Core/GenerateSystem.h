#pragma once
#include "../../System/ECS/ECS.h"
#include "../../System/Component/Component.h"
#include "GameConfig.h"
#include <set>

namespace GenerateSystem
{
	//void CreatePlayer25D(EntityManager& registry);
	void CreatePlayer3D(EntityManager& registry);
	void SpawnEnemy(EntityManager& registry);
	void MapGenerationSystem(EntityManager& registry, float playerZ, float& nextSpawnZ);

	// Chunk-based map generation for infinite 4-direction map
	void ChunkGenerationSystem(EntityManager& registry, float playerX, float playerZ, std::set<std::pair<int, int>>& loadedChunks);

	// Template-based map generation
	void GenerateMapFromTemplate(EntityManager& registry, const MapTemplate& mapTemplate, float startZ);
	MapTemplate CreateDefaultTemplate();
	MapTemplate CreateTestTemplate();


	// Wolf generation functions
	void GenerateWolf(EntityManager& registry);  // Generate basic wolf (backward compatibility)
	void GenerateWolfOfType(EntityManager& registry, WolfType type);  // Generate specific wolf type
};