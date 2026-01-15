#include "CollisionSystem.h"
#include "../../System/Component/Component.h"
#include "../../System/Physic/Collision.h"
#include <cmath>

Collision* gCollision;

// 2D collision for player-enemy
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
            registry.destroyEntity(enemyId);
            break;
        }
    }
}

// Simplified 3D collision system - just detect and respond
void CheckPlayer3DCollisions(EntityManager& registry) {
    View<PlayerTag, Transform3D, Velocity3D> playerView(registry);
    
    for (EntityID playerId : playerView) {
        auto& playerTag = playerView.get<PlayerTag>(playerId);
        auto& playerTransform = playerView.get<Transform3D>(playerId);
        auto& vel = playerView.get<Velocity3D>(playerId).vel;
        Vec3& pos = playerTransform.pos;
        
        // Reset ground flag
        playerTag.isOnGround = false;
        
        // Player bounding box
        Vec3 playerMin(pos.x - playerTransform.width / 2, 
                       pos.y - playerTransform.height / 2, 
                       pos.z - playerTransform.depth / 2);
        Vec3 playerMax(pos.x + playerTransform.width / 2, 
                       pos.y + playerTransform.height / 2, 
                       pos.z + playerTransform.depth / 2);
        
        // Check all colliders
        View<Collider3D, Transform3D> colliderView(registry);
        float highestFloor = -1000.0f;
        
        for (EntityID colliderId : colliderView) {
            auto& collider = colliderView.get<Collider3D>(colliderId);
            auto& transform = colliderView.get<Transform3D>(colliderId);
            
            // Collider bounding box
            Vec3 colliderMin(transform.pos.x - transform.width / 2,
                            transform.pos.y - transform.height / 2,
                            transform.pos.z - transform.depth / 2);
            Vec3 colliderMax(transform.pos.x + transform.width / 2,
                            transform.pos.y + transform.height / 2,
                            transform.pos.z + transform.depth / 2);
            
            // Check collision using AABB
            if (gCollision->AABB3D(playerMin, playerMax, colliderMin, colliderMax)) {
                // Calculate penetration on each axis
                float penetrationX = (playerMin.x < colliderMin.x) ? 
                    (colliderMin.x - playerMax.x) : (colliderMax.x - playerMin.x);
                float penetrationY = (playerMin.y < colliderMin.y) ? 
                    (colliderMin.y - playerMax.y) : (colliderMax.y - playerMin.y);
                float penetrationZ = (playerMin.z < colliderMin.z) ? 
                    (colliderMin.z - playerMax.z) : (colliderMax.z - playerMin.z);
                
                float absX = std::abs(penetrationX);
                float absY = std::abs(penetrationY);
                float absZ = std::abs(penetrationZ);
                
                // Resolve on minimum penetration axis
                if (absX < absY && absX < absZ) {
                    // X-axis collision (left/right wall)
                    pos.x += penetrationX;
                    vel.x = 0.0f;
                } else if (absZ < absY) {
                    // Z-axis collision (front/back wall)
                    pos.z += penetrationZ;
                    vel.z = 0.0f;
                } else {
                    // Y-axis collision (floor/ceiling)
                    if (penetrationY > 0) {
                        // Hit from below (floor)
                        if (collider.isFloor) {
                            float floorTop = colliderMax.y;
                            if (floorTop > highestFloor) {
                                highestFloor = floorTop;
                            }
                        }
                    } else {
                        // Hit from above (ceiling)
                        pos.y += penetrationY;
                        vel.y = 0.0f;
                    }
                }
                
                // Update bounding box after position change
                playerMin = Vec3(pos.x - playerTransform.width / 2, 
                               pos.y - playerTransform.height / 2, 
                               pos.z - playerTransform.depth / 2);
                playerMax = Vec3(pos.x + playerTransform.width / 2, 
                               pos.y + playerTransform.height / 2, 
                               pos.z + playerTransform.depth / 2);
            }
        }
        
        // Apply floor collision if detected
        if (highestFloor > -999.0f) {
            float playerBottom = pos.y - playerTransform.height / 2;
            if (playerBottom <= highestFloor + 0.5f) {
                pos.y = highestFloor + playerTransform.height / 2;
                vel.y = 0.0f;
                playerTag.isOnGround = true;
            }
        }
    }
}

