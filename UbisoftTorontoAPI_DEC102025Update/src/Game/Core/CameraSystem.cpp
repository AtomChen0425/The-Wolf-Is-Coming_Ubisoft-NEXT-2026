#include "CameraSystem.h"
#include "../../System/Component/Component.h"
#include <cmath>

void CameraFollow3D(EntityManager& registry, Camera3D& camera) {
    View<PlayerTag, Transform3D, Velocity3D> view(registry);
    for (EntityID id : view) {
        auto& playerTransform = view.get<Transform3D>(id);
        auto& playerTag = view.get<PlayerTag>(id);
        
        Vec3 pos = playerTransform.pos;
        
        // Calculate camera position based on rotation angles
        // Camera follows player position and uses camera's own rotation angles
        float cosYaw = std::cos(camera.rotationAngle);
        float sinYaw = std::sin(camera.rotationAngle);
        float cosPitch = std::cos(camera.pitchAngle);
        float sinPitch = std::sin(camera.pitchAngle);
        
        // Rotate the offset (followOffsetX, followOffsetZ) around Y axis
        float rotatedOffsetX = camera.followOffsetX * cosYaw + camera.followOffsetZ * sinYaw;
        float rotatedOffsetZ = -camera.followOffsetX * sinYaw + camera.followOffsetZ * cosYaw;
        
        // Apply pitch to the camera offset
        float pitchAdjustedOffsetY = camera.followOffsetY + camera.followOffsetZ * sinPitch * 0.5f;
        float pitchAdjustedOffsetZ = rotatedOffsetZ * cosPitch;
        
        camera.x = pos.x + rotatedOffsetX;
        camera.y = pos.y + pitchAdjustedOffsetY;
        camera.z = pos.z + pitchAdjustedOffsetZ;
    }
}

void CameraSystem::Update(EntityManager& registry, Camera3D& camera)
{
    CameraFollow3D(registry, camera);
}