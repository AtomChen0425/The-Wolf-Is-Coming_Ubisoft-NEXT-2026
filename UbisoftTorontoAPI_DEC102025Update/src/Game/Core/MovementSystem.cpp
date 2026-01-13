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

            // 2) force ��һ���Եģ����ֻ����һ֡���������������
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
            // 2) force ��һ���Եģ����ֻ����һ֡���������������
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
        targetPos = pos.pos; // ��¼���λ��
        playerFound = true;
        break; // ֻҪ�ҵ�һ����Ҿ�ֹͣ��������Ϸ��
    }

    // ���������˻��߻�û���ɣ��Ͳ�Ҫ���е��˵��߼�����ֹ��������
    if (!playerFound) return;


    // ==================================================
    // 2. �ڶ������������е��� (Process Enemies)
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
void MovementSystem::Update(EntityManager& registry, const float dt) {
    SpriteMovementSystem25D(registry, dt);
    /*SpriteMovementSystem(registry, dt);
    EnemyMovementSystem(registry, dt);*/
}