void CheckPlayerGetPoints(EntityManager& registry) {
    View<PlayerTag, Transform3D> playerView(registry);
    EntityID playerId{};
    Vec3 playerPos;
    Transform3D playerTransform;
    float playerRadius = 0.0f;
	for (EntityID id : playerView) {
        playerId = id;
		playerTransform = playerView.get<Transform3D>(id);
        playerPos = playerView.get<Transform3D>(id).pos;
        playerRadius = playerTransform.width / 2; // Assume width as diameter
        break;
    }
    Vec3 playerMin(playerPos.x - playerTransform.width / 2,
        playerPos.y - playerTransform.height / 2,
        playerPos.z - playerTransform.depth / 2);
    Vec3 playerMax(playerPos.x + playerTransform.width / 2,
        playerPos.y + playerTransform.height / 2,
        playerPos.z + playerTransform.depth / 2);
    View<ScorePointTag, Transform3D> scoreView(registry);
    for (EntityID scoreId : scoreView) {
        auto& scoreTransform = scoreView.get<Transform3D>(scoreId);
        float scoreRadius = scoreTransform.width / 2; // Assume width as diameter
        Vec3 colliderMin(scoreTransform.pos.x - scoreTransform.width / 2,
            scoreTransform.pos.y - scoreTransform.height / 2,
            scoreTransform.pos.z - scoreTransform.depth / 2);
        Vec3 colliderMax(scoreTransform.pos.x + scoreTransform.width / 2,
            scoreTransform.pos.y + scoreTransform.height / 2,
            scoreTransform.pos.z + scoreTransform.depth / 2);
        if (gCollision->AABB3D(playerMin, playerMax, colliderMin, colliderMax)) {
            auto& scoreTag = scoreView.get<ScorePointTag>(scoreId);
            scoreTag.collected = true;
			auto& playerTag = playerView.get<PlayerTag>(playerId);
			playerTag.score += scoreTag.points;
        }
	}
}
void CheckBulletDamage(EntityManager& registry) {
    static std::vector<EntityID> blockToRemove;
    blockToRemove.clear();
    static std::vector<EntityID> bulletToRemove;
    bulletToRemove.clear();
    View<Bullet, Transform3D> bulletView(registry);
    View<Health, Transform3D> enemyView(registry);
    for (EntityID bulletId : bulletView) {
		float bulletDamage = bulletView.get<Bullet>(bulletId).damage;
        auto& bulletTransform = bulletView.get<Transform3D>(bulletId);
        Vec3 bulletPos = bulletTransform.pos;
        Vec3 bulletMin(bulletPos.x - bulletTransform.width / 2,
            bulletPos.y - bulletTransform.height / 2,
            bulletPos.z - bulletTransform.depth / 2);
        Vec3 bulletMax(bulletPos.x + bulletTransform.width / 2,
            bulletPos.y + bulletTransform.height / 2,
            bulletPos.z + bulletTransform.depth / 2);
        for (EntityID enemyId : enemyView) {
            auto& enemyHealth = enemyView.get<Health>(enemyId);
			auto& enemyTransform = enemyView.get<Transform3D>(enemyId);
            Vec3 colliderMin(enemyTransform.pos.x - enemyTransform.width / 2,
                enemyTransform.pos.y - enemyTransform.height / 2,
                enemyTransform.pos.z - enemyTransform.depth / 2);
            Vec3 colliderMax(enemyTransform.pos.x + enemyTransform.width / 2,
                enemyTransform.pos.y + enemyTransform.height / 2,
                enemyTransform.pos.z + enemyTransform.depth / 2);
            if (gCollision->AABB3D(bulletMin, bulletMax, colliderMin, colliderMax)) {
				bulletToRemove.push_back(bulletId);
                enemyHealth.currentHealth -= bulletDamage;
                if (enemyHealth.currentHealth <= 0) {
                    blockToRemove.push_back(enemyId);
                }
            }
        }
    }
    for (EntityID id : bulletToRemove) {
        registry.destroyEntity(id);
	}
    for (EntityID id : blockToRemove) {
        registry.destroyEntity(id);
	}
}

void CollisionSystem::Update(EntityManager& registry) {
    CheckPlayer3DCollisions(registry);
    CheckPlayerGetPoints(registry);
    CheckBulletDamage(registry);
}
