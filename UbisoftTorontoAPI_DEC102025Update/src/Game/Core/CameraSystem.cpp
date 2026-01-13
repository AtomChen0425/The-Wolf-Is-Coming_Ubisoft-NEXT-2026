#include "CameraSystem.h"
#include "../System/Component/Component.h"
void CameraFollow3D(EntityManager& registry,Camera3D& camera) {

    View<PlayerTag, Transform3D, Velocity3D> view(registry);
    for (EntityID id : view) {
        auto& pos = view.get<Transform3D>(id);
        camera.x = pos.x; // X轴跟随玩家 (可选，也可以设为 0 保持居中)
        camera.y = pos.y - camera.followOffsetY; // 保持在玩家上方高度
        camera.z = pos.z - abs(camera.followOffsetZ); // 保持在玩家后方距离
    }
}
void CameraSystem::Update(EntityManager& registry, Camera3D& camera)
{
    CameraFollow3D( registry, camera);
}