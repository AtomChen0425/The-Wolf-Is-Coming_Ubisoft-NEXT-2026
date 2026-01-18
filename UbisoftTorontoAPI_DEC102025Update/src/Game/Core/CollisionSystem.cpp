#include "CollisionSystem.h"
#include "../../System/Component/Component.h"
#include "../../System/Physic/Collision.h"
#include "../../Game/Core/ParticleSystem.h"
#include "WolfSystem.h"
#include <cmath>
#include "../System/Math/Math.h"
#include <unordered_map>
Collision* gCollision;
struct CollisionGrid {
    float cellSize;
    std::unordered_map<int, std::vector<EntityID>> grid;

    CollisionGrid(float size) : cellSize(size) {}

    int GetKey(float x, float z) {
        int cx = static_cast<int>(std::floor((x + 100000.0f) / cellSize));
        int cz = static_cast<int>(std::floor((z + 100000.0f) / cellSize));
        return cx * 73856093 ^ cz * 19349663;
    }

    void Clear() {
        grid.clear();
    }

    void Insert(EntityID id, const Transform3D& t) {
        float minX = t.pos.x - t.width / 2.0f;
        float maxX = t.pos.x + t.width / 2.0f;
        float minZ = t.pos.z - t.depth / 2.0f;
        float maxZ = t.pos.z + t.depth / 2.0f;

        int startX = static_cast<int>(std::floor((minX + 100000.0f) / cellSize));
        int endX = static_cast<int>(std::floor((maxX + 100000.0f) / cellSize));
        int startZ = static_cast<int>(std::floor((minZ + 100000.0f) / cellSize));
        int endZ = static_cast<int>(std::floor((maxZ + 100000.0f) / cellSize));

        for (int x = startX; x <= endX; x++) {
            for (int z = startZ; z <= endZ; z++) {
                int key = x * 73856093 ^ z * 19349663;
                grid[key].push_back(id);
            }
        }
    }

    void GetPotentialColliders(float x, float z, std::vector<EntityID>& outList) {
        int cx = static_cast<int>(std::floor((x + 100000.0f) / cellSize));
        int cz = static_cast<int>(std::floor((z + 100000.0f) / cellSize));

        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                int key = (cx + i) * 73856093 ^ (cz + j) * 19349663;
                auto it = grid.find(key);
                if (it != grid.end()) {
                    outList.insert(outList.end(), it->second.begin(), it->second.end());
                }
            }
        }
    }
};

