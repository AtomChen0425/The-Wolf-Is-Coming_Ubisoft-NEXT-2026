#include "PhysicsSystem.h"
#include "../System/Component/Component.h"
void PhysicsSystem25D(EntityManager& registry, float dtMs) {
    float dtSec = dtMs / 1000.0f;
    float gravity = -9.8f * 100.0f; // 重力系数

    View<Position3D, Velocity3D> view(registry);
    for (EntityID id : view) {
        auto& pos = view.get<Position3D>(id);
        auto& vel = view.get<Velocity3D>(id);

        // 1. 应用重力 (只影响高度 Y)
        vel.vy += gravity * dtSec;

        // 2. 更新位置
        pos.x += vel.vx * dtSec;
        pos.z += vel.vz * dtSec; // 地板上的前后移动
        pos.y += vel.vy * dtSec; // 空中的上下跳跃

        // 3. 地面碰撞检测 (假设地面高度是 0)
        if (pos.y < 0.0f) {
            pos.y = 0.0f;
            vel.vy = 0.0f;
            // 这里可以重置 "CanJump" 标记
        }
    }
}
void PhysicsSystem::Update(EntityManager& registry, const float dtMs) {
    PhysicsSystem25D(registry, dtMs);
}