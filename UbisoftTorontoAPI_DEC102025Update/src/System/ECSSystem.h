#pragma once
#include "../System/ECS/ECS.h"

class EngineSystem
{
public:
    EngineSystem();
    ~EngineSystem() = default;
    
    float gSpawnTimerMs = 0.0f;
    int   gScore = 0;
    //void SpawnEnemy(EntityManager& registry);
    void Update(const float deltaTimeMs);
    void Render();
    void Shutdown();
    void ResetGame();
private:
    std::unique_ptr<EntityManager> registry;
};