void CheckBulletHitMap(EntityManager& registry) {
    View<Bullet, Transform3D> bulletView(registry);
    View<Collider3D, Transform3D, MapBlockTag> colliderView(registry);

    // 1. 静态容器复用内存，避免每帧 new/delete
    static std::vector<Entity> bulletToRemove;
    bulletToRemove.clear();

    // 定义网格大小，建议设为比最大的地图块稍大一些 (例如 100-200)
    static CollisionGrid grid(200.0f);
    grid.Clear(); // 每帧清空，处理动态加载/卸载的地图块

    // 2. 【宽阶段】将所有地图碰撞体填入网格
    // 这是 O(M)，但 M 是地图块数量，比 O(N*M) 快得多
    for (EntityID id : colliderView) {
        auto& t = colliderView.get<Transform3D>(id);
        grid.Insert(id, t);
    }

    // 预分配一个候选列表，避免循环内反复分配
    static std::vector<EntityID> potentialColliders;

    for (EntityID bulletId : bulletView) {
        auto& bullet = bulletView.get<Bullet>(bulletId);
        auto& bulletTransform = bulletView.get<Transform3D>(bulletId);
        Vec3 bulletPos = bulletTransform.pos;

        // 预计算子弹 AABB (只计算一次)
        Vec3 bulletHalf(bulletTransform.width / 2, bulletTransform.height / 2, bulletTransform.depth / 2);
        Vec3 bulletMin = bulletPos - bulletHalf;
        Vec3 bulletMax = bulletPos + bulletHalf;

        // 3. 【宽阶段查询】只获取附近的地图块
        potentialColliders.clear();
        grid.GetPotentialColliders(bulletPos.x, bulletPos.z, potentialColliders);

        // 如果周围没有方块，直接跳过
        if (potentialColliders.empty()) continue;

        // 4. 去重 (因为一个大方块可能横跨多个网格，会被添加多次)
        std::sort(potentialColliders.begin(), potentialColliders.end());
        auto last = std::unique(potentialColliders.begin(), potentialColliders.end());
        potentialColliders.erase(last, potentialColliders.end());

        // 5. 【窄阶段】只对候选方块进行 AABB 检测
        for (EntityID colliderId : potentialColliders) {
            // 安全检查：防止该 Entity 在同一帧被其他逻辑销毁了
            //if (!registry.has<Transform3D>(colliderId)) continue;

            auto& colliderTransform = colliderView.get<Transform3D>(colliderId);

            Vec3 colliderHalf(colliderTransform.width / 2, colliderTransform.height / 2, colliderTransform.depth / 2);
            Vec3 colliderMin = colliderTransform.pos - colliderHalf;
            Vec3 colliderMax = colliderTransform.pos + colliderHalf;

            if (gCollision->AABB3D(bulletMin, bulletMax, colliderMin, colliderMax)) {

                // --- 命中逻辑 ---
                if (bullet.explosionRadius > 0.0f) {
                    ParticleSystem::CreateExplosion(registry, bulletTransform.pos, 50, Vec3{ 1, 0.5f, 0 }, 100.0f);

                    // 注意：这里仍然遍历所有敌人 (O(Enemy))
                    // 如果敌人很多，建议也为敌人建立一个 Grid，或者简单地只计算距离平方
                    View<EnemyTag, Transform3D, Health> allEnemies(registry);
                    float radiusSq = bullet.explosionRadius * bullet.explosionRadius; // 预计算平方

                    for (EntityID otherId : allEnemies) {
                        auto& otherT = allEnemies.get<Transform3D>(otherId);

                        // 优化：使用距离平方 (DistanceSquared3D) 避免开方运算
                        float distSq = Distance3D(otherT.pos, bulletTransform.pos);

                        if (distSq <= radiusSq) {
                            auto& hp = allEnemies.get<Health>(otherId);
                            hp.currentHealth -= bullet.damage;
                            if (hp.currentHealth <= 0) {
                                ParticleSystem::CreateExplosion(registry, otherT.pos, 20, Vec3{ 1.0f, 0.0f, 0.0f }, 200.0f);
                                // 使用带版本号的Entity结构以安全删除
                                bulletToRemove.push_back({ otherId, registry.getEntityVersion(otherId) });
                            }
                        }
                    }
                }

                // 标记删除子弹
                bulletToRemove.push_back({ bulletId, registry.getEntityVersion(bulletId) });
                break; // 子弹撞墙后销毁，不再检测其他墙壁
            }
        }
    }

    // 6. 统一删除
    for (Entity& e : bulletToRemove) {
        if (registry.isValid(e)) {
            registry.destroyEntity(e);
        }
    }
}
// 2D collision for player-enemy
void CheckPlayerEnemyCollision(EntityManager& registry) {
    View<PlayerTag, Position, Velocity, RigidBody, Health> playerView(registry);
    View<EnemyTag, Position> enemyView(registry);
    EntityID playerId{};
    Vec2 playerPos;
    float playerRadius = 0.0f;
    for (EntityID id : playerView) {
        playerId = id;
        playerPos = playerView.get<Position>(id).pos;
        playerRadius = playerView.get<RigidBody>(id).radius;
        break;
    }

    for (EntityID enemyId : enemyView) {
        auto& enemyPos = enemyView.get<Position>(enemyId);
        float enemyRadius = enemyView.get<RigidBody>(enemyId).radius;
        if (gCollision->Circle(playerPos, playerRadius, enemyPos.pos, enemyRadius)) {
            auto& playerHealth = playerView.get<Health>(playerId);
            playerHealth.currentHealth -= 10;

            auto& playerRigidBody = playerView.get<RigidBody>(playerId);

            Vec2 dir = Normalize(playerPos - enemyPos.pos);
            const float knockbackImpulse = 10.0f;

            playerRigidBody.force = dir * knockbackImpulse;
            registry.destroyEntity({ enemyId, registry.getEntityVersion(enemyId) });
            break;
        }
    }
}

