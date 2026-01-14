#include "CollisionSystem.h"
#include "../../System/Component/Component.h"
#include "../../System/Physic/Collision.h"
#include <cmath>

// Helper function to check and resolve 3D collisions for the player
void CheckPlayer3DCollisions(EntityManager& registry) {
    Collision collision;
    
    // Process each player entity
    View<PlayerTag, Transform3D, Velocity3D> playerView(registry);
    for (EntityID playerId : playerView) {
        auto& playerTag = playerView.get<PlayerTag>(playerId);
        auto& playerTransform = playerView.get<Transform3D>(playerId);
        auto& vel = playerView.get<Velocity3D>(playerId);
        
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
        
        // 2. Check wall collisions and correct position if needed
        View<Collider3D, Transform3D> wallView(registry);
        for (EntityID wallId : wallView) {
            auto& wallCollider = wallView.get<Collider3D>(wallId);
            if (!wallCollider.isWall) continue;
            
            auto& wallTransform = wallView.get<Transform3D>(wallId);
            Vec3 wallPos = wallTransform.pos;
            Vec3 wallMin(wallPos.x - wallTransform.width / 2, 
                        wallPos.y - wallTransform.height / 2, 
                        wallPos.z - wallTransform.depth / 2);
            Vec3 wallMax(wallPos.x + wallTransform.width / 2, 
                        wallPos.y + wallTransform.height / 2, 
                        wallPos.z + wallTransform.depth / 2);
            
            // Check if player is colliding with wall
            if (collision.AABB3D(playerMin, playerMax, wallMin, wallMax)) {
                // Calculate penetration depth in each axis
                float overlapX = 0.0f;
                float overlapZ = 0.0f;
                
                // Calculate how much we're penetrating from each side
                float penetrationLeft = playerMax.x - wallMin.x;
                float penetrationRight = wallMax.x - playerMin.x;
                float penetrationFront = playerMax.z - wallMin.z;
                float penetrationBack = wallMax.z - playerMin.z;
                
                // Find minimum penetration for X
                if (penetrationLeft < penetrationRight) {
                    overlapX = -penetrationLeft;
                } else {
                    overlapX = penetrationRight;
                }
                
                // Find minimum penetration for Z
                if (penetrationFront < penetrationBack) {
                    overlapZ = -penetrationFront;
                } else {
                    overlapZ = penetrationBack;
                }
                
                // Resolve collision by pushing out on the axis with minimum penetration
                if (std::abs(overlapX) < std::abs(overlapZ)) {
                    pos.x += overlapX;
                    vel.vx = 0.0f;
                } else {
                    pos.z += overlapZ;
                    vel.vz = 0.0f;
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
        
        // 3. Find the highest floor the player is on and apply ground collision
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
                if (floorTop > groundY) {
                    groundY = floorTop;
                }
            }
        }
        
        // Apply ground collision response
        if (pos.y - playerTransform.height / 2 < groundY) {
            pos.y = groundY + playerTransform.height / 2;
            vel.vy = 0.0f;
        }
    }
}

void CollisionSystem::Update(EntityManager& registry) {
    CheckPlayer3DCollisions(registry);
}
