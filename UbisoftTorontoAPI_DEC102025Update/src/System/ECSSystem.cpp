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
float Lerp(float a, float b, float t) {
    return a + (b - a) * t;
}
void EngineSystem::UpdateCamera(EntityManager& registry, float dt) {
    // 1. 寻找玩家
    View<PlayerTag, Position3D> view(registry);
    EntityID playerID = -1;
    for (EntityID id : view) { playerID = id; break; }

    if (playerID == -1) return;

    auto& pos = view.get<Position3D>(playerID);

    // 2. 计算目标点 (Target)
    // 目标是把玩家的【脚底】(pos.x, pos.z) 放在屏幕中心
    float targetX = pos.x - (camera.width / 2.0f);
    float targetY = pos.z - (camera.height / 2.0f); // 注意：用 z 对应屏幕 y

    // 3. 平滑移动 (Lerp)
    // t 决定了相机的滞后感，0.1 比较平滑，1.0 是死死咬住
    float smoothSpeed = 5.0f * (dt / 1000.0f);
    // 防止 dt 过大导致穿帮，限制 t 在 0~1
    if (smoothSpeed > 1.0f) smoothSpeed = 1.0f;

    camera.x = Lerp(camera.x, targetX, smoothSpeed);
    camera.y = Lerp(camera.y, targetY, smoothSpeed);

    // 4. 边界限制 (Clamping)
    // 这一步保证相机不会拍到地图外面的黑边
    // X 轴限制
    float maxCamX = camera.worldMaxX - camera.width;
    camera.x = std::clamp(camera.x, camera.worldMinX, maxCamX);

    // Y 轴限制 (对应世界的 Z)
    float maxCamY = camera.worldMaxZ - camera.height;
    camera.y = std::clamp(camera.y, camera.worldMinZ, maxCamY);
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
    UpdateCamera(*registry, deltaTimeMs);
	RenderSystem::Update(*registry, deltaTimeMs);
	
}
void EngineSystem::Render() {
    if (!registry) return;
    //RenderSystem::Render(*registry);
    RenderSystem::Render(*registry, camera);
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