// Simplified 3D collision system - just detect and respond
void CheckPlayer3DCollisions(EntityManager& registry) {
    View<PlayerTag, Transform3D, Velocity3D> playerView(registry);
    
    for (EntityID playerId : playerView) {
        auto& playerTag = playerView.get<PlayerTag>(playerId);
        auto& playerTransform = playerView.get<Transform3D>(playerId);
        auto& vel = playerView.get<Velocity3D>(playerId).vel;
        Vec3& pos = playerTransform.pos;
        
        // Reset ground flag
        playerTag.isOnGround = false;
        
        // Player bounding box
        Vec3 playerMin(pos.x - playerTransform.width / 2, 
                       pos.y - playerTransform.height / 2, 
                       pos.z - playerTransform.depth / 2);
        Vec3 playerMax(pos.x + playerTransform.width / 2, 
                       pos.y + playerTransform.height / 2, 
                       pos.z + playerTransform.depth / 2);
        
        // Check all colliders
        View<Collider3D, Transform3D> colliderView(registry);
        float highestFloor = -1000.0f;
        
        for (EntityID colliderId : colliderView) {
            auto& collider = colliderView.get<Collider3D>(colliderId);
            auto& transform = colliderView.get<Transform3D>(colliderId);
            
            // Collider bounding box
            Vec3 colliderMin(transform.pos.x - transform.width / 2,
                            transform.pos.y - transform.height / 2,
                            transform.pos.z - transform.depth / 2);
            Vec3 colliderMax(transform.pos.x + transform.width / 2,
                            transform.pos.y + transform.height / 2,
                            transform.pos.z + transform.depth / 2);
            
            // Check collision using AABB
            if (gCollision->AABB3D(playerMin, playerMax, colliderMin, colliderMax)) {
                // Calculate penetration on each axis
                float penetrationX = (playerMin.x < colliderMin.x) ? 
                    (colliderMin.x - playerMax.x) : (colliderMax.x - playerMin.x);
                float penetrationY = (playerMin.y < colliderMin.y) ? 
                    (colliderMin.y - playerMax.y) : (colliderMax.y - playerMin.y);
                float penetrationZ = (playerMin.z < colliderMin.z) ? 
                    (colliderMin.z - playerMax.z) : (colliderMax.z - playerMin.z);
                
                float absX = std::abs(penetrationX);
                float absY = std::abs(penetrationY);
                float absZ = std::abs(penetrationZ);
                
                // Resolve on minimum penetration axis
                if (absX < absY && absX < absZ) {
                    // X-axis collision (left/right wall)
                    pos.x += penetrationX;
                    vel.x = 0.0f;
                } else if (absZ < absY) {
                    // Z-axis collision (front/back wall)
                    pos.z += penetrationZ;
                    vel.z = 0.0f;
                } else {
                    // Y-axis collision (floor/ceiling)
                    if (penetrationY > 0) {
                        // Hit from below (floor)
                        if (collider.isFloor) {
                            float floorTop = colliderMax.y;
                            if (floorTop > highestFloor) {
                                highestFloor = floorTop;
                            }
                        }
                    } else {
                        // Hit from above (ceiling)
                        pos.y += penetrationY;
                        vel.y = 0.0f;
                    }
                }
                
                // Update bounding box after position change
                playerMin = Vec3(pos.x - playerTransform.width / 2, 
                               pos.y - playerTransform.height / 2, 
                               pos.z - playerTransform.depth / 2);
                playerMax = Vec3(pos.x + playerTransform.width / 2, 
                               pos.y + playerTransform.height / 2, 
                               pos.z + playerTransform.depth / 2);
            }
        }
        
        // Apply floor collision if detected
        if (highestFloor > -999.0f) {
            float playerBottom = pos.y - playerTransform.height / 2;
            if (playerBottom <= highestFloor + 2.0f) {
                pos.y = highestFloor + playerTransform.height / 2;
                vel.y = 0.0f;
                playerTag.isOnGround = true;
            }
        }
    }
}
//void CheckPhysics3DCollisions(EntityManager& registry) {
//    View<PhysicsTag, SheepTag, Transform3D, Velocity3D> entityView(registry);
//
//    for (EntityID entityId : entityView) {
//        auto& physicsTag = entityView.get<PhysicsTag>(entityId);
//        auto& entityTransform = entityView.get<Transform3D>(entityId);
//        auto& vel = entityView.get<Velocity3D>(entityId).vel;
//        Vec3& pos = entityTransform.pos;
//
//        // Reset ground flag
//        physicsTag.isOnGround = false;
//
//        // Entity bounding box
//        Vec3 entityMin(pos.x - entityTransform.width / 2,
//            pos.y - entityTransform.height / 2,
//            pos.z - entityTransform.depth / 2);
//        Vec3 entityMax(pos.x + entityTransform.width / 2,
//            pos.y + entityTransform.height / 2,
//            pos.z + entityTransform.depth / 2);
//
//        // Check all colliders
//        View<Collider3D, Transform3D> colliderView(registry);
//        float highestFloor = -1000.0f;
//
//        for (EntityID colliderId : colliderView) {
//            auto& collider = colliderView.get<Collider3D>(colliderId);
//            auto& transform = colliderView.get<Transform3D>(colliderId);
//
//            // Collider bounding box
//            Vec3 colliderMin(transform.pos.x - transform.width / 2,
//                transform.pos.y - transform.height / 2,
//                transform.pos.z - transform.depth / 2);
//            Vec3 colliderMax(transform.pos.x + transform.width / 2,
//                transform.pos.y + transform.height / 2,
//                transform.pos.z + transform.depth / 2);
//
//            // Check collision using AABB
//            if (gCollision->AABB3D(entityMin, entityMax, colliderMin, colliderMax)) {
//                // Calculate penetration on each axis
//                float penetrationX = (entityMin.x < colliderMin.x) ?
//                    (colliderMin.x - entityMax.x) : (colliderMax.x - entityMin.x);
//                float penetrationY = (entityMin.y < colliderMin.y) ?
//                    (colliderMin.y - entityMax.y) : (colliderMax.y - entityMin.y);
//                float penetrationZ = (entityMin.z < colliderMin.z) ?
//                    (colliderMin.z - entityMax.z) : (colliderMax.z - entityMin.z);
//
//                float absX = std::abs(penetrationX);
//                float absY = std::abs(penetrationY);
//                float absZ = std::abs(penetrationZ);
//
//                // Resolve on minimum penetration axis
//                if (absX < absY && absX < absZ) {
//                    // X-axis collision (left/right wall)
//                    pos.x += penetrationX;
//                    vel.x = 0.0f;
//                }
//                else if (absZ < absY) {
//                    // Z-axis collision (front/back wall)
//                    pos.z += penetrationZ;
//                    vel.z = 0.0f;
//                }
//                else {
//                    // Y-axis collision (floor/ceiling)
//                    if (penetrationY > 0) {
//                        // Hit from below (floor)
//                        if (collider.isFloor) {
//                            float floorTop = colliderMax.y;
//                            if (floorTop > highestFloor) {
//                                highestFloor = floorTop;
//                            }
//                        }
//                    }
//                    else {
//                        // Hit from above (ceiling)
//                        pos.y += penetrationY;
//                        vel.y = 0.0f;
//                    }
//                }
//
//                // Update bounding box after position change
//                entityMin = Vec3(pos.x - entityTransform.width / 2,
//                    pos.y - entityTransform.height / 2,
//                    pos.z - entityTransform.depth / 2);
//                entityMax = Vec3(pos.x + entityTransform.width / 2,
//                    pos.y + entityTransform.height / 2,
//                    pos.z + entityTransform.depth / 2);
//            }
//        }
//
//        // Apply floor collision if detected
//        if (highestFloor > -999.0f) {
//            float entityBottom = pos.y - entityTransform.height / 2;
//            if (entityBottom <= highestFloor + 2.0f) {
//                pos.y = highestFloor + entityTransform.height / 2;
//                vel.y = 0.0f;
//                physicsTag.isOnGround = true;
//            }
//        }
//    }
//}

