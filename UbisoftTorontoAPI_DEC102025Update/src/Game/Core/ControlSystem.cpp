#include "ControlSystem.h"
#include "../../ContestAPI/app.h"
#include "../../ContestAPI/AppSettings.h"
#include "../../System/Component/Component.h"
#include "GenerateSystem.h"
#include <cmath>
#include "Math/Math.h"

void PlayerControl3D(EntityManager& registry, float dt, float& nextSpawnZ, Camera3D& camera, const GameSettings& settings) {
    float dtSec = dt / 1000.0f;
    float playerCurrentZ = 0.0f;
    
    // Constants for physics
    const float gravity = -980.0f;      // Gravity acceleration (pixels/s^2)
    const float jumpVelocity = 400.0f;  // Initial jump velocity
    float forwardSpeed = 200.0f;  // Forward/backward speed
    const float strafeSpeed = 300.0f;   // Left/right strafe speed
    const float rotationSpeed = 2.0f; // Radians per second
    
    View<PlayerTag, Transform3D, Velocity3D> view(registry);
    for (EntityID id : view) {
        auto& playerTag = view.get<PlayerTag>(id);
        auto& playerTransform = view.get<Transform3D>(id);
        auto& vel = view.get<Velocity3D>(id).vel;
        Vec3& pos = playerTransform.pos;
        forwardSpeed += playerTag.score;
        
        // === Camera Control Logic (moved from CameraSystem) ===
        if (settings.cameraControlMode == CameraControlMode::Mouse) {
            // Mouse control mode
            float mouseX, mouseY;
            App::GetMousePos(mouseX, mouseY);
            
            // Initialize mouse position on first frame
            if (!camera.mouseInitialized) {
                camera.lastMouseX = mouseX;
                camera.lastMouseY = mouseY;
                camera.mouseInitialized = true;
            }
            
            // Calculate mouse delta (sensitivity adjusted)
            const float mouseSensitivity = 0.003f;
            float deltaX = (mouseX - camera.lastMouseX) * mouseSensitivity;
            float deltaY = (mouseY - camera.lastMouseY) * mouseSensitivity;
            
            // Update camera rotation based on mouse movement
            camera.rotationAngle += deltaX;
            
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
        } else {
            // Arrow keys control mode
            if (App::IsKeyPressed(App::KEY_LEFT)) {
                camera.rotationAngle -= rotationSpeed * dtSec;
            }
            if (App::IsKeyPressed(App::KEY_RIGHT)) {
                camera.rotationAngle += rotationSpeed * dtSec;
            }
            if (App::IsKeyPressed(App::KEY_UP)) {
                camera.pitchAngle += rotationSpeed * dtSec;
                // Clamp pitch
                const float maxPitch = 1.4f;
                if (camera.pitchAngle > maxPitch) camera.pitchAngle = maxPitch;
            }
            if (App::IsKeyPressed(App::KEY_DOWN)) {
                camera.pitchAngle -= rotationSpeed * dtSec;
                // Clamp pitch
                const float minPitch = -1.4f;
                if (camera.pitchAngle < minPitch) camera.pitchAngle = minPitch;
            }
        }
        // === End Camera Control Logic ===
        
        // 1. Handle horizontal input (forward/backward and strafe)
        // Use damping for smooth movement instead of hard reset
        float inputX = 0.0f;
        float inputZ = 0.0f;
        
        if (App::IsKeyPressed(App::KEY_A)) inputX = -1.0f;
        if (App::IsKeyPressed(App::KEY_D)) inputX = 1.0f;
        if (App::IsKeyPressed(App::KEY_W)) inputZ = 1.0f;
        if (App::IsKeyPressed(App::KEY_S)) inputZ = -1.0f;
        
        // Apply input with smooth acceleration/deceleration
        const float acceleration = 2000.0f; // How fast to reach target speed
        const float damping = 8.0f;         // How fast to slow down when no input
        
        if (inputX != 0.0f) {
            vel.x += inputX * acceleration * dtSec;
            // Clamp to max speed
            if (vel.x > 1.0f) vel.x = 1.0f;
            if (vel.x < -1.0f) vel.x = -1.0f;
        } else {
            // Apply damping when no input
            vel.x *= (1.0f - damping * dtSec);
            if (std::abs(vel.x) < 0.01f) vel.x = 0.0f;
        }
        
        if (inputZ != 0.0f) {
            vel.z += inputZ * acceleration * dtSec;
            // Clamp to max speed
            if (vel.z > 1.0f) vel.z = 1.0f;
            if (vel.z < -1.0f) vel.z = -1.0f;
        } else {
            // Apply damping when no input
            vel.z *= (1.0f - damping * dtSec);
            if (std::abs(vel.z) < 0.01f) vel.z = 0.0f;
        }

        //// 2. Apply gravity
        //vel.y += gravity * dtSec;

        // 3. Handle jump input (only when on ground - collision system sets this flag)
        if (App::IsKeyPressed(App::KEY_SPACE) && playerTag.isOnGround) {
            vel.y = jumpVelocity;
        }

        // 4. Apply velocity to position
        // CollisionSystem will detect and resolve any collisions after this
		Vec3 dirSpeed = Vec3(strafeSpeed, 1.0f, forwardSpeed);
        float cosAngle = std::cos(playerTag.rotationY);
        float sinAngle = -std::sin(playerTag.rotationY);
		Vec3 localMove = vel * dirSpeed * dtSec;

        Vec4 worldMove4 = RotateByY(playerTag.rotationY, Vec4(localMove.x, localMove.y, localMove.z, 0.0f));
        pos += Vec3(worldMove4.x, worldMove4.y, worldMove4.z);
        // Record player's current Z position for map generation
            
        playerCurrentZ = pos.z;
    }
    
    // 6. Update map generation system based on player position
    GenerateSystem::MapGenerationSystem(registry, playerCurrentZ, nextSpawnZ);
}

void ControlSystem::Update(EntityManager& registry, float dt, float& nextSpawnZ, Camera3D& camera, const GameSettings& settings)
{
    PlayerControl3D(registry, dt, nextSpawnZ, camera, settings);
}
