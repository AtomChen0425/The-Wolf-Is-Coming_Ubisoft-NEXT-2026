#include "ECSSystem.h"
#include "Math/Vec2.h"
#include <cmath>
#include "../Game/Core/RenderSystem.h"
#include "../Game/Core/MovementSystem.h"
#include "../Game/Core/GenerateSystem.h"
#include "../Game/Core/ControlSystem.h"
#include "../Game/Core/CollisionSystem.h"
#include "../System/Component/Component.h"
EngineSystem::EngineSystem()
    : registry(std::make_unique<EntityManager>())
{
}
void EngineSystem::ResetGame() {
    // 实现游戏重置逻辑，例如清空实体，重新创建玩家等
    registry = std::make_unique<EntityManager>();
    gSpawnTimerMs = 0.0f;
    gScore = 0;
    GenerateSystem::CreatePlayer(*registry);
}
void EngineSystem::Update(const float deltaTimeMs) {
    if (!registry) return;
    // ----- Spawn enemies -----
    gSpawnTimerMs += deltaTimeMs;
    float spawnInterval = std::fmax(280.0f, 1200.0f);
    if (gSpawnTimerMs >= spawnInterval)
    {
        gSpawnTimerMs = 0.0f;
        GenerateSystem::SpawnEnemy(*registry);
    }
    
    ControlSystem::Update(*registry);
    CollisionSystem::Update(*registry);
    MovementSystem::Update(*registry, deltaTimeMs);
	RenderSystem::Update(*registry, deltaTimeMs);
	
}
void EngineSystem::Render() {
    if (!registry) return;
    RenderSystem::Render(*registry);
}
void EngineSystem::Shutdown() {
    // 清理资源
    View<SpriteComponent> view(*registry);
    for (EntityID id : view) {
        auto& spr = view.get<SpriteComponent>(id);
        delete spr.sprite;
    }
    registry.reset();
}