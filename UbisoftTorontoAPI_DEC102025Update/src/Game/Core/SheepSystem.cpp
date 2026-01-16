#include "SheepSystem.h"
#include "../../System/Component/Component.h"
#include "../../System/Math/Math.h" 
#include <cmath>
#include <vector>
#include <unordered_map>

// --- 简易空间网格优化 (Spatial Grid) ---
// 用于快速查找附近的羊，避免 O(N^2) 复杂度
struct SpatialGrid {
    float cellSize;
    std::unordered_map<int, std::vector<EntityID>> grid;

    SpatialGrid(float size) : cellSize(size) {}

    // 将坐标转换为唯一的 Hash Key
    int GetKey(float x, float z) {
        // 防止负数坐标除法问题，加一个大偏移
        int cx = static_cast<int>(std::floor((x + 100000.0f) / cellSize));
        int cz = static_cast<int>(std::floor((z + 100000.0f) / cellSize));
        return cx * 73856093 ^ cz * 19349663; // 简单的空间哈希
    }

    void Clear() { grid.clear(); }

    void Insert(EntityID id, float x, float z) {
        grid[GetKey(x, z)].push_back(id);
    }

    // 获取 3x3 范围内的邻居
    void Query(float x, float z, std::vector<EntityID>& results) {
        int centerCx = static_cast<int>(std::floor((x + 100000.0f) / cellSize));
        int centerCz = static_cast<int>(std::floor((z + 100000.0f) / cellSize));

        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                int key = (centerCx + i) * 73856093 ^ (centerCz + j) * 19349663;
                auto it = grid.find(key);
                if (it != grid.end()) {
                    results.insert(results.end(), it->second.begin(), it->second.end());
                }
            }
        }
    }
};

// --- 辅助数学函数 ---
inline float Mag(const Vec3& v) { return std::sqrt(v.x * v.x + v.z * v.z); } // 只计算 XZ 平面模长
inline Vec3 Norm(const Vec3& v) {
    float m = Mag(v);
    if (m > 0.0001f) return { v.x / m, 0, v.z / m };
    return { 0,0,0 };
}
inline void Limit(Vec3& v, float max) {
    float mSq = v.x * v.x + v.z * v.z;
    if (mSq > max * max && mSq > 0) {
        float m = std::sqrt(mSq);
        v.x = (v.x / m) * max;
        v.z = (v.z / m) * max;
    }
}

namespace SheepSystem {

    void InitSheep(EntityManager& registry, float startX, float startZ, int count) {
        for (int i = 0; i < count; i++) {
            Entity sheep = registry.createEntity();

            // 随机分布在起始点周围
            float offsetX = (rand() % 200 - 100.0f);
            float offsetZ = (rand() % 200 - 100.0f);

            registry.addComponent(sheep, Transform3D{
                Vec3{startX + offsetX, 20.0f, startZ + offsetZ},
                15.0f, 15.0f, 15.0f, // 尺寸
                0.9f, 0.9f, 0.9f     // 颜色：白羊
                });
            registry.addComponent(sheep, Velocity3D{ Vec3{0,0,0} });
            registry.addComponent(sheep, SheepTag{});
            registry.addComponent(sheep, SheepComponent{}); // 使用默认 Boids 参数

            // 重要：添加 Collider3D 和 RigidBody (如果系统需要) 以便与地面碰撞
            // 注意：你现有的 PhysicsSystem 似乎只处理 PlayerTag，
            // 你可能需要修改 PhysicsSystem 让他也处理 SheepTag，或者在这里简单模拟
            registry.addComponent(sheep, PhysicsTag{ true}); //
            // 更好的做法是在 PhysicsSystem 里把 View<..., PlayerTag> 改成 View<..., Velocity3D> 并过滤掉静态物体
        }
    }

