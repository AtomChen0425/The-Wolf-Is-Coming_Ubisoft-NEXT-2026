#include "SheepSystem.h"
#include "../../System/Component/Component.h"
#include "../../System/Math/Math.h" 
#include "../ContestAPI/app.h"
#include <cmath>
#include <vector>
#include <unordered_map>

struct SpatialGrid {
    float cellSize;
    std::unordered_map<int, std::vector<EntityID>> grid;

    SpatialGrid(float size) : cellSize(size) {}
    int GetKey(float x, float z) {
        int cx = static_cast<int>(std::floor((x + 100000.0f) / cellSize));
        int cz = static_cast<int>(std::floor((z + 100000.0f) / cellSize));
        return cx * 73856093 ^ cz * 19349663; // 
    }
    void Clear() { grid.clear(); }

    void Insert(EntityID id, float x, float z) {
        grid[GetKey(x, z)].push_back(id);
    }
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

inline float Mag(const Vec3& v) { return std::sqrt(v.x * v.x + v.z * v.z); } // 
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
    void SheepShoot1(EntityManager& registry, float dt) {
        // Find all enemy positions
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
    void SheepShoot(EntityManager& registry, float dt) {
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
                float dy = targetPositions[i].y - sheepPos.y;
                float distSq = dx * dx + dz * dz + dy * dy;

                if (distSq < nearestDistSq) {
                    nearestDistSq = distSq;
                    nearestTarget = targetPositions[i];
                    foundTarget = true;
                }
            }
            if (!foundTarget) continue;

            float dx = nearestTarget.x - sheepPos.x;
            float dz = nearestTarget.z - sheepPos.z;
            float dy = nearestTarget.y - sheepPos.y;
            Vec3 bulletDirection = Normalize3D(Vec3{ dx, dy, dz });

            size_t weaponCount = inventory.weapons.size();

            const float totalArc = 1.5f * PI;
            const float radius = 10.0f; //

            float currentYaw = std::atan2(bulletDirection.x, bulletDirection.z);
            float startAngle = currentYaw - (totalArc / 2.0f);

            float angleStep = (weaponCount > 1) ? (totalArc / (weaponCount - 1)) : 0.0f;

            for (int i = 0; i < weaponCount; ++i) {
                Weapon& weapon = inventory.weapons[i];
                weapon.currentCooldown -= dt;

                if (weapon.currentCooldown > 0.0f) continue;
                // Fire a bullet
                float theta = (weaponCount > 1) ? (startAngle + i * angleStep) : currentYaw;

                Vec3 circleOffset = Vec3{
                    std::sin(theta) * radius,
                    0.0f,
                    std::cos(theta) * radius
                };

                // 
                Vec3 bulletPosition = sheepPos + circleOffset;
                // Create bullet entity
                Entity bullet = registry.createEntity();
                registry.addComponent(bullet, Bullet{ bulletDirection ,weapon.projectileSpeed, weapon.projectileLife,weapon.damage,true,weapon.explosionRadius,weapon.projectileSize,weapon.knockback });
                registry.addComponent(bullet, Transform3D{ bulletPosition, weapon.projectileSize, weapon.projectileSize, weapon.projectileSize, weapon.r, weapon.g, weapon.b });
                registry.addComponent(bullet, Velocity3D{ bulletDirection * weapon.projectileSpeed });
                registry.addComponent(bullet, TrailEmitter{ 50.0f, 0.0f, 150.0f, 3.0f, 1.0f, 0.5f, 0.0f });
                weapon.currentCooldown = weapon.fireRate;
            }
        }
    }
    void InitSheep(EntityManager& registry, float startX, float startZ, int count) {
        for (int i = 0; i < count; i++) {
            Entity sheep = registry.createEntity();
            float offsetX = (rand() % 200 - 100.0f);
            float offsetZ = (rand() % 200 - 100.0f);

            registry.addComponent(sheep, Transform3D{
                Vec3{startX + offsetX, 30.0f, startZ + offsetZ},
                15.0f, 15.0f, 15.0f, // 
                0.9f, 0.9f, 0.9f     // 
                });
            registry.addComponent(sheep, Velocity3D{ Vec3{0,0,0} });
            registry.addComponent(sheep, SheepTag{});
            registry.addComponent(sheep, SheepComponent{}); // 
            registry.addComponent(sheep, PhysicsTag{ true }); //
            registry.addComponent(sheep, AnimalTag{});
            WeaponInventory inventory;
            registry.addComponent(sheep, inventory);

            CSimpleSprite* pSprite2 = App::CreateSprite("data/TestData/Sheep1.png", 2, 1);

            // 
            const float speed = 1.0f / 15.0f;
            pSprite2->CreateAnimation(ANIM_BACKWARDS, speed, { 0,1 });
            pSprite2->CreateAnimation(ANIM_LEFT, speed, { 0,1 });
            pSprite2->CreateAnimation(ANIM_RIGHT, speed, { 0,1 });
            pSprite2->CreateAnimation(ANIM_FORWARDS, speed, { 0,1 });
            pSprite2->SetScale(0.3f);
            // 


            registry.addComponent(sheep, SpriteComponent{ pSprite2, 0 });
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
            break; // 
        }

        //
        std::vector<Vec3> enemies;
        View<EnemyTag, Transform3D> enemyView(registry);
        for (auto id : enemyView) {
            enemies.push_back(enemyView.get<Transform3D>(id).pos);
        }

        // 
        static SpatialGrid grid(80.0f); // 
        grid.Clear();
        View<SheepTag, Transform3D, Velocity3D, SheepComponent> sheepView(registry);

        for (auto id : sheepView) {
            auto& pos = sheepView.get<Transform3D>(id).pos;
            grid.Insert(id, pos.x, pos.z);
        }

        // 4. Boids
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

                    // Enhanced: stronger repulsion for very close neighbors
                    float mySheepSize = t.width;
                    float otherSheepSize = ot.width;
                    float minDist = (mySheepSize + otherSheepSize) * 0.55f;  // Minimum safe distance

                    if (dist < minDist) {
                        // Very close - strong repulsion to prevent overlap
                        forceSep.x += (dx / dist) * (minDist - dist) / dist * 2.0f;
                        forceSep.z += (dz / dist) * (minDist - dist) / dist * 2.0f;
                    }
                    else {
                        // Normal separation
                        forceSep.x += (dx / dist) / dist;
                        forceSep.z += (dz / dist) / dist;
                    }

                    // 2. Alignment
                    forceAli.x += ov.vel.x;
                    forceAli.z += ov.vel.z;

                    // 3. Cohesion
                    forceCoh.x += ot.pos.x;
                    forceCoh.z += ot.pos.z;

                    neighborCount++;
                }
            }

            if (neighborCount > 0) {
                // Alignment 
                forceAli.x /= neighborCount;
                forceAli.z /= neighborCount;
                forceAli = Norm(forceAli);

                // Cohesion
                forceCoh.x /= neighborCount;
                forceCoh.z /= neighborCount;
                // Cohesion 
                forceCoh.x -= pos.x;
                forceCoh.z -= pos.z;
                forceCoh = Norm(forceCoh);

                // Separation 
            }

			// 4. Target: follow player
            if (hasPlayer) {
                float dx = targetPos.x - pos.x;
                float dz = targetPos.z - pos.z;
                float distToTarget = std::sqrt(dx * dx + dz * dz);

                // 
                if (distToTarget > 60.0f) { // 
                    forceTar = { dx, 0, dz };
                    forceTar = Norm(forceTar);
                }
                else if (distToTarget < 30.0f) {
                    // 
                    forceTar = { -dx, 0, -dz };
                    forceTar = Norm(forceTar);
                }
            }

            // 5. Fear
            for (const auto& enemyPos : enemies) {
                float dx = pos.x - enemyPos.x;
                float dz = pos.z - enemyPos.z;
                float distSq = dx * dx + dz * dz;

                if (distSq < params.enemyDetectRange * params.enemyDetectRange) {

                    Vec3 fleeDir = { dx, 0, dz };
                    fleeDir = Norm(fleeDir);
                    forceFear.x += fleeDir.x;
                    forceFear.z += fleeDir.z;
                }
            }


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


            Limit(totalForce, params.maxForce);

            // 
            // Vel += Force * dt
            vel.x += totalForce.x * dt;
            vel.z += totalForce.z * dt;

            //
            Limit(vel, params.maxSpeed);

            // 
            vel.x *= 0.98f;
            vel.z *= 0.98f;

            //
            v.vel.x = vel.x;
            v.vel.z = vel.z;

            // 
            if (Mag(vel) > 1.0f) {

            }
        }
    }
}