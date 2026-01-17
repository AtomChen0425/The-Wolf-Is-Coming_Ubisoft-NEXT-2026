#include "SheepSystem.h"
#include "../../System/Component/Component.h"
#include "../../System/Math/Math.h" 
#include <cmath>
#include <vector>
#include <unordered_map>

// --- ���׿ռ������Ż� (Spatial Grid) ---
// ���ڿ��ٲ��Ҹ������򣬱��� O(N^2) ���Ӷ�
struct SpatialGrid {
    float cellSize;
    std::unordered_map<int, std::vector<EntityID>> grid;

    SpatialGrid(float size) : cellSize(size) {}

    // ������ת��ΪΨһ�� Hash Key
    int GetKey(float x, float z) {
        // ��ֹ��������������⣬��һ����ƫ��
        int cx = static_cast<int>(std::floor((x + 100000.0f) / cellSize));
        int cz = static_cast<int>(std::floor((z + 100000.0f) / cellSize));
        return cx * 73856093 ^ cz * 19349663; // �򵥵Ŀռ��ϣ
    }

    void Clear() { grid.clear(); }

    void Insert(EntityID id, float x, float z) {
        grid[GetKey(x, z)].push_back(id);
    }

    // ��ȡ 3x3 ��Χ�ڵ��ھ�
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

// --- ������ѧ���� ---
inline float Mag(const Vec3& v) { return std::sqrt(v.x * v.x + v.z * v.z); } // ֻ���� XZ ƽ��ģ��
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
    void SheepShoot(EntityManager& registry, float dt) {
        // �����ӵ�ʵ��
        std::vector<Vec3> targetPositions;
        View<EnemyTag, Transform3D> enemyView(registry);
        for (auto id : enemyView) {
            targetPositions.push_back(enemyView.get<Transform3D>(id).pos);
        }
        if (targetPositions.empty()) {
            return;
        }
        View<SheepTag, WeaponInventory, Transform3D> sheepView(registry);
        for (auto id : sheepView) {
            auto& inventory = sheepView.get<WeaponInventory>(id);
            auto& sheepPos = sheepView.get<Transform3D>(id).pos;

            float nearestDistSq = std::numeric_limits<float>::max();
            Vec3 nearestTarget;
            bool foundTarget = false;

            for (size_t i = 0; i < targetPositions.size(); i++) {
                float dx = targetPositions[i].x - sheepPos.x;
                float dz = targetPositions[i].z - sheepPos.z;
                float distSq = dx * dx + dz * dz;

                if (distSq < nearestDistSq) {
                    nearestDistSq = distSq;
                    nearestTarget = targetPositions[i];
                    foundTarget = true;
                }
            }
            float dx = nearestTarget.x - sheepPos.x;
            float dz = nearestTarget.z - sheepPos.z;
            for (Weapon& weapon : inventory.weapons) {
                weapon.currentCooldown -= dt;

				if (weapon.currentCooldown > 0.0f) continue;
                // Fire a bullet
                Vec3 bulletDirection = Normalize3D(Vec3{ dx, 0, dz });
                Vec3 bulletPosition = sheepPos + bulletDirection * 2.0f; // Spawn bullet slightly in front of player
                // Create bullet entity
                Entity bullet = registry.createEntity();
                registry.addComponent(bullet, Bullet{ bulletDirection ,weapon.projectileSpeed, weapon.projectileLife,weapon.damage,true,weapon.explosionRadius,weapon.projectileSize,weapon.knockback });
                registry.addComponent(bullet, Transform3D{ bulletPosition, weapon.projectileSize, weapon.projectileSize, weapon.projectileSize, weapon.r, weapon.g, weapon.b });
                registry.addComponent(bullet, Velocity3D{ bulletDirection * weapon.projectileSpeed });
                weapon.currentCooldown = weapon.fireRate;
            }
        }
    }
    void InitSheep(EntityManager& registry, float startX, float startZ, int count) {
        for (int i = 0; i < count; i++) {
            Entity sheep = registry.createEntity();

            // ����ֲ�����ʼ����Χ
            float offsetX = (rand() % 200 - 100.0f);
            float offsetZ = (rand() % 200 - 100.0f);

            registry.addComponent(sheep, Transform3D{
                Vec3{startX + offsetX, 20.0f, startZ + offsetZ},
                15.0f, 15.0f, 15.0f, // 
                0.9f, 0.9f, 0.9f     // 
                });
            registry.addComponent(sheep, Velocity3D{ Vec3{0,0,0} });
            registry.addComponent(sheep, SheepTag{});
            registry.addComponent(sheep, SheepComponent{}); // 
            registry.addComponent(sheep, PhysicsTag{ true}); //
            registry.addComponent(sheep, AnimalTag{});
            WeaponInventory inventory;
			registry.addComponent(sheep, inventory);
        }
    }