void CheckPhysics3DCollisions(EntityManager& registry) {
    // A. 准备阶段：构建静态碰撞体的空间网格
    // 假设格子大小为 100 (根据你的地图块大小调整，建议略大于最大的墙)
    static CollisionGrid colGrid(120.0f);
    colGrid.Clear();

    View<Collider3D, Transform3D> colliderView(registry);

    // O(M): 遍历所有墙/地，放入网格
    for (EntityID id : colliderView) {
        auto& t = colliderView.get<Transform3D>(id);
        colGrid.Insert(id, t);
    }

    // B. 检测阶段：遍历所有动态实体
    View<PhysicsTag, Transform3D, Velocity3D> entityView(registry);

    // 用于缓存查询结果的 vector，避免在循环内反复分配内存
    static std::vector<EntityID> nearbyColliders;

    // O(N): 遍历所有物理实体
    for (EntityID entityId : entityView) {
        auto& physicsTag = entityView.get<PhysicsTag>(entityId);
        auto& entityTransform = entityView.get<Transform3D>(entityId);
        auto& vel = entityView.get<Velocity3D>(entityId).vel;
        Vec3& pos = entityTransform.pos;

        physicsTag.isOnGround = false;

        // 实体 AABB
        Vec3 entityMin(pos.x - entityTransform.width / 2,
            pos.y - entityTransform.height / 2,
            pos.z - entityTransform.depth / 2);
        Vec3 entityMax(pos.x + entityTransform.width / 2,
            pos.y + entityTransform.height / 2,
            pos.z + entityTransform.depth / 2);

        // --- 优化核心：只获取附近的碰撞体 ---
        nearbyColliders.clear();
        colGrid.GetPotentialColliders(pos.x, pos.z, nearbyColliders);

        float highestFloor = -1000.0f;

        // O(k): k 是附近碰撞体的数量，通常只有 个位数
        for (EntityID colliderId : nearbyColliders) {
            // 防止重复检测 (如果一个物体跨了多个格子，可能会在 list 里出现多次)
            // 简单的做法是不管它，反正 AABB 计算很快。
            // 严谨的做法可以用一个 visited set，但 vector 遍历通常更快。

            // 注意：因为 nearbyColliders 存的是 ID，我们需要重新获取组件引用
            // 这里假设 registry.getComponent 是 O(1) 的 (数组索引)
            auto* pCollider = registry.getComponent<Collider3D>(Entity{ colliderId, registry.getEntityVersion(colliderId) });
            auto* pTransform = registry.getComponent<Transform3D>(Entity{ colliderId, registry.getEntityVersion(colliderId) });

            if (!pCollider || !pTransform) continue;

            auto& collider = *pCollider;
            auto& transform = *pTransform;

            // 碰撞体 AABB
            Vec3 colliderMin(transform.pos.x - transform.width / 2,
                transform.pos.y - transform.height / 2,
                transform.pos.z - transform.depth / 2);
            Vec3 colliderMax(transform.pos.x + transform.width / 2,
                transform.pos.y + transform.height / 2,
                transform.pos.z + transform.depth / 2);
            // AABB 检测
            if (gCollision->AABB3D(entityMin, entityMax, colliderMin, colliderMax)) {
                // ... 你的原有逻辑保持不变 ...

                float penetrationX = (entityMin.x < colliderMin.x) ?
                    (colliderMin.x - entityMax.x) : (colliderMax.x - entityMin.x);
                float penetrationY = (entityMin.y < colliderMin.y) ?
                    (colliderMin.y - entityMax.y) : (colliderMax.y - entityMin.y);
                float penetrationZ = (entityMin.z < colliderMin.z) ?
                    (colliderMin.z - entityMax.z) : (colliderMax.z - entityMin.z);

                float absX = std::abs(penetrationX);
                float absY = std::abs(penetrationY);
                float absZ = std::abs(penetrationZ);

                if (absX < absY && absX < absZ) {
                    pos.x += penetrationX;
                    vel.x = 0.0f;
                }
                else if (absZ < absY) {
                    pos.z += penetrationZ;
                    vel.z = 0.0f;
                }
                else {
                    if (penetrationY > 0) {
                        if (collider.isFloor) {
                            float floorTop = colliderMax.y;
                            if (floorTop > highestFloor) {
                                highestFloor = floorTop;
                            }
                        }
                    }
                    else {
                        pos.y += penetrationY;
                        vel.y = 0.0f;
                    }
                }
                // 更新 AABB 以便下一次检测准确
                entityMin = Vec3(pos.x - entityTransform.width / 2,
                    pos.y - entityTransform.height / 2,
                    pos.z - entityTransform.depth / 2);
                entityMax = Vec3(pos.x + entityTransform.width / 2,
                    pos.y + entityTransform.height / 2,
                    pos.z + entityTransform.depth / 2);
            }
        }

        // 地面吸附逻辑
        if (highestFloor > -999.0f) {
            float entityBottom = pos.y - entityTransform.height / 2;
            // 稍微放宽一点判定范围，防止抖动
            if (entityBottom <= highestFloor + 4.0f) {
                pos.y = highestFloor + entityTransform.height / 2;
                vel.y = 0.0f;
                physicsTag.isOnGround = true;
            }
        }
    }
}
void CheckPlayerGetPoints(EntityManager& registry) {
    View<PlayerTag, Transform3D> playerView(registry);
    EntityID playerId{};
    Vec3 playerPos;
    Transform3D playerTransform;
    float playerRadius = 0.0f;
	for (EntityID id : playerView) {
        playerId = id;
		playerTransform = playerView.get<Transform3D>(id);
        playerPos = playerView.get<Transform3D>(id).pos;
        playerRadius = playerTransform.width / 2; // Assume width as diameter
        break;
    }
    Vec3 playerMin(playerPos.x - playerTransform.width / 2,
        playerPos.y - playerTransform.height / 2,
        playerPos.z - playerTransform.depth / 2);
    Vec3 playerMax(playerPos.x + playerTransform.width / 2,
        playerPos.y + playerTransform.height / 2,
        playerPos.z + playerTransform.depth / 2);
    View<ScorePointTag, Transform3D> scoreView(registry);
    for (EntityID scoreId : scoreView) {
        auto& scoreTransform = scoreView.get<Transform3D>(scoreId);
        float scoreRadius = scoreTransform.width / 2; // Assume width as diameter
        Vec3 colliderMin(scoreTransform.pos.x - scoreTransform.width / 2,
            scoreTransform.pos.y - scoreTransform.height / 2,
            scoreTransform.pos.z - scoreTransform.depth / 2);
        Vec3 colliderMax(scoreTransform.pos.x + scoreTransform.width / 2,
            scoreTransform.pos.y + scoreTransform.height / 2,
            scoreTransform.pos.z + scoreTransform.depth / 2);
        if (gCollision->AABB3D(playerMin, playerMax, colliderMin, colliderMax)) {
            auto& scoreTag = scoreView.get<ScorePointTag>(scoreId);
            scoreTag.collected = true;
			auto& playerTag = playerView.get<PlayerTag>(playerId);
			playerTag.score += scoreTag.points;
        }
	}
}
void CheckBulletDamage(EntityManager& registry) {
    static std::vector<Entity> EnemyToRemove;
    EnemyToRemove.clear();
    static std::vector<Entity> bulletToRemove;
    bulletToRemove.clear();
	View<PlayerTag> playerView(registry);
	int* playerScore = nullptr;
    for (EntityID id : playerView) {
        playerScore = &playerView.get<PlayerTag>(id).score;
        break;
	}
    View<Bullet, Transform3D> bulletView(registry);
    View<Health, Transform3D,EnemyTag> enemyView(registry);
    for (EntityID bulletId : bulletView) {
		auto& bullet = bulletView.get<Bullet>(bulletId);
        if (!bullet.isPlayerBullet) {
            continue; // Skip enemy bullets
		}
		float bulletDamage = bullet.damage;
        float bulletKnockback = bullet.knockback;
        auto& bulletTransform = bulletView.get<Transform3D>(bulletId);
        Vec3 bulletPos = bulletTransform.pos;
        Vec3 bulletMin(bulletPos.x - bulletTransform.width / 2,
            bulletPos.y - bulletTransform.height / 2,
            bulletPos.z - bulletTransform.depth / 2);
        Vec3 bulletMax(bulletPos.x + bulletTransform.width / 2,
            bulletPos.y + bulletTransform.height / 2,
            bulletPos.z + bulletTransform.depth / 2);
        for (EntityID enemyId : enemyView) {
            auto& enemyHealth = enemyView.get<Health>(enemyId);
			auto& enemyTransform = enemyView.get<Transform3D>(enemyId);
			auto& enemyVel = enemyView.get<Velocity3D>(enemyId).vel;
            Vec3 colliderMin(enemyTransform.pos.x - enemyTransform.width / 2,
                enemyTransform.pos.y - enemyTransform.height / 2,
                enemyTransform.pos.z - enemyTransform.depth / 2);
            Vec3 colliderMax(enemyTransform.pos.x + enemyTransform.width / 2,
                enemyTransform.pos.y + enemyTransform.height / 2,
                enemyTransform.pos.z + enemyTransform.depth / 2);
            if (gCollision->AABB3D(bulletMin, bulletMax, colliderMin, colliderMax)) {
				bulletToRemove.push_back({ bulletId, registry.getEntityVersion(bulletId) });
				Vec3 enemyDirection = Normalize3D(enemyVel);
				enemyVel =  enemyDirection * ( - bulletKnockback); // Apply knockback
                enemyHealth.currentHealth -= bulletDamage;
                ParticleSystem::CreateExplosion(registry, bulletTransform.pos, 5, Vec3{ 1.0f, 1.0f, 0.0f }, 150.0f);
                if (enemyHealth.currentHealth <= 0) {
                    ParticleSystem::CreateExplosion(registry, enemyTransform.pos, 20, Vec3{ 1.0f, 0.0f, 0.0f }, 200.0f);
                    EnemyToRemove.push_back({ enemyId, registry.getEntityVersion(enemyId) });
                }
                if (bullet.explosionRadius > 0.0f) {
                    // 创建爆炸特效
                    ParticleSystem::CreateExplosion(registry, bulletTransform.pos, 50, Vec3{ 1, 0.5f, 0 }, 100.0f);

                    // 查找爆炸范围内的所有敌人
                    View<EnemyTag, Transform3D, Health> allEnemies(registry);
                    for (EntityID otherId : allEnemies) {
                        auto& otherT = allEnemies.get<Transform3D>(otherId);
                        float dist = Distance3D(otherT.pos, bulletTransform.pos);

                        if (dist <= bullet.explosionRadius) {
                            auto& hp = allEnemies.get<Health>(otherId);
                            hp.currentHealth -= bullet.damage;
                            if (hp.currentHealth <= 0) {
                                ParticleSystem::CreateExplosion(registry, otherT.pos, 20, Vec3{ 1.0f, 0.0f, 0.0f }, 200.0f);
                                EnemyToRemove.push_back({ otherId, registry.getEntityVersion(otherId) });
                            }
                        }

                    }
                }
				break; // Bullet can hit only one enemy
            }
        }
    }
    for (Entity& e : bulletToRemove) {
        registry.destroyEntity(e);
	}
    for (Entity& e : EnemyToRemove) {
        *playerScore += 100;
        registry.destroyEntity(e);
	}
}