    void Update(EntityManager& registry, float dtMs) {
        float dt = dtMs / 10.0f;
        if (dt <= 0) return;

        // 1. 获取玩家位置 (作为领头羊/目标)
        Vec3 targetPos;
        bool hasPlayer = false;
        View<PlayerTag, Transform3D> playerView(registry);
        for (auto id : playerView) {
            targetPos = playerView.get<Transform3D>(id).pos;
            hasPlayer = true;
            break; // 只跟第一个玩家
        }

        // 2. 收集所有敌人位置 (用于恐惧逻辑)
        std::vector<Vec3> enemies;
        View<EnemyTag, Transform3D> enemyView(registry);
        for (auto id : enemyView) {
            enemies.push_back(enemyView.get<Transform3D>(id).pos);
        }

        // 3. 构建空间网格
        static SpatialGrid grid(80.0f); // 格子大小略大于视野半径
        grid.Clear();
        View<SheepTag, Transform3D, Velocity3D, SheepComponent> sheepView(registry);

        for (auto id : sheepView) {
            auto& pos = sheepView.get<Transform3D>(id).pos;
            grid.Insert(id, pos.x, pos.z);
        }

        // 4. Boids 循环
        std::vector<EntityID> neighbors;
        neighbors.reserve(50);

        for (auto id : sheepView) {
            auto& t = sheepView.get<Transform3D>(id);
            auto& v = sheepView.get<Velocity3D>(id);
            auto& params = sheepView.get<SheepComponent>(id);

            Vec3 pos = t.pos;
            Vec3 vel = v.vel;

            Vec3 forceSep = { 0,0,0 };
            Vec3 forceAli = { 0,0,0 };
            Vec3 forceCoh = { 0,0,0 };
            Vec3 forceTar = { 0,0,0 };
            Vec3 forceFear = { 0,0,0 };

            int neighborCount = 0;

            // --- 查找邻居 ---
            neighbors.clear();
            grid.Query(pos.x, pos.z, neighbors);

            for (EntityID otherID : neighbors) {
                if (id == otherID) continue;

                auto& ot = sheepView.get<Transform3D>(otherID);
                auto& ov = sheepView.get<Velocity3D>(otherID);

                float dx = pos.x - ot.pos.x;
                float dz = pos.z - ot.pos.z;
                float distSq = dx * dx + dz * dz;

                if (distSq < params.viewRadius * params.viewRadius && distSq > 0.0001f) {
                    float dist = std::sqrt(distSq);

                    // 1. Separation: 远离邻居
                    // 权重随距离衰减 (越近斥力越大)
                    forceSep.x += (dx / dist) / dist;
                    forceSep.z += (dz / dist) / dist;

                    // 2. Alignment: 模仿邻居速度
                    forceAli.x += ov.vel.x;
                    forceAli.z += ov.vel.z;

                    // 3. Cohesion: 记录邻居位置
                    forceCoh.x += ot.pos.x;
                    forceCoh.z += ot.pos.z;

                    neighborCount++;
                }
            }

            if (neighborCount > 0) {
                // Alignment 平均化
                forceAli.x /= neighborCount;
                forceAli.z /= neighborCount;
                forceAli = Norm(forceAli);

                // Cohesion 平均化并求方向
                forceCoh.x /= neighborCount;
                forceCoh.z /= neighborCount;
                // Cohesion 是由于“想去中心点”产生的力 -> (Center - CurrentPos)
                forceCoh.x -= pos.x;
                forceCoh.z -= pos.z;
                forceCoh = Norm(forceCoh);

                // Separation 已经是累加的归一化向量，无需除以 count
            }

            // 4. Target: 跟随玩家
            if (hasPlayer) {
                float dx = targetPos.x - pos.x;
                float dz = targetPos.z - pos.z;
                float distToTarget = std::sqrt(dx * dx + dz * dz);

                // 如果距离过远，产生吸引力
                if (distToTarget > 60.0f) { // 保持一定距离，别贴在玩家身上
                    forceTar = { dx, 0, dz };
                    forceTar = Norm(forceTar);
                }
                else if (distToTarget < 30.0f) {
                    // 离太近了稍微让开一点
                    forceTar = { -dx, 0, -dz };
                    forceTar = Norm(forceTar);
                }
            }

            // 5. Fear: 躲避敌人
            for (const auto& enemyPos : enemies) {
                float dx = pos.x - enemyPos.x;
                float dz = pos.z - enemyPos.z;
                float distSq = dx * dx + dz * dz;

                if (distSq < params.enemyDetectRange * params.enemyDetectRange) {
                    // 产生巨大的反向斥力
                    Vec3 fleeDir = { dx, 0, dz };
                    fleeDir = Norm(fleeDir);
                    forceFear.x += fleeDir.x;
                    forceFear.z += fleeDir.z;
                }
            }

            // --- 力的合成 ---
            Vec3 totalForce = { 0,0,0 };
            totalForce.x = forceSep.x * params.separationWeight +
                forceAli.x * params.alignmentWeight +
                forceCoh.x * params.cohesionWeight +
                forceTar.x * params.targetWeight +
                forceFear.x * params.fearWeight;

            totalForce.z = forceSep.z * params.separationWeight +
                forceAli.z * params.alignmentWeight +
                forceCoh.z * params.cohesionWeight +
                forceTar.z * params.targetWeight +
                forceFear.z * params.fearWeight;

            // 限制转向力 (Steering Force)
            Limit(totalForce, params.maxForce);

            // --- 应用物理 ---
            // Vel += Force * dt
            vel.x += totalForce.x * dt;
            vel.z += totalForce.z * dt;

            // 限制最大速度
            Limit(vel, params.maxSpeed);

            // 简单的阻力 (防止永远滑行)
            vel.x *= 0.98f;
            vel.z *= 0.98f;

            // --- 写回组件 ---
            // 注意：我们只修改 X 和 Z，Y 轴由 PhysicsSystem 处理重力
            v.vel.x = vel.x;
            v.vel.z = vel.z;

            // 简单的朝向更新 (让模型面向移动方向)
            if (Mag(vel) > 1.0f) {
                // 如果你有 Rotation 组件可以在这里更新 Yaw
            }
        }
    }
}