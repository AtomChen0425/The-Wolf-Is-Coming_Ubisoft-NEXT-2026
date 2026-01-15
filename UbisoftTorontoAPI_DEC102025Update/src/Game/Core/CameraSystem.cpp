#include "CameraSystem.h"
#include "../../System/Component/Component.h"
#include "../../ContestAPI/app.h"
#include "../../ContestAPI/AppSettings.h"
#include <cmath>

void CameraFollow3D(EntityManager& registry, Camera3D& camera) {
    View<PlayerTag, Transform3D, Velocity3D> view(registry);
    for (EntityID id : view) {
        auto& playerTransform = view.get<Transform3D>(id);
        auto& playerTag = view.get<PlayerTag>(id);
        
        // Get mouse position for camera control
        float mouseX, mouseY;
        App::GetMousePos(mouseX, mouseY);
        
        // Initialize mouse position on first frame
        if (!camera.mouseInitialized) {
            camera.lastMouseX = mouseX;
            camera.lastMouseY = mouseY;
            camera.mouseInitialized = true;
        }
        
        // Calculate mouse delta (sensitivity adjusted)
        const float mouseSensitivity = 0.003f; // Adjust this for faster/slower mouse movement
        float deltaX = (mouseX - camera.lastMouseX) * mouseSensitivity;
        float deltaY = (mouseY - camera.lastMouseY) * mouseSensitivity;
        
        // Update camera rotation based on mouse movement
        // Horizontal mouse movement controls yaw (rotation around Y axis)
        camera.rotationAngle += deltaX;
        playerTag.rotationY = -camera.rotationAngle;
        
        // Vertical mouse movement controls pitch (up/down viewing)
        camera.pitchAngle -= deltaY; // Subtract because screen Y is inverted
        
        // Clamp pitch to prevent camera flipping
        const float maxPitch = 1.4f; // ~80 degrees
        const float minPitch = -1.4f; // ~-80 degrees
        if (camera.pitchAngle > maxPitch) camera.pitchAngle = maxPitch;
        if (camera.pitchAngle < minPitch) camera.pitchAngle = minPitch;
        
        // Store current mouse position for next frame
        camera.lastMouseX = mouseX;
        camera.lastMouseY = mouseY;
        
        Vec3 pos = playerTransform.pos;
        
        // Calculate camera position based on rotation angles
        // First rotate around Y axis (yaw), then apply pitch
        float cosYaw = std::cos(playerTag.rotationY);
        float sinYaw = -std::sin(playerTag.rotationY);
        float cosPitch = std::cos(camera.pitchAngle);
        float sinPitch = std::sin(camera.pitchAngle);
        
        // Rotate the offset (followOffsetX, followOffsetZ) around Y axis
        float rotatedOffsetX = camera.followOffsetX * cosYaw - camera.followOffsetZ * sinYaw;
        float rotatedOffsetZ = camera.followOffsetX * sinYaw + camera.followOffsetZ * cosYaw;
        
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