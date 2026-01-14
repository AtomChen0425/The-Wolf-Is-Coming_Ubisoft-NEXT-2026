#include "ControlSystem.h"
#include "../../ContestAPI/app.h"
#include "../../ContestAPI/AppSettings.h"
#include "../../System/Component/Component.h"
#include "GenerateSystem.h"
#include <cmath>
#include "Math/Math.h"

void PlayerControl3D(EntityManager& registry, float dt, float& nextSpawnZ) {
    float dtSec = dt / 1000.0f;
    float playerCurrentZ = 0.0f;
    
    // Constants for physics
    const float gravity = -980.0f;      // Gravity acceleration (pixels/s^2)
    const float jumpVelocity = 400.0f;  // Initial jump velocity
    const float forwardSpeed = 200.0f;  // Forward/backward speed
    const float strafeSpeed = 300.0f;   // Left/right strafe speed
    const float rotationSpeed = 2.0f; // Radians per second
    View<PlayerTag, Transform3D, Velocity3D> view(registry);
    for (EntityID id : view) {
        auto& playerTag = view.get<PlayerTag>(id);
        auto& playerTransform = view.get<Transform3D>(id);
        auto& vel = view.get<Velocity3D>(id).vel;
        Vec3& pos = playerTransform.pos;
        
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

        Mat4 rotY;
        // Build a pure Y-rotation matrix (keep translation = 0)
        // Note: Mat4 is row-major in your operator*(Vec4) implementation.
        rotY.m[0][0] = cosAngle; rotY.m[0][1] = 0.0f; rotY.m[0][2] = sinAngle; rotY.m[0][3] = 0.0f;
        rotY.m[1][0] = 0.0f;    rotY.m[1][1] = 1.0f; rotY.m[1][2] = 0.0f;     rotY.m[1][3] = 0.0f;
        rotY.m[2][0] = -sinAngle; rotY.m[2][1] = 0.0f; rotY.m[2][2] = cosAngle; rotY.m[2][3] = 0.0f;
        rotY.m[3][0] = 0.0f;    rotY.m[3][1] = 0.0f; rotY.m[3][2] = 0.0f;     rotY.m[3][3] = 1.0f;

        Vec4 worldMove4 = rotY * Vec4(localMove.x, localMove.y, localMove.z, 0.0f);
        pos += Vec3(worldMove4.x, worldMove4.y, worldMove4.z);
        // Record player's current Z position for map generation

        if (App::IsKeyPressed(App::KEY_LEFT)) {
            playerTag.rotationY += rotationSpeed * dtSec;
        }
        if (App::IsKeyPressed(App::KEY_RIGHT)) {
            playerTag.rotationY -= rotationSpeed * dtSec;
        }
            
        playerCurrentZ = pos.z;
    }
    
    // 6. Update map generation system based on player position
    GenerateSystem::MapGenerationSystem(registry, playerCurrentZ, nextSpawnZ);
}

void ControlSystem::Update(EntityManager& registry, float dt, float& nextSpawnZ)
{
    PlayerControl3D(registry, dt, nextSpawnZ);
}
