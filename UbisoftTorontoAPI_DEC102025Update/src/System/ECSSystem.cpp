#include "ECSSystem.h"
#include "Math/Vec2.h"
#include <cmath>
#include "../Game/Core/RenderSystem.h"
#include "../Game/Core/MovementSystem.h"
#include "../Game/Core/GenerateSystem.h"
#include "../Game/Core/ControlSystem.h"
#include "../Game/Core/CollisionSystem.h"
#include "../Game/Core/CameraSystem.h"
#include "Component/Component.h"
EngineSystem::EngineSystem()
    : registry(std::make_unique<EntityManager>())
{
}
float Lerp(float a, float b, float t) {
    return a + (b - a) * t;
}
//void EngineSystem::UpdateCamera(EntityManager& registry, float dt) {
//    // 1. Ѱ����� 2.5D
//    View<PlayerTag, Position3D> view(registry);
//    EntityID playerID = -1;
//    for (EntityID id : view) { playerID = id; break; }
//
//    if (playerID == -1) return;
//
//    auto& pos = view.get<Position3D>(playerID);
//
//    // 2. ����Ŀ��� (Target)
//    // Ŀ���ǰ���ҵġ��ŵס�(pos.x, pos.z) ������Ļ����
//    float targetX = pos.x - (camera.width / 2.0f);
//    float targetY = pos.z - (camera.height / 2.0f); // ע�⣺�� z ��Ӧ��Ļ y
//
//    // 3. ƽ���ƶ� (Lerp)
//    // t ������������ͺ�У�0.1 �Ƚ�ƽ����1.0 ������ҧס
//    float smoothSpeed = 5.0f * (dt / 1000.0f);
//    // ��ֹ dt �����´������ t �� 0~1
//    if (smoothSpeed > 1.0f) smoothSpeed = 1.0f;
//
//    camera.x = Lerp(camera.x, targetX, smoothSpeed);
//    camera.y = Lerp(camera.y, targetY, smoothSpeed);
//
//    // 4. �߽����� (Clamping)
//    // ��һ����֤��������ĵ���ͼ����ĺڱ�
//    // X ������
//    float maxCamX = camera.worldMaxX - camera.width;
//    camera.x = std::clamp(camera.x, camera.worldMinX, maxCamX);
//
//    // Y ������ (��Ӧ����� Z)
//    float maxCamY = camera.worldMaxZ - camera.height;
//    camera.y = std::clamp(camera.y, camera.worldMinZ, maxCamY);
//}
void EngineSystem::ResetGame() {
    // Reset the game: clear registry, reset score, create new player
    registry = std::make_unique<EntityManager>();
    gSpawnTimerMs = 0.0f;
    gScore = 0;

    nextSpawnZ = 0.0f; // Reset map generation pointer

    // Initialize camera position and offsets for 3D view
    // Camera should be behind and above the player
    camera.followOffsetX = 0.0f;    // No horizontal offset
    camera.followOffsetY = 200.0f;  // Height above player
    camera.followOffsetZ = -300.0f; // Distance behind player

    // Set initial camera position
    camera.x = 0.0f;
    camera.y = camera.followOffsetY;
    camera.z = camera.followOffsetZ;

    // Create the player
    GenerateSystem::CreatePlayer3D(*registry);

    // Generate initial map
    GenerateSystem::MapGenerationSystem(*registry, 0.0f, nextSpawnZ);
}
void EngineSystem::Update(const float deltaTimeMs) {
    if (!registry) return;
    
    // Update player control (handles input and movement)
    ControlSystem::Update(*registry, deltaTimeMs, camera, nextSpawnZ);
    
    // Update camera to follow player
    CameraSystem::Update(*registry, camera);
}
void EngineSystem::Render() {
    if (!registry) return;
    
    // Render the 3D scene with camera
    RenderSystem::Render(*registry, camera);
}
void EngineSystem::Shutdown() {
    // Clean up resources
    View<SpriteComponent> view(*registry);
    for (EntityID id : view) {
        auto& spr = view.get<SpriteComponent>(id);
        delete spr.sprite;
    }
    registry.reset();
}