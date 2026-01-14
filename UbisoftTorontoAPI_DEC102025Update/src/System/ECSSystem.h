#pragma once
#include "ECS/ECS.h"

struct Camera25D {
    float x; // World X coordinate
    float y; // World Z coordinate (note: maps to z)

    // Viewport size
    float width = 1024.0f;
    float height = 768.0f;

    // World boundaries
    float worldMinX = 0.0f;
    float worldMaxX = 2000.0f;
    float worldMinZ = 0.0f;
    float worldMaxZ = 2000.0f;
};

struct Camera3D {
    float x, y, z; // Camera position in world space

    // Fixed offset for following target (in camera space)
    // For example: 200 units up, 300 units back
    float followOffsetY = 0.0f;
    float followOffsetZ = -700.0f;
    float followOffsetX = 0.0f;
    
    // Camera rotation angle in radians (rotation around Y axis)
    // 0 = looking along +Z, PI/2 = looking along +X
    float rotationAngle = 0.0f;
};

class EngineSystem
{
public:
    EngineSystem();
    ~EngineSystem() = default;
    
    float gSpawnTimerMs = 0.0f;
    int   gScore = 0;
    
    void Update(const float deltaTimeMs);
    void Render();
    void Shutdown();
    void ResetGame();
    
private:
    std::unique_ptr<EntityManager> registry;
    Camera3D camera;
    float nextSpawnZ = 0.0f;
};