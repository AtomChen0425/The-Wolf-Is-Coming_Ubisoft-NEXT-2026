#pragma once
#include "../../System/ECS/ECS.h"
#include "../../System/Component/Component.h"
#include "GameConfig.h"

namespace GenerateSystem
{
	void CreatePlayer(EntityManager& registry);
	void CreatePlayer3D(EntityManager& registry, const GameConfig& config);
	void SpawnEnemy(EntityManager& registry);
	void MapGenerationSystem(EntityManager& registry, float playerZ, float& nextSpawnZ, const GameConfig& config);
	
	// Chunk-based map generation for infinite 4-direction map
	void ChunkGenerationSystem(EntityManager& registry, float playerX, float playerZ, std::set<std::pair<int, int>>& loadedChunks, const GameConfig& config);
	void GenerateChunk(EntityManager& registry, int chunkX, int chunkZ, const GameConfig& config);
	void DespawnDistantChunks(EntityManager& registry, float playerX, float playerZ, std::set<std::pair<int, int>>& loadedChunks, const GameConfig& config);
	
	// Template-based map generation
	void GenerateMapFromTemplate(EntityManager& registry, const MapTemplate& mapTemplate, float startZ, const GameConfig& config);
	MapTemplate CreateDefaultTemplate(const GameConfig& config);
	MapTemplate CreateTestTemplate(const GameConfig& config);
	
	// Create a test enemy with AI components (for demonstration purposes)
	void CreateTestEnemyWithAI(EntityManager& registry, float x, float y, float z, bool enableMovement, bool enableShooting);
};