#include "../System/Component/Component.h"
#include "MovementSystem.h"
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

            // 2) force 是一次性的（冲击只在那一帧），用完必须清零
            rb.force = { 0.0f, 0.0f };
        }

        pos.pos = pos.pos + vel.vel * dtMs;
    }
}
void EnemyMovementSystem(EntityManager& registry,const float deltaTimeMs) {
    Vec2 targetPos = { 0.0f, 0.0f };
    bool playerFound = false;

    // View<PlayerTag, Position>: 只筛选有玩家标签的实体
    View<PlayerTag, Position> playerView(registry);

    for (EntityID id : playerView) {
        auto& pos = playerView.get<Position>(id);
        targetPos = pos.pos; // 记录玩家位置
        playerFound = true;
        break; // 只要找到一个玩家就停止（单人游戏）
    }

    // 如果玩家死了或者还没生成，就不要运行敌人的逻辑，防止敌人乱跑
    if (!playerFound) return;


    // ==================================================
    // 2. 第二步：更新所有敌人 (Process Enemies)
    // ==================================================

    // View<EnemyTag, Position, Velocity>: 只筛选敌人
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
    SpriteMovementSystem(registry, dt);
    EnemyMovementSystem(registry, dt);
}