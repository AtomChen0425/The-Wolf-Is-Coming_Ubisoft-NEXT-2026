#include "CameraSystem.h"
#include "../../System/Component/Component.h"

void CameraFollow3D(EntityManager& registry, Camera3D& camera) {
    View<PlayerTag, Transform3D, Velocity3D> view(registry);
    for (EntityID id : view) {
        auto& pos = view.get<Transform3D>(id);
        camera.x = pos.x + camera.followOffsetX; // X axis follows player
        camera.y = pos.y + camera.followOffsetY; // Y offset for camera height
        camera.z = pos.z + camera.followOffsetZ; // Z offset for camera distance behind player
    }
}

void CameraSystem::Update(EntityManager& registry, Camera3D& camera)
{
    CameraFollow3D(registry, camera);
}