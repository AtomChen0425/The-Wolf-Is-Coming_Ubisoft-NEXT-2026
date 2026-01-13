#include "ControlSystem.h"
#include "../../ContestAPI/app.h"
#include "../../ContestAPI/AppSettings.h"
#include "../../System/Component/Component.h"
#include "GenerateSystem.h"

void PlayerControl3D(EntityManager& registry, float dt, Camera3D& camera, float& nextSpawnZ) {
    float dtSec = dt / 1000.0f;
    float playerCurrentZ = 0.0f;
    
    // Constants for physics
    const float gravity = -980.0f;      // Gravity acceleration (pixels/s^2)
    const float groundLevel = 0.0f;     // Ground Y position
    const float jumpVelocity = 400.0f;  // Initial jump velocity
    const float forwardSpeed = 200.0f;  // Forward/backward speed
    const float strafeSpeed = 300.0f;   // Left/right strafe speed
    
    // Road constraints (5 blocks * 20 units = 100 units wide, centered at X=0)
    const float roadHalfWidth = 50.0f;  // Half width of the road
    const float roadMinX = -roadHalfWidth + 10.0f; // Leave 10 units padding
    const float roadMaxX = roadHalfWidth - 10.0f;  // Leave 10 units padding

    View<PlayerTag, Transform3D, Velocity3D> view(registry);
    for (EntityID id : view) {
        auto& pos = view.get<Transform3D>(id);
        auto& vel = view.get<Velocity3D>(id);

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

        // 3. Handle jump input (only when on ground)
        if (App::IsKeyPressed(App::KEY_SPACE) && pos.y <= groundLevel + 0.1f) {
            vel.vy = jumpVelocity;
        }

        // 4. Update position based on velocity
        pos.z += vel.vz * forwardSpeed * dtSec;
        pos.x += vel.vx * strafeSpeed * dtSec;
        pos.y += vel.vy * dtSec;

        // 5. Keep player on the road (boundary constraints)
        if (pos.x < roadMinX) {
            pos.x = roadMinX;
            vel.vx = 0.0f; // Stop horizontal movement when hitting boundary
        }
        if (pos.x > roadMaxX) {
            pos.x = roadMaxX;
            vel.vx = 0.0f;
        }

        // 6. Ground collision
        if (pos.y < groundLevel) {
            pos.y = groundLevel;
            vel.vy = 0.0f;
        }

        // Record player's current Z position for map generation
        playerCurrentZ = pos.z;
    }

    // 7. Update map generation system based on player position
    GenerateSystem::MapGenerationSystem(registry, playerCurrentZ, nextSpawnZ);
}

void ControlSystem::Update(EntityManager& registry, float dt, Camera3D& camera, float& nextSpawnZ)
{
    PlayerControl3D(registry, dt, camera, nextSpawnZ);
}