    void Update(EntityManager& registry, float dtMs) {
        float dt = dtMs / 10.0f;
        if (dt <= 0) return;

        Vec3 targetPos;
        bool hasPlayer = false;
        View<PlayerTag, Transform3D> playerView(registry);
        for (auto id : playerView) {
            targetPos = playerView.get<Transform3D>(id).pos;
            hasPlayer = true;
            break; // ֻ����һ�����
        }

        // 2. �ռ����е���λ�� (���ڿ־��߼�)
        std::vector<Vec3> enemies;
        View<EnemyTag, Transform3D> enemyView(registry);
        for (auto id : enemyView) {
            enemies.push_back(enemyView.get<Transform3D>(id).pos);
        }

        // 3. �����ռ�����
        static SpatialGrid grid(80.0f); // ���Ӵ�С�Դ�����Ұ�뾶
        grid.Clear();
        View<SheepTag, Transform3D, Velocity3D, SheepComponent> sheepView(registry);

        for (auto id : sheepView) {
            auto& pos = sheepView.get<Transform3D>(id).pos;
            grid.Insert(id, pos.x, pos.z);
        }

        // 4. Boids ѭ��
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

            // --- �����ھ� ---
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

                    // 1. Separation: Զ���ھ�
                    // Ȩ�������˥�� (Խ������Խ��)
                    // Enhanced: stronger repulsion for very close neighbors
                    float mySheepSize = t.width;
                    float otherSheepSize = ot.width;
                    float minDist = (mySheepSize + otherSheepSize) * 0.55f;  // Minimum safe distance
                    
                    if (dist < minDist) {
                        // Very close - strong repulsion to prevent overlap
                        forceSep.x += (dx / dist) * (minDist - dist) / dist * 2.0f;
                        forceSep.z += (dz / dist) * (minDist - dist) / dist * 2.0f;
                    } else {
                        // Normal separation
                        forceSep.x += (dx / dist) / dist;
                        forceSep.z += (dz / dist) / dist;
                    }

                    // 2. Alignment: ģ���ھ��ٶ�
                    forceAli.x += ov.vel.x;
                    forceAli.z += ov.vel.z;

                    // 3. Cohesion: ��¼�ھ�λ��
                    forceCoh.x += ot.pos.x;
                    forceCoh.z += ot.pos.z;

                    neighborCount++;
                }
            }

            if (neighborCount > 0) {
                // Alignment ƽ����
                forceAli.x /= neighborCount;
                forceAli.z /= neighborCount;
                forceAli = Norm(forceAli);

                // Cohesion ƽ����������
                forceCoh.x /= neighborCount;
                forceCoh.z /= neighborCount;
                // Cohesion �����ڡ���ȥ���ĵ㡱�������� -> (Center - CurrentPos)
                forceCoh.x -= pos.x;
                forceCoh.z -= pos.z;
                forceCoh = Norm(forceCoh);

                // Separation �Ѿ����ۼӵĹ�һ��������������� count
            }

            // 4. Target: �������
            if (hasPlayer) {
                float dx = targetPos.x - pos.x;
                float dz = targetPos.z - pos.z;
                float distToTarget = std::sqrt(dx * dx + dz * dz);

                // ��������Զ������������
                if (distToTarget > 60.0f) { // ����һ�����룬�������������
                    forceTar = { dx, 0, dz };
                    forceTar = Norm(forceTar);
                }
                else if (distToTarget < 30.0f) {
                    // ��̫������΢�ÿ�һ��
                    forceTar = { -dx, 0, -dz };
                    forceTar = Norm(forceTar);
                }
            }

            // 5. Fear: ��ܵ���
            for (const auto& enemyPos : enemies) {
                float dx = pos.x - enemyPos.x;
                float dz = pos.z - enemyPos.z;
                float distSq = dx * dx + dz * dz;

                if (distSq < params.enemyDetectRange * params.enemyDetectRange) {
                    // �����޴�ķ������
                    Vec3 fleeDir = { dx, 0, dz };
                    fleeDir = Norm(fleeDir);
                    forceFear.x += fleeDir.x;
                    forceFear.z += fleeDir.z;
                }
            }

            // --- ���ĺϳ� ---
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

            // ����ת���� (Steering Force)
            Limit(totalForce, params.maxForce);

            // --- Ӧ������ ---
            // Vel += Force * dt
            vel.x += totalForce.x * dt;
            vel.z += totalForce.z * dt;

            // ��������ٶ�
            Limit(vel, params.maxSpeed);

            // �򵥵����� (��ֹ��Զ����)
            vel.x *= 0.98f;
            vel.z *= 0.98f;

            // --- д����� ---
            // ע�⣺����ֻ�޸� X �� Z��Y ���� PhysicsSystem ��������
            v.vel.x = vel.x;
            v.vel.z = vel.z;

            // �򵥵ĳ������ (��ģ�������ƶ�����)
            if (Mag(vel) > 1.0f) {
                // ������� Rotation ���������������� Yaw
            }
        }
    }
}