#include "CollisionSystem.h"
#include "../../System/Component/Component.h"
#include "../../System/Physic/Collision.h"
#include <cmath>
Collision* gCollision;
void CheckPlayerEnemyCollision(EntityManager& registry) {
    View<PlayerTag, Position, Velocity, RigidBody, Health> playerView(registry);
    View<EnemyTag, Position> enemyView(registry);
    EntityID playerId{};
    Vec2 playerPos;
    float playerRadius = 0.0f;
    for (EntityID id : playerView) {
        playerId = id;
        playerPos = playerView.get<Position>(id).pos;
        playerRadius = playerView.get<RigidBody>(id).radius;
        break;
    }

    for (EntityID enemyId : enemyView) {
        auto& enemyPos = enemyView.get<Position>(enemyId);
        float enemyRadius = enemyView.get<RigidBody>(enemyId).radius;
        if (gCollision->Circle(playerPos, playerRadius, enemyPos.pos, enemyRadius)) {
            auto& playerHealth = playerView.get<Health>(playerId);
            playerHealth.currentHealth -= 10;

            auto& playerRigidBody = playerView.get<RigidBody>(playerId);

            Vec2 dir = Normalize(playerPos - enemyPos.pos);
            const float knockbackImpulse = 10.0f;

            playerRigidBody.force = dir * knockbackImpulse;
            registry.destroyEntity(Entity{ enemyId, registry.getEntityVersion(enemyId) });
            break;
        }
    }
}
// Helper function to check and resolve 3D collisions for the player
void CheckPlayer3DCollisions(EntityManager& registry) {
    // Process each player entity
    View<PlayerTag, Transform3D, Velocity3D> playerView(registry);
    for (EntityID playerId : playerView) {
        auto& playerTag = playerView.get<PlayerTag>(playerId);
        auto& playerTransform = playerView.get<Transform3D>(playerId);
        auto& vel = playerView.get<Velocity3D>(playerId).vel;
        
        Vec3& pos = playerTransform.pos;
        
        // Reset collision flag
        playerTag.isOnGround = false;
        
        // Calculate player bounding box
        Vec3 playerMin(pos.x - playerTransform.width / 2, 
                       pos.y - playerTransform.height / 2, 
                       pos.z - playerTransform.depth / 2);
        Vec3 playerMax(pos.x + playerTransform.width / 2, 
                       pos.y + playerTransform.height / 2, 
                       pos.z + playerTransform.depth / 2);
        
        // 1. Check ground detection for jump (isOnGround) based on CURRENT position
        View<Collider3D, Transform3D> floorCheckView(registry);
        for (EntityID floorId : floorCheckView) {
            auto& floorCollider = floorCheckView.get<Collider3D>(floorId);
            if (!floorCollider.isFloor) continue;
            
            auto& floorTransform = floorCheckView.get<Transform3D>(floorId);
            Vec3 floorPos = floorTransform.pos;
            Vec3 floorMin(floorPos.x - floorTransform.width / 2, 
                         floorPos.y - floorTransform.height / 2, 
                         floorPos.z - floorTransform.depth / 2);
            Vec3 floorMax(floorPos.x + floorTransform.width / 2, 
                         floorPos.y + floorTransform.height / 2, 
                         floorPos.z + floorTransform.depth / 2);
            
            // Check if player is standing on this floor
            if (playerMin.x < floorMax.x && playerMax.x > floorMin.x &&
                playerMin.z < floorMax.z && playerMax.z > floorMin.z &&
                playerMin.y <= floorMax.y + 0.1f && playerMin.y >= floorMax.y - 5.0f) {
                playerTag.isOnGround = true;
                break;
            }
        }
        
        // 2. Check wall and tall block collisions (treat tall blocks as solid obstacles from sides)
        View<Collider3D, Transform3D> wallView(registry);
        for (EntityID wallId : wallView) {
            auto& wallCollider = wallView.get<Collider3D>(wallId);
            // Skip if it's not a wall and not a floor (tall blocks have isFloor=true but should also block sides)
            if (!wallCollider.isWall && !wallCollider.isFloor) continue;
            
            auto& wallTransform = wallView.get<Transform3D>(wallId);
            Vec3 wallPos = wallTransform.pos;
            Vec3 wallMin(wallPos.x - wallTransform.width / 2, 
                        wallPos.y - wallTransform.height / 2, 
                        wallPos.z - wallTransform.depth / 2);
            Vec3 wallMax(wallPos.x + wallTransform.width / 2, 
                        wallPos.y + wallTransform.height / 2, 
                        wallPos.z + wallTransform.depth / 2);
            
            // Check if player is colliding with this obstacle
            if (gCollision->AABB3D(playerMin, playerMax, wallMin, wallMax)) {
                // For tall blocks, only treat as wall if player is at the side (not landing on top)
                // Check if this is a vertical landing: player's bottom was above the block's top
                bool isLandingOnTop = playerMin.y > wallMax.y - 1.0f && vel.y <= 0.0f && wallCollider.isFloor;
                
                // If landing on top, let the ground collision system handle it
                // Otherwise, treat as a wall and push the player out
                if (!isLandingOnTop) {
                    // Calculate penetration depth in each axis
                    float overlapX = 0.0f;
                    float overlapZ = 0.0f;
                    float overlapY = 0.0f;
                    
                    // Calculate how much we're penetrating from each side
                    float penetrationLeft = playerMax.x - wallMin.x;
                    float penetrationRight = wallMax.x - playerMin.x;
                    float penetrationFront = playerMax.z - wallMin.z;
                    float penetrationBack = wallMax.z - playerMin.z;
                    float penetrationTop = wallMax.y - playerMin.y;
                    float penetrationBottom = playerMax.y - wallMin.y;
                    
                    // Find minimum penetration for each axis
                    if (penetrationLeft < penetrationRight) {
                        overlapX = -penetrationLeft;
                    } else {
                        overlapX = penetrationRight;
                    }
                    
                    if (penetrationFront < penetrationBack) {
                        overlapZ = -penetrationFront;
                    } else {
                        overlapZ = penetrationBack;
                    }
                    
                    if (penetrationTop < penetrationBottom) {
                        overlapY = -penetrationTop;
                    } else {
                        overlapY = penetrationBottom;
                    }
                    
                    // Resolve collision by pushing out on the axis with minimum penetration
                    float absX = std::abs(overlapX);
                    float absZ = std::abs(overlapZ);
                    float absY = std::abs(overlapY);
                    
                    // For floor blocks, avoid Y-axis resolution when player is falling onto them
                    // This prevents jittering between wall and ground collision systems
                    bool avoidYResolution = wallCollider.isFloor && vel.y <= 0.0f && playerMin.y <= wallMax.y + 5.0f;
                    
                    if (absX < absZ && absX < absY) {
                        // Push out in X direction
                        pos.x += overlapX;
                        vel.x = 0.0f;
                    } else if (absZ < absY) {
                        // Push out in Z direction
                        pos.z += overlapZ;
                        vel.z = 0.0f;
                    } else if (!avoidYResolution) {
                        // Push out in Y direction only if not falling onto a floor block
                        pos.y += overlapY;
                        if (overlapY < 0) {
                            // Hit ceiling from below - reverse velocity to fall back down
                            vel.y = -std::abs(vel.y);
                        } else {
                            // Hit floor from above (shouldn't happen as ground system handles this)
                            vel.y = 0.0f;
                        }
                    }
                    
                    // Recalculate bounding box after correction
                    playerMin = Vec3(pos.x - playerTransform.width / 2, 
                                   pos.y - playerTransform.height / 2, 
                                   pos.z - playerTransform.depth / 2);
                    playerMax = Vec3(pos.x + playerTransform.width / 2, 
                                   pos.y + playerTransform.height / 2, 
                                   pos.z + playerTransform.depth / 2);
                }
            }
        }
        
        // 3. Find the highest floor the player is on and apply ground collision
        const float FLOOR_PROXIMITY_THRESHOLD = 2.0f;  // Reduced threshold for stricter landing detection
        float groundY = -1000.0f;  // Default very low ground
        View<Collider3D, Transform3D> groundCheckView(registry);
        for (EntityID floorId : groundCheckView) {
            auto& floorCollider = groundCheckView.get<Collider3D>(floorId);
            if (!floorCollider.isFloor) continue;
            
            auto& floorTransform = groundCheckView.get<Transform3D>(floorId);
            Vec3 floorPos = floorTransform.pos;
            Vec3 floorMin(floorPos.x - floorTransform.width / 2, 
                         floorPos.y - floorTransform.height / 2, 
                         floorPos.z - floorTransform.depth / 2);
            Vec3 floorMax(floorPos.x + floorTransform.width / 2, 
                         floorPos.y + floorTransform.height / 2, 
                         floorPos.z + floorTransform.depth / 2);
            
            // Check if player is horizontally aligned with this floor
            if (playerMin.x < floorMax.x && playerMax.x > floorMin.x &&
                playerMin.z < floorMax.z && playerMax.z > floorMin.z) {
                float floorTop = floorMax.y;
                
                // Only consider this floor if player is coming from above
                // This prevents auto-bouncing when hitting tall blocks from the side
                float playerBottom = playerMin.y;
                
                // More strict check: player must be falling AND their previous position was above the floor
                // Only set as ground if:
                // 1. Player is falling (vel.y <= 0)
                // 2. Player's bottom is very close to OR just below the floor top
                // 3. Player's center is above the floor center (prevents side collision from triggering)
                bool isFalling = vel.y <= 0.0f;
                bool isNearFloorTop = playerBottom <= floorTop + FLOOR_PROXIMITY_THRESHOLD && 
                                     playerBottom >= floorTop - FLOOR_PROXIMITY_THRESHOLD;
                bool isAboveFloorCenter = pos.y > floorPos.y;  // Player center must be above floor center
                
                if (isFalling && isNearFloorTop && isAboveFloorCenter) {
                    if (floorTop > groundY) {
                        groundY = floorTop;
                    }
                }
            }
        }
        
        // Apply ground collision response
        // If player's bottom is at or below the ground level, place them on top
        if (pos.y - playerTransform.height / 2 <= groundY) {
            pos.y = groundY + playerTransform.height / 2;
            if (vel.y < 0.0f) {  // Only reset velocity if falling
                vel.y = 0.0f;
            }
        }
    }
}

void CollisionSystem::Update(EntityManager& registry) {
    CheckPlayer3DCollisions(registry);
}
