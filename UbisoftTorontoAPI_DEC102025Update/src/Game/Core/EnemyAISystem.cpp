#include "EnemyAISystem.h"
#include "../../System/Component/Component.h"
#include "../../System/Math/Vec3.h"
#include <cmath>

// Configuration constants
static const float MAX_BULLET_DISTANCE = 5000.0f;  // Maximum distance bullets can travel from origin

// Helper function to calculate distance between two 3D points
static float Distance3D(const Vec3& a, const Vec3& b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dz = a.z - b.z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

// Helper function to normalize a Vec3
static Vec3 Normalize3D(const Vec3& v) {
    float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (len < 0.0001f) return Vec3(0.0f, 0.0f, 0.0f);
    return Vec3(v.x / len, v.y / len, v.z / len);
}

// Helper function to get player position
static bool GetPlayerPosition(EntityManager& registry, Vec3& outPosition) {
    View<PlayerTag, Transform3D> playerView(registry);
    for (EntityID id : playerView) {
        auto& transform = playerView.get<Transform3D>(id);
        outPosition = transform.pos;
        return true;
    }
    return false;
}

void EnemyAISystem::UpdateEnemyMovement(EntityManager& registry, const float deltaTimeMs) {
    // Find player position
    Vec3 playerPos;
    if (!GetPlayerPosition(registry, playerPos)) return;
    
    // Update all enemies with EnemyMoveToPlayer component
    View<EnemyTag, Transform3D, EnemyMoveToPlayer> enemyView(registry);
    const float dtSec = deltaTimeMs / 1000.0f;
    
    for (EntityID id : enemyView) {
        auto& enemyTransform = enemyView.get<Transform3D>(id);
        auto& moveComponent = enemyView.get<EnemyMoveToPlayer>(id);
        
        // Skip if not active
        if (!moveComponent.isActive) continue;
        
        // Calculate distance to player
        float distance = Distance3D(enemyTransform.pos, playerPos);
        
        // Only move if within detection range
        if (distance <= moveComponent.detectionRange) {
            // Calculate direction to player
            Vec3 direction = Vec3(
                playerPos.x - enemyTransform.pos.x,
                playerPos.y - enemyTransform.pos.y,
                playerPos.z - enemyTransform.pos.z
            );
            direction = Normalize3D(direction);
            
            // Move enemy toward player
            enemyTransform.pos.x += direction.x * moveComponent.speed * dtSec;
            enemyTransform.pos.y += direction.y * moveComponent.speed * dtSec;
            enemyTransform.pos.z += direction.z * moveComponent.speed * dtSec;
        }
    }
}

void EnemyAISystem::UpdateEnemyShooting(EntityManager& registry, const float deltaTimeMs) {
    // Find player position
    Vec3 playerPos;
    if (!GetPlayerPosition(registry, playerPos)) return;
    
    // Update all enemies with EnemyShootAtPlayer component
    View<EnemyTag, Transform3D, EnemyShootAtPlayer> enemyView(registry);
    
    for (EntityID id : enemyView) {
        auto& enemyTransform = enemyView.get<Transform3D>(id);
        auto& shootComponent = enemyView.get<EnemyShootAtPlayer>(id);
        
        // Skip if not active
        if (!shootComponent.isActive) continue;
        
        // Update cooldown timer
        shootComponent.timeSinceLastShot += deltaTimeMs;
        
        // Calculate distance to player
        float distance = Distance3D(enemyTransform.pos, playerPos);
        
        // Only shoot if within detection range and cooldown has passed
        if (distance <= shootComponent.detectionRange && 
            shootComponent.timeSinceLastShot >= shootComponent.shootCooldown) {
            
            // Reset cooldown
            shootComponent.timeSinceLastShot = 0.0f;
            
            // Calculate direction to player
            Vec3 direction = Vec3(
                playerPos.x - enemyTransform.pos.x,
                playerPos.y - enemyTransform.pos.y,
                playerPos.z - enemyTransform.pos.z
            );
            direction = Normalize3D(direction);
            
            // Create bullet entity
            Entity bulletId = registry.createEntity();
            
            // Add Transform3D component (bullet starts at enemy position)
            Transform3D bulletTransform;
            bulletTransform.pos = enemyTransform.pos;
            bulletTransform.width = 5.0f;
            bulletTransform.height = 5.0f;
            bulletTransform.depth = 5.0f;
            bulletTransform.r = 1.0f;
            bulletTransform.g = 0.0f;
            bulletTransform.b = 0.0f; // Red color for enemy bullets
            registry.addComponent(bulletId, bulletTransform);
            
            // Add Bullet component
            Bullet bulletComponent;
            bulletComponent.direction = direction;
            bulletComponent.speed = shootComponent.projectileSpeed;
            bulletComponent.lifetime = 5000.0f; // 5 seconds
            bulletComponent.damage = 10.0f;
            bulletComponent.isPlayerBullet = false;
            registry.addComponent(bulletId, bulletComponent);
        }
    }
}

void EnemyAISystem::UpdateBullets(EntityManager& registry, const float deltaTimeMs) {
    View<Transform3D, Bullet> bulletView(registry);
    const float dtSec = deltaTimeMs / 1000.0f;
    
    // Reuse vector across frames to avoid repeated allocations
    static std::vector<EntityID> bulletsToRemove;
    bulletsToRemove.clear();
    
    for (EntityID id : bulletView) {
        auto& transform = bulletView.get<Transform3D>(id);
        auto& bullet = bulletView.get<Bullet>(id);
        
        // Update lifetime
        bullet.lifetime -= deltaTimeMs;
        
        // Remove bullet if lifetime expired
        if (bullet.lifetime <= 0.0f) {
            bulletsToRemove.push_back(id);
            continue;
        }
        
        // Move bullet in its direction
        transform.pos.x += bullet.direction.x * bullet.speed * dtSec;
        transform.pos.y += bullet.direction.y * bullet.speed * dtSec;
        transform.pos.z += bullet.direction.z * bullet.speed * dtSec;
        
        // Remove bullets that are too far from origin
        float distanceFromOrigin = std::sqrt(
            transform.pos.x * transform.pos.x + 
            transform.pos.y * transform.pos.y + 
            transform.pos.z * transform.pos.z
        );
        if (distanceFromOrigin > MAX_BULLET_DISTANCE) {
            bulletsToRemove.push_back(id);
        }
    }
    
    // Remove expired bullets
    for (EntityID id : bulletsToRemove) {
        registry.destroyEntity(id);
    }
}

void EnemyAISystem::Update(EntityManager& registry, const float deltaTimeMs) {
    UpdateEnemyMovement(registry, deltaTimeMs);
    UpdateEnemyShooting(registry, deltaTimeMs);
    UpdateBullets(registry, deltaTimeMs);
}
