#include "WeaponSystem.h"
#include "GenerateSystem.h"
#include "../System/Component/Component.h"
#include <cmath>

// Helper function to find nearest enemy
static bool FindNearestEnemy(EntityManager& registry, Vec2 playerPos, Vec2& nearestEnemyPos, float maxRange) {
    View<EnemyTag, Position3D> enemyView(registry);
    
    float nearestDistSq = maxRange * maxRange;
    bool found = false;
    
    for (EntityID id : enemyView) {
        auto& enemyPos3d = enemyView.get<Position3D>(id);
        Vec2 enemyPos = { enemyPos3d.x, enemyPos3d.z };
        
        float distSq = LenSq(enemyPos - playerPos);
        if (distSq < nearestDistSq) {
            nearestDistSq = distSq;
            nearestEnemyPos = enemyPos;
            found = true;
        }
    }
    
    return found;
}

void WeaponSystem::Update(EntityManager& registry, const float dt) {
    View<PlayerTag, Position3D, AutoWeapon> playerView(registry);
    
    for (EntityID id : playerView) {
        auto& pos3d = playerView.get<Position3D>(id);
        auto& weapon = playerView.get<AutoWeapon>(id);
        
        Vec2 playerPos = { pos3d.x, pos3d.z };
        
        // Update cooldown
        weapon.currentCooldownMs -= dt;
        
        // Try to shoot if cooldown is ready
        if (weapon.currentCooldownMs <= 0.0f) {
            Vec2 targetPos;
            if (FindNearestEnemy(registry, playerPos, targetPos, weapon.range)) {
                // Fire at nearest enemy
                Vec2 direction = targetPos - playerPos;
                
                GenerateSystem::CreateBullet(registry, playerPos, direction, weapon.bulletSpeed, weapon.damage);
                
                // Reset cooldown
                weapon.currentCooldownMs = weapon.cooldownMs;
            }
        }
    }
}