void CheckWolfEatSheep(EntityManager& registry) {
    View<WolfTag, Transform3D> wolfView(registry);
    View<SheepTag, Transform3D> sheepView(registry);
    static std::vector<Entity> sheepToRemove;
    sheepToRemove.clear();
    for (EntityID wolfId : wolfView) {
        auto& wolfTransform = wolfView.get<Transform3D>(wolfId);
        Vec3 wolfPos = wolfTransform.pos;
        Vec3 wolfMin(wolfPos.x - wolfTransform.width / 2,
            wolfPos.y - wolfTransform.height / 2,
            wolfPos.z - wolfTransform.depth / 2);
        Vec3 wolfMax(wolfPos.x + wolfTransform.width / 2,
            wolfPos.y + wolfTransform.height / 2,
            wolfPos.z + wolfTransform.depth / 2);
        for (EntityID sheepId : sheepView) {
            auto& sheepTransform = sheepView.get<Transform3D>(sheepId);
            Vec3 colliderMin(sheepTransform.pos.x - sheepTransform.width / 2,
                sheepTransform.pos.y - sheepTransform.height / 2,
                sheepTransform.pos.z - sheepTransform.depth / 2);
            Vec3 colliderMax(sheepTransform.pos.x + sheepTransform.width / 2,
                sheepTransform.pos.y + sheepTransform.height / 2,
                sheepTransform.pos.z + sheepTransform.depth / 2);
            if (gCollision->AABB3D(wolfMin, wolfMax, colliderMin, colliderMax)) {
				sheepToRemove.push_back({ sheepId, registry.getEntityVersion(sheepId) });
            }
        }
    }
    for (const Entity& e : sheepToRemove) {
        registry.destroyEntity(e);
	}
}

