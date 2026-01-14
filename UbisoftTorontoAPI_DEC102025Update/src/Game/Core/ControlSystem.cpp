#include "ControlSystem.h"
#include "../../ContestAPI/app.h"
#include "../../ContestAPI/AppSettings.h"
#include "../../System/Component/Component.h"
#include "GenerateSystem.h"
#include <cmath>

void PlayerControl3D(EntityManager& registry, float dt, Camera3D& camera, float& nextSpawnZ) {
    float dtSec = dt / 1000.0f;
    float playerCurrentZ = 0.0f;
    
    // Constants for physics
    const float gravity = -980.0f;      // Gravity acceleration (pixels/s^2)
    const float jumpVelocity = 400.0f;  // Initial jump velocity
    const float forwardSpeed = 200.0f;  // Forward/backward speed
    const float strafeSpeed = 300.0f;   // Left/right strafe speed

    View<PlayerTag, Transform3D, Velocity3D> view(registry);
    for (EntityID id : view) {
        auto& playerTag = view.get<PlayerTag>(id);
        auto& playerTransform = view.get<Transform3D>(id);
        auto& vel = view.get<Velocity3D>(id);
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
            vel.vx += inputX * acceleration * dtSec;
            // Clamp to max speed
            if (vel.vx > 1.0f) vel.vx = 1.0f;
            if (vel.vx < -1.0f) vel.vx = -1.0f;
        } else {
            // Apply damping when no input
            vel.vx *= (1.0f - damping * dtSec);
            if (std::abs(vel.vx) < 0.01f) vel.vx = 0.0f;
        }
        
        if (inputZ != 0.0f) {
            vel.vz += inputZ * acceleration * dtSec;
            // Clamp to max speed
            if (vel.vz > 1.0f) vel.vz = 1.0f;
            if (vel.vz < -1.0f) vel.vz = -1.0f;
        } else {
            // Apply damping when no input
            vel.vz *= (1.0f - damping * dtSec);
            if (std::abs(vel.vz) < 0.01f) vel.vz = 0.0f;
        }

        // 2. Apply gravity
        vel.vy += gravity * dtSec;

        // 3. Handle jump input (only when on ground - collision system sets this flag)
        if (App::IsKeyPressed(App::KEY_SPACE) && playerTag.isOnGround) {
            vel.vy = jumpVelocity;
        }

        // 4. Apply velocity to position
        // CollisionSystem will detect and resolve any collisions after this
        pos.x += vel.vx * strafeSpeed * dtSec;
        pos.z += vel.vz * forwardSpeed * dtSec;
        pos.y += vel.vy * dtSec;

        // Record player's current Z position for map generation
        playerCurrentZ = pos.z;
    }

    // 5. Camera rotation control using arrow keys
    const float rotationSpeed = 2.0f; // Radians per second
    
    if (App::IsKeyPressed(App::KEY_LEFT)) {
        camera.rotationAngle += rotationSpeed * dtSec;
    }
    if (App::IsKeyPressed(App::KEY_RIGHT)) {
        camera.rotationAngle -= rotationSpeed * dtSec;
    }

    // 6. Update map generation system based on player position
    GenerateSystem::MapGenerationSystem(registry, playerCurrentZ, nextSpawnZ);
}

void ControlSystem::Update(EntityManager& registry, float dt, Camera3D& camera, float& nextSpawnZ)
{
    PlayerControl3D(registry, dt, camera, nextSpawnZ);
}
