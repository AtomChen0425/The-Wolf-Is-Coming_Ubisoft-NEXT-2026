#pragma once
#include "../System/ECS/ECS.h"

struct Camera25D {
    float x; // 对应世界的 x
    float y; // 对应世界的 z (注意！这里是 z)

    // 视口大小
    float width = 1024.0f;
    float height = 768.0f;

    // 关卡边界 (假设是一个很大的矩形区域)
    float worldMinX = 0.0f;
    float worldMaxX = 2000.0f;
    float worldMinZ = 0.0f;
    float worldMaxZ = 2000.0f; // 深度限制
};

struct Camera3D {
    float x, y, z; // 相机在世界中的绝对位置

    // 相机相对于玩家的固定偏移量 (配置项)
    // 放在玩家上方 200，后方 300 的位置
    float followOffsetY = 0.0f;
    float followOffsetZ = -300.0f;
    float followOffsetX = 0.0f;
};
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
    Camera3D camera; // 实例化
    float nextSpawnZ = 0.0f;
    //void UpdateCamera(EntityManager& registry, float dt); // 新增系统
};