void CheckWolfHeartsPlayer(EntityManager& registry) {
    View<WolfTag, Transform3D> wolfView(registry);
    View<PlayerTag, Transform3D,Health> playerView(registry);
   /* static std::vector<Entity> playersToRemove;
    playersToRemove.clear();*/
    for (EntityID wolfId : wolfView) {
        auto& wolfTransform = wolfView.get<Transform3D>(wolfId);
        Vec3 wolfPos = wolfTransform.pos;
        Vec3 wolfMin(wolfPos.x - wolfTransform.width / 2,
            wolfPos.y - wolfTransform.height / 2,
            wolfPos.z - wolfTransform.depth / 2);
        Vec3 wolfMax(wolfPos.x + wolfTransform.width / 2,
            wolfPos.y + wolfTransform.height / 2,
            wolfPos.z + wolfTransform.depth / 2);
        for (EntityID playerId : playerView) {
            auto& playerTransform = playerView.get<Transform3D>(playerId);
			auto& playerHealth = playerView.get<Health>(playerId);
            Vec3 colliderMin(playerTransform.pos.x - playerTransform.width / 2,
                playerTransform.pos.y - playerTransform.height / 2,
                playerTransform.pos.z - playerTransform.depth / 2);
            Vec3 colliderMax(playerTransform.pos.x + playerTransform.width / 2,
                playerTransform.pos.y + playerTransform.height / 2,
                playerTransform.pos.z + playerTransform.depth / 2);
            if (gCollision->AABB3D(wolfMin, wolfMax, colliderMin, colliderMax)) {
                playerHealth.currentHealth -= 10;
				Vec3 dir = Normalize3D(playerTransform.pos - wolfTransform.pos);
				const float knockbackImpulse = 20.0f;
				auto& playerVel = playerView.get<Velocity3D>(playerId).vel;
				playerVel = dir * knockbackImpulse;
            }
        }
    }

}

