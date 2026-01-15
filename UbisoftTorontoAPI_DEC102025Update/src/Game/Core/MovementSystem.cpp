#include "MovementSystem.h"
#include "../../System/Component/Component.h"
#include <cmath>
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
void SpriteMovementSystem3D(EntityManager& registry, float dtMs) {
    View<Position3D, Velocity3D, PlayerTag, Health, RigidBody> view(registry);
    const float dtSec = dtMs / 1000.0f;
    const float damping = 8.0f;
    for (EntityID id : view)
    {
        auto& pos = view.get<Position3D>(id);
        auto& vel = view.get<Velocity3D>(id).vel;
        auto& rb = view.get<RigidBody>(id);
        if (rb.force.x != 0.0f || rb.force.y != 0.0f)
        {
            const float invMass = (rb.mass > 0.0001f) ? (1.0f / rb.mass) : 0.0f;
            const Vec2 accel = rb.force * invMass;
            vel.x = vel.x + accel.x * dtSec;
            vel.z = vel.z + accel.y * dtSec;
            // 2)
            rb.force = { 0.0f, 0.0f };
        }
        pos.x = pos.x + vel.x * dtSec;
        pos.z = pos.z + vel.z * dtSec;
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



    // View<EnemyTag, Position, Velocity>: 
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
void BulletMovementSystem(EntityManager& registry, const float deltaTimeMs) {
    View<Transform3D, Bullet> bulletView(registry);
    const float dtSec = deltaTimeMs / 1000.0f;

    static std::vector<EntityID> bulletsToRemove;
    bulletsToRemove.clear();
	for (EntityID id : bulletView) {
		auto& bulletPos = bulletView.get<Transform3D>(id).pos;
		auto& bullet = bulletView.get<Bullet>(id);
        bullet.lifetime -= deltaTimeMs;

        // Remove bullet if lifetime expired
        if (bullet.lifetime <= 0.0f) {
            bulletsToRemove.push_back(id);
            continue;
        }
		// Update bullet position based on its velocity
		bulletPos += bullet.direction * (bullet.speed * dtSec);
	
	}
    for (EntityID id : bulletsToRemove) {
        registry.destroyEntity(id);
    }
}
void MovementSystem::Update(EntityManager& registry, const float dt) {
    BulletMovementSystem(registry, dt);
    //SpriteMovementSystem3D(registry, dt);
    /*SpriteMovementSystem(registry, dt);
    EnemyMovementSystem(registry, dt);*/
}