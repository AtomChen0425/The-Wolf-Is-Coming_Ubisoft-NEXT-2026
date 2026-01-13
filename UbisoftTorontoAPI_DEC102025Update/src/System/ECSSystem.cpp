#include "ECSSystem.h"
#include "Math/Vec2.h"
#include <cmath>
#include "../Game/Core/RenderSystem.h"
#include "../Game/Core/MovementSystem.h"
#include "../Game/Core/GenerateSystem.h"
#include "../Game/Core/ControlSystem.h"
#include "../Game/Core/CollisionSystem.h"
#include "../Game/Core/CameraSystem.h"
#include "../System/Component/Component.h"
EngineSystem::EngineSystem()
    : registry(std::make_unique<EntityManager>())
{
}
float Lerp(float a, float b, float t) {
    return a + (b - a) * t;
}
//void EngineSystem::UpdateCamera(EntityManager& registry, float dt) {
//    // 1. 寻找玩家 2.5D
//    View<PlayerTag, Position3D> view(registry);
//    EntityID playerID = -1;
//    for (EntityID id : view) { playerID = id; break; }
//
//    if (playerID == -1) return;
//
//    auto& pos = view.get<Position3D>(playerID);
//
//    // 2. 计算目标点 (Target)
//    // 目标是把玩家的【脚底】(pos.x, pos.z) 放在屏幕中心
//    float targetX = pos.x - (camera.width / 2.0f);
//    float targetY = pos.z - (camera.height / 2.0f); // 注意：用 z 对应屏幕 y
//
//    // 3. 平滑移动 (Lerp)
//    // t 决定了相机的滞后感，0.1 比较平滑，1.0 是死死咬住
//    float smoothSpeed = 5.0f * (dt / 1000.0f);
//    // 防止 dt 过大导致穿帮，限制 t 在 0~1
//    if (smoothSpeed > 1.0f) smoothSpeed = 1.0f;
//
//    camera.x = Lerp(camera.x, targetX, smoothSpeed);
//    camera.y = Lerp(camera.y, targetY, smoothSpeed);
//
//    // 4. 边界限制 (Clamping)
//    // 这一步保证相机不会拍到地图外面的黑边
//    // X 轴限制
//    float maxCamX = camera.worldMaxX - camera.width;
//    camera.x = std::clamp(camera.x, camera.worldMinX, maxCamX);
//
//    // Y 轴限制 (对应世界的 Z)
//    float maxCamY = camera.worldMaxZ - camera.height;
//    camera.y = std::clamp(camera.y, camera.worldMinZ, maxCamY);
//}
void EngineSystem::ResetGame() {
    // 实现游戏重置逻辑，例如清空实体，重新创建玩家等
    registry = std::make_unique<EntityManager>();
    gSpawnTimerMs = 0.0f;
    gScore = 0;
    //GenerateSystem::CreatePlayer(*registry);


    nextSpawnZ = 0.0f; // 重置地图生成点

    // 初始化相机位置 (避免第一帧闪烁)
    camera.x = 0;
    camera.y = camera.followOffsetY;
    camera.z = camera.followOffsetZ;
	GenerateSystem::CreatePlayer3D(*registry);

    float targetScreenX = 200.0f;
    float targetScreenY = 100.0f;

    // 2. 屏幕中心 (假设窗口 1024x768)
    float screenCenterX = 1024.0f / 2.0f; // 512
    float screenCenterY = 768.0f / 2.0f;  // 384

    // 3. 投影参数 (必须和 RenderSystem3D 里的 fov 一致)
    float fov = 600.0f;
    // 玩家到相机的距离 (取绝对值)
    float dist = std::abs(camera.followOffsetZ); // 300

    // 4. 逆向推导相机偏移
    // 公式原理：偏移量 = (目标屏幕 - 中心) * (距离 / FOV)

    // X轴推导: 
    // 因为相机往右移(X+)，物体在屏幕上会往左(X-)，所以是反向关系
    // 如果想要物体在 400 (左边)，相机得往右移 (512 - 400) 的量
    float screenDiffX = screenCenterX - targetScreenX; // 512 - 400 = 112
    camera.followOffsetX = screenDiffX * (dist / fov); // 112 * (300/600) = 56

    // Y轴推导:
    // 屏幕Y向下是正。物体在 400 (偏下)，相机需要往上抬还是往下压？
    // 3D世界Y向上是正。
    float screenDiffY = screenCenterY - targetScreenY; // 384 - 400 = -16
    // 这里的正负号比较绕，通常需要试一下，根据之前的公式：
    // outY = -ry * ... -> ry = -outY ...
    // 简单来说：(384 - 400) * 0.5 = -8。
    // 我们需要在原有的高度基础上，再调整 -8
    float baseHeight = 200.0f; // 基础高度
    camera.followOffsetY = baseHeight + (screenDiffY * (dist / fov));


    // 5. 初始化相机位置
    camera.x = camera.followOffsetX;
    camera.y = camera.followOffsetY;
    camera.z = camera.followOffsetZ;
	GenerateSystem::MapGenerationSystem(*registry, 0.0f, nextSpawnZ);
}
void EngineSystem::Update(const float deltaTimeMs) {
    if (!registry) return;
    // ----- Spawn enemies -----
    /*gSpawnTimerMs += deltaTimeMs;
    float spawnInterval = std::fmax(280.0f, 1200.0f);
    if (gSpawnTimerMs >= spawnInterval)
    {
        gSpawnTimerMs = 0.0f;
        GenerateSystem::SpawnEnemy(*registry);
    }
    */
    //ControlSystem::Update(*registry);
    //CollisionSystem::Update(*registry);
    //MovementSystem::Update(*registry, deltaTimeMs);
    //UpdateCamera(*registry, deltaTimeMs);
	ControlSystem::Update(*registry, deltaTimeMs, camera, nextSpawnZ);
    CameraSystem::Update(*registry, camera);
	//RenderSystem::Update(*registry, deltaTimeMs);
	
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