void CheckBulletHitMap1(EntityManager& registry) {
    View<Bullet, Transform3D> bulletView(registry);
    View<Collider3D, Transform3D,MapBlockTag> colliderView(registry);
    static std::vector<Entity> bulletToRemove;
    bulletToRemove.clear();
    for (EntityID bulletId : bulletView) {
        auto& bulletTransform = bulletView.get<Transform3D>(bulletId);
		auto& bullet = bulletView.get<Bullet>(bulletId);
        Vec3 bulletPos = bulletTransform.pos;
        Vec3 bulletMin(bulletPos.x - bulletTransform.width / 2,
            bulletPos.y - bulletTransform.height / 2,
            bulletPos.z - bulletTransform.depth / 2);
        Vec3 bulletMax(bulletPos.x + bulletTransform.width / 2,
            bulletPos.y + bulletTransform.height / 2,
            bulletPos.z + bulletTransform.depth / 2);
        for (EntityID colliderId : colliderView) {
            auto& colliderTransform = colliderView.get<Transform3D>(colliderId);
            Vec3 colliderMin(colliderTransform.pos.x - colliderTransform.width / 2,
                colliderTransform.pos.y - colliderTransform.height / 2,
                colliderTransform.pos.z - colliderTransform.depth / 2);
            Vec3 colliderMax(colliderTransform.pos.x + colliderTransform.width / 2,
                colliderTransform.pos.y + colliderTransform.height / 2,
                colliderTransform.pos.z + colliderTransform.depth / 2);
            if (gCollision->AABB3D(bulletMin, bulletMax, colliderMin, colliderMax)) {
                if (bullet.explosionRadius > 0.0f) {
                    // 创建爆炸特效
                    ParticleSystem::CreateExplosion(registry, bulletTransform.pos, 50, Vec3{ 1, 0.5f, 0 }, 100.0f);
                    View<EnemyTag, Transform3D, Health> allEnemies(registry);
                    for (EntityID otherId : allEnemies) {
                        auto& otherT = allEnemies.get<Transform3D>(otherId);
                        float dist = Distance3D(otherT.pos, bulletTransform.pos);

                        if (dist <= bullet.explosionRadius) {
                            auto& hp = allEnemies.get<Health>(otherId);
                            hp.currentHealth -= bullet.damage;
                            if (hp.currentHealth <= 0) {
                                ParticleSystem::CreateExplosion(registry, otherT.pos, 20, Vec3{ 1.0f, 0.0f, 0.0f }, 200.0f);
                                bulletToRemove.push_back({ otherId, registry.getEntityVersion(otherId) });
                            }
                        }

                    }
                }
                bulletToRemove.push_back({ bulletId, registry.getEntityVersion(bulletId) });
                break; // Bullet can hit only one collider
            }
        }
    }
    for (Entity& e : bulletToRemove) {
        registry.destroyEntity(e);
    }
}

void CheckEnemyBulletCollision(EntityManager& registry, const GameConfig& config) {
    View<Bullet, EnemyBulletTag, Transform3D> enemyBulletView(registry);
    // Get all potential targets (players and sheep)
    std::vector<Transform3D> targetTransform;
    std::vector<bool> isPlayerTarget;  // Track if target is player (true) or sheep (false)
	std::vector<EntityID> targetsEntityID;
    // Add all players as potential targets
    View<PlayerTag, Transform3D> playerView(registry);
    for (auto id : playerView) {
        targetTransform.push_back(playerView.get<Transform3D>(id));
        isPlayerTarget.push_back(true);
		targetsEntityID.push_back(id);
        break; // Only target the first player found
    }

    // Add all sheep as potential targets
    View<SheepTag, Transform3D> sheepView(registry);
    for (auto id : sheepView) {
        targetTransform.push_back(sheepView.get<Transform3D>(id));
        isPlayerTarget.push_back(false);
        targetsEntityID.push_back(id);
    }
    static std::vector<Entity> TargetToRemove;
    TargetToRemove.clear();
    static std::vector<Entity> bulletToRemove;
    bulletToRemove.clear();
    static std::vector<Vec3> wolfToAdd;
    wolfToAdd.clear();
    for (EntityID bulletId : enemyBulletView) {
        auto& bulletTransform = enemyBulletView.get<Transform3D>(bulletId);
		auto& bullet = enemyBulletView.get<Bullet>(bulletId);
        Vec3 bulletPos = bulletTransform.pos;
        Vec3 bulletMin(bulletPos.x - bulletTransform.width / 2,
            bulletPos.y - bulletTransform.height / 2,
            bulletPos.z - bulletTransform.depth / 2);
        Vec3 bulletMax(bulletPos.x + bulletTransform.width / 2,
            bulletPos.y + bulletTransform.height / 2,
            bulletPos.z + bulletTransform.depth / 2);
        for (size_t i = 0; i < targetTransform.size(); i++) {
            Vec3 targetPos = targetTransform[i].pos;
            Vec3 targetMin(targetPos.x - targetTransform[i].width / 2,
                targetPos.y - targetTransform[i].height / 2,
                targetPos.z - targetTransform[i].depth / 2);
            Vec3 targetMax(targetPos.x + targetTransform[i].width / 2,
                targetPos.y + targetTransform[i].height / 2,
                targetPos.z + targetTransform[i].depth / 2);
            if (gCollision->AABB3D(bulletMin, bulletMax, targetMin, targetMax)) {
                bulletToRemove.push_back({ bulletId, registry.getEntityVersion(bulletId) });
                if (isPlayerTarget[i]) {
                    // Handle player hit
                    auto& playerHealth = playerView.get<Health>(targetsEntityID[i]);
                    playerHealth.currentHealth -= bullet.damage;
                    Vec3 dir = Normalize3D(playerView.get<Transform3D>(targetsEntityID[i]).pos - bulletTransform.pos);
                    auto& playerVel = playerView.get<Velocity3D>(targetsEntityID[i]).vel;
                    playerVel = dir * bullet.knockback;
                } else {
                    // Handle sheep hit
					Entity& sheepEntity = Entity{ targetsEntityID[i],registry.getEntityVersion(targetsEntityID[i]) };
                    TargetToRemove.push_back(sheepEntity);
                    if (enemyBulletView.has<MagicTag>(bulletId)) {
                        wolfToAdd.push_back(targetPos);
                        ParticleSystem::CreateExplosion(registry, targetPos, 20, Vec3{ 0.0f, 1.0f, 0.0f }, 150.0f);
					}
					
                }
                break;
            }
        }
    }
    for (Entity& e : TargetToRemove) {
        registry.destroyEntity(e);
    }
    for (Entity& e : bulletToRemove) {
        registry.destroyEntity(e);
    }
    for (Vec3& pos : wolfToAdd) {
        WolfSystem::InitWolfOfType(registry, pos.x, pos.z, WolfType::Basic, config);
    }
}
void CollisionSystem::Update(EntityManager& registry, const GameConfig& config) {
    //CheckBulletHitMap(registry);
    //CheckPlayer3DCollisions(registry);
	CheckPhysics3DCollisions(registry);
    CheckPlayerGetPoints(registry);
    CheckBulletDamage(registry);
    CheckEnemyBulletCollision(registry, config);
	CheckWolfEatSheep(registry);
    CheckWolfHeartsPlayer(registry);    
}
