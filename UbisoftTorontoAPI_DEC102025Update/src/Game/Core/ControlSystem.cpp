#include "ControlSystem.h"
#include "../../ContestAPI/app.h"
#include "../../ContestAPI/AppSettings.h"
#include "../../System/Component/Component.h"
#include "../../System/Physic/Collision.h"
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
    
    // Road constraints (5 blocks * 20 units = 100 units wide, centered at X=0)
    const float roadHalfWidth = 50.0f;  // Half width of the road
    const float roadMinX = -roadHalfWidth + 10.0f; // Leave 10 units padding
    const float roadMaxX = roadHalfWidth - 10.0f;  // Leave 10 units padding

    Collision collision;

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

        // 3. Check if player is on ground (for jump detection)
        bool isOnGround = false;
        Vec3 playerMin(pos.x - playerTransform.width / 2, pos.y - playerTransform.height / 2, pos.z - playerTransform.depth / 2);
        Vec3 playerMax(pos.x + playerTransform.width / 2, pos.y + playerTransform.height / 2, pos.z + playerTransform.depth / 2);

        View<Collider3D, Transform3D> floorView(registry);
        for (EntityID floorId : floorView) {
            auto& floorCollider = floorView.get<Collider3D>(floorId);
            if (!floorCollider.isFloor) continue;

            auto& floorTransform = floorView.get<Transform3D>(floorId);
            Vec3 floorPos = floorTransform.pos;
            Vec3 floorMin(floorPos.x - floorTransform.width / 2, floorPos.y - floorTransform.height / 2, floorPos.z - floorTransform.depth / 2);
            Vec3 floorMax(floorPos.x + floorTransform.width / 2, floorPos.y + floorTransform.height / 2, floorPos.z + floorTransform.depth / 2);

            // Check if player is above this floor block
            playerMin = Vec3(pos.x - playerTransform.width / 2, pos.y - playerTransform.height / 2, pos.z - playerTransform.depth / 2);
            playerMax = Vec3(pos.x + playerTransform.width / 2, pos.y + playerTransform.height / 2, pos.z + playerTransform.depth / 2);

            if (playerMin.x < floorMax.x && playerMax.x > floorMin.x &&
                playerMin.z < floorMax.z && playerMax.z > floorMin.z &&
                playerMin.y <= floorMax.y + 0.1f && playerMin.y >= floorMax.y - 5.0f) {
                isOnGround = true;
				break; // No need to check further
            }
        }

        // 4. Handle jump input (only when on ground)
        if (App::IsKeyPressed(App::KEY_SPACE) && isOnGround) {
            vel.vy = jumpVelocity;
        }

        // 5. Calculate next position based on velocity
        float nextX = pos.x + vel.vx * strafeSpeed * dtSec;
        float nextZ = pos.z + vel.vz * forwardSpeed * dtSec;
        float nextY = pos.y + vel.vy * dtSec;

        // 6. Check wall collisions for horizontal movement
        Vec3 nextPlayerMin(nextX - playerTransform.width / 2, pos.y - playerTransform.height / 2, nextZ - playerTransform.depth / 2);
        Vec3 nextPlayerMax(nextX + playerTransform.width / 2, pos.y + playerTransform.height / 2, nextZ + playerTransform.depth / 2);
        
        bool collisionX = false;
        bool collisionZ = false;
        
        View<Collider3D, Transform3D> wallView(registry);
        for (EntityID wallId : wallView) {
            auto& wallCollider = wallView.get<Collider3D>(wallId);
            if (!wallCollider.isWall) continue; // Skip non-wall objects
            
            auto& wallTransform = wallView.get<Transform3D>(wallId);
			Vec3 wallPos = wallTransform.pos;
            Vec3 wallMin(wallPos.x - wallTransform.width / 2, wallPos.y - wallTransform.height / 2, wallPos.z - wallTransform.depth / 2);
            Vec3 wallMax(wallPos.x + wallTransform.width / 2, wallPos.y + wallTransform.height / 2, wallPos.z + wallTransform.depth / 2);
            
            // Check collision with wall for X movement
            Vec3 testXMin(nextX - playerTransform.width / 2, pos.y - playerTransform.height / 2, pos.z - playerTransform.depth / 2);
            Vec3 testXMax(nextX + playerTransform.width / 2, pos.y + playerTransform.height / 2, pos.z + playerTransform.depth / 2);
            if (collision.AABB3D(testXMin, testXMax, wallMin, wallMax)) {
                collisionX = true;
            }
            
            // Check collision with wall for Z movement
            Vec3 testZMin(pos.x - playerTransform.width / 2, pos.y - playerTransform.height / 2, nextZ - playerTransform.depth / 2);
            Vec3 testZMax(pos.x + playerTransform.width / 2, pos.y + playerTransform.height / 2, nextZ + playerTransform.depth / 2);
            if (collision.AABB3D(testZMin, testZMax, wallMin, wallMax)) {
                collisionZ = true;
            }
        }

        // 7. Update position based on velocity and collision results
        if (!collisionX) {
            pos.x = nextX;
        } else {
            vel.vx = 0.0f; // Stop horizontal movement when hitting wall
        }
        
        if (!collisionZ) {
            pos.z = nextZ;
        } else {
            vel.vz = 0.0f; // Stop forward movement when hitting wall
        }
        
        pos.y = nextY;

        // 8. Ground collision - check all floors and find the highest one player is on
        float groundY = -1000.0f; // Default very low ground
        View<Collider3D, Transform3D> floorCheckView(registry);
        for (EntityID floorId : floorCheckView) {
            auto& floorCollider = floorCheckView.get<Collider3D>(floorId);
            if (!floorCollider.isFloor) continue;
            
            auto& floorTransform = floorCheckView.get<Transform3D>(floorId);
			Vec3 floorPos = floorTransform.pos;
            Vec3 floorMin(floorPos.x - floorTransform.width / 2, floorPos.y - floorTransform.height / 2, floorPos.z - floorTransform.depth / 2);
            Vec3 floorMax(floorPos.x + floorTransform.width / 2, floorPos.y + floorTransform.height / 2, floorPos.z + floorTransform.depth / 2);
            
            // Check if player is above this floor block
            playerMin = Vec3(pos.x - playerTransform.width / 2, pos.y - playerTransform.height / 2, pos.z - playerTransform.depth / 2);
            playerMax = Vec3(pos.x + playerTransform.width / 2, pos.y + playerTransform.height / 2, pos.z + playerTransform.depth / 2);
            
            if (playerMin.x < floorMax.x && playerMax.x > floorMin.x &&
                playerMin.z < floorMax.z && playerMax.z > floorMin.z) {
                // Player is horizontally aligned with this floor
                float floorTop = floorMax.y;
                if (floorTop > groundY) {
                    groundY = floorTop;
                }
            }
        }
        
        // Apply ground collision
        if (pos.y - playerTransform.height / 2 < groundY) {
            pos.y = groundY + playerTransform.height / 2;
            vel.vy = 0.0f;
        }

        // Record player's current Z position for map generation
        playerCurrentZ = pos.z;
    }

    // 9. Camera rotation control using arrow keys
    const float rotationSpeed = 2.0f; // Radians per second
    
    if (App::IsKeyPressed(App::KEY_LEFT)) {
        camera.rotationAngle += rotationSpeed * dtSec;
    }
    if (App::IsKeyPressed(App::KEY_RIGHT)) {
        camera.rotationAngle -= rotationSpeed * dtSec;
    }

    // 10. Update map generation system based on player position
    GenerateSystem::MapGenerationSystem(registry, playerCurrentZ, nextSpawnZ);
}

void ControlSystem::Update(EntityManager& registry, float dt, Camera3D& camera, float& nextSpawnZ)
{
    PlayerControl3D(registry, dt, camera, nextSpawnZ);
}
