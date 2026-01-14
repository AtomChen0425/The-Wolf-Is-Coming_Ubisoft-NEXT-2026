#include "CameraSystem.h"
#include "../../System/Component/Component.h"
#include <cmath>

void CameraFollow3D(EntityManager& registry, Camera3D& camera) {
    View<PlayerTag, Transform3D, Velocity3D> view(registry);
    for (EntityID id : view) {
        auto& playerTransform = view.get<Transform3D>(id);
		Vec3 pos = playerTransform.pos;
        // Calculate camera position based on rotation angle
        // Rotate the offset vector around the player
        float cosAngle = std::cos(camera.rotationAngle);
        float sinAngle = std::sin(camera.rotationAngle);
        
        // Rotate the offset (followOffsetX, followOffsetZ) around Y axis
        float rotatedOffsetX = camera.followOffsetX * cosAngle - camera.followOffsetZ * sinAngle;
        float rotatedOffsetZ = camera.followOffsetX * sinAngle + camera.followOffsetZ * cosAngle;
        
        camera.x = pos.x + rotatedOffsetX;
        camera.y = pos.y + camera.followOffsetY; // Y offset for camera height
        camera.z = pos.z + rotatedOffsetZ;
    }
}

void CameraSystem::Update(EntityManager& registry, Camera3D& camera)
{
    CameraFollow3D(registry, camera);
}