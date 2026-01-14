#include "../System/Component/Component.h"
#include "MovementSystem.h"
#include <cmath>
#include <vector>
void SpriteMovementSystem(EntityManager& registry, float dtMs) {
    View<Position, Velocity,PlayerTag,Health, RigidBody> view(registry);
    const float dtSec = dtMs / 1000.0f;
    const float damping = 8.0f;

    for (EntityID id : view)
    {
        auto& pos = view.get<Position>(id);
        auto& vel = view.get<Velocity>(id);
        auto& rb = view.get<RigidBody>(id);

        if (rb.force.x != 0.0f || rb.force.y != 0.0f)
        {
            const float invMass = (rb.mass > 0.0001f) ? (1.0f / rb.mass) : 0.0f;
            const Vec2 accel = rb.force * invMass;
            vel.vel = vel.vel + accel * dtMs;

            rb.force = { 0.0f, 0.0f };
        }

        pos.pos = pos.pos + vel.vel * dtMs;
    }
}
void SpriteMovementSystem25D(EntityManager& registry, float dtMs) {
    View<Position3D, Velocity3D, PlayerTag, Health, RigidBody> view(registry);
    const float dtSec = dtMs / 1000.0f;
    const float damping = 8.0f;
    for (EntityID id : view)
    {
        auto& pos = view.get<Position3D>(id);
        auto& vel = view.get<Velocity3D>(id);
        auto& rb = view.get<RigidBody>(id);
        if (rb.force.x != 0.0f || rb.force.y != 0.0f)
        {
            const float invMass = (rb.mass > 0.0001f) ? (1.0f / rb.mass) : 0.0f;
            const Vec2 accel = rb.force * invMass;
            vel.vx = vel.vx + accel.x * dtSec;
            vel.vz = vel.vz + accel.y * dtSec;
            rb.force = { 0.0f, 0.0f };
        }
        pos.x = pos.x + vel.vx * dtSec;
        pos.z = pos.z + vel.vz * dtSec;
    }
}
void EnemyMovementSystem(EntityManager& registry,const float deltaTimeMs) {
    Vec2 targetPos = { 0.0f, 0.0f };
    bool playerFound = false;

    // View<PlayerTag, Position>: ֻɸѡ����ұ�ǩ��ʵ��
    View<PlayerTag, Position> playerView(registry);

    for (EntityID id : playerView) {
        auto& pos = playerView.get<Position>(id);
        targetPos = pos.pos; // 
        playerFound = true;
        break; // 
    }

    // 
    if (!playerFound) return;


    // ==================================================
    // 2.
    // ==================================================

    // View<EnemyTag, Position, Velocity>: ֻɸѡ����
    View<EnemyTag, Position, Velocity> enemyView(registry);
    const float enemySpeed = 130.0f;
    const float dtSec = deltaTimeMs / 1000.0f;
    for (EntityID id : enemyView) {
        auto& enemyPos = enemyView.get<Position>(id);
        auto& enemyVel = enemyView.get<Velocity>(id);

        Vec2 dir = Normalize(targetPos - enemyPos.pos);
        enemyPos.pos = enemyPos.pos + dir * (enemySpeed * dtSec);
    }
}

void EnemyMovementSystem3D(EntityManager& registry, const float deltaTimeMs) {
    // Find player position
    Vec2 targetPos = { 512.0f, 384.0f };
    bool playerFound = false;

    View<PlayerTag, Position3D> playerView(registry);
    for (EntityID id : playerView) {
        auto& pos = playerView.get<Position3D>(id);
        targetPos = { pos.x, pos.z };
        playerFound = true;
        break;
    }

    if (!playerFound) return;

    // Update all enemies with type-specific speed
    View<EnemyTag, Position3D, Velocity3D, EnemyTypeComponent> enemyView(registry);
    const float dtSec = deltaTimeMs / 1000.0f;
    
    for (EntityID id : enemyView) {
        auto& enemyPos = enemyView.get<Position3D>(id);
        auto& enemyVel = enemyView.get<Velocity3D>(id);
        auto& enemyType = enemyView.get<EnemyTypeComponent>(id);

        Vec2 currentPos = { enemyPos.x, enemyPos.z };
        Vec2 dir = Normalize(targetPos - currentPos);
        
        // Use enemy type speed
        enemyPos.x += dir.x * enemyType.speed * dtSec;
        enemyPos.z += dir.y * enemyType.speed * dtSec;
    }
}

void BulletMovementSystem(EntityManager& registry, const float deltaTimeMs) {
    View<BulletTag, Position, Velocity> bulletView(registry);
    const float dtSec = deltaTimeMs / 1000.0f;
    
    for (EntityID id : bulletView) {
        auto& pos = bulletView.get<Position>(id);
        auto& vel = bulletView.get<Velocity>(id);
        
        pos.pos = pos.pos + vel.vel * dtSec;
    }
    
    // Update 3D bullets
    View<BulletTag, Position3D, Velocity> bullet3DView(registry);
    for (EntityID id : bullet3DView) {
        auto& pos = bullet3DView.get<Position3D>(id);
        auto& vel = bullet3DView.get<Velocity>(id);
        
        pos.x += vel.vel.x * dtSec;
        pos.z += vel.vel.y * dtSec;
    }
}

void BulletLifetimeSystem(EntityManager& registry, const float deltaTimeMs) {
    View<BulletTag, BulletComponent> bulletView(registry);
    
    std::vector<EntityID> bulletsToRemove;
    
    for (EntityID id : bulletView) {
        auto& bullet = bulletView.get<BulletComponent>(id);
        
        bullet.lifetime -= deltaTimeMs;
        if (bullet.lifetime <= 0.0f) {
            bulletsToRemove.push_back(id);
        }
    }
    
    // Remove expired bullets
    for (EntityID id : bulletsToRemove) {
        registry.destroyEntity(Entity{ id, registry.getEntityVersion(id) });
    }
}

void MovementSystem::Update(EntityManager& registry, const float dt) {
    SpriteMovementSystem25D(registry, dt);
    EnemyMovementSystem3D(registry, dt);
    BulletMovementSystem(registry, dt);
    BulletLifetimeSystem(registry, dt);
}