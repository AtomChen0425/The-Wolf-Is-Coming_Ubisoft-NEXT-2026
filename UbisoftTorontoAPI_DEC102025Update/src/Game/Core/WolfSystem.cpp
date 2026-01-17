#include "WolfSystem.h"
#include "../../System/Component/Component.h"
#include "../../System/Math/Math.h"
#include <cmath>
#include <vector>
#include <limits>

// --- Helper Math Functions ---
inline float WolfMag(const Vec3& v) { return std::sqrt(v.x * v.x + v.z * v.z); } // Only XZ plane magnitude
inline Vec3 WolfNorm(const Vec3& v) {
    float m = WolfMag(v);
    if (m > 0.0001f) return { v.x / m, 0, v.z / m };
    return { 0,0,0 };
}
inline void WolfLimit(Vec3& v, float max) {
    float mSq = v.x * v.x + v.z * v.z;
    if (mSq > max * max && mSq > 0) {
        float m = std::sqrt(mSq);
        v.x = (v.x / m) * max;
        v.z = (v.z / m) * max;
    }
}

namespace WolfSystem {

    void InitWolfOfType(EntityManager& registry, float x, float z, WolfType type) {
        Entity wolf = registry.createEntity();
        
        // Base stats that will be modified by type
        float size = 20.0f;
        float r = 0.4f, g = 0.2f, b = 0.1f;  // Color
        int maxHealth = 100;
        float chaseForce = 200.0f;
        float maxSpeed = 300.0f;
        float detectionRange = 400.0f;
        bool canJump = false;
        
        // Configure stats based on wolf type
        switch (type) {
            case WolfType::Basic:
                // Standard stats (already set above)
                break;
                
            case WolfType::Sniper:
                // Has gun, slower movement
                maxSpeed = 150.0f;
                chaseForce = 100.0f;
                r = 0.5f; g = 0.3f; b = 0.5f;  // Purple tint
                size = 18.0f;
                break;
                
            case WolfType::Tank:
                // High health, very slow
                maxHealth = 300;
                maxSpeed = 100.0f;
                chaseForce = 150.0f;
                r = 0.6f; g = 0.1f; b = 0.1f;  // Dark red
                size = 30.0f;
                break;
                
            case WolfType::Fast:
                // Fast movement
                maxSpeed = 500.0f;
                chaseForce = 300.0f;
                maxHealth = 60;
                r = 0.2f; g = 0.6f; b = 0.2f;  // Green tint
                size = 15.0f;
                break;
                
            case WolfType::Hunter:
                // Fast with jumping
                maxSpeed = 450.0f;
                chaseForce = 280.0f;
                maxHealth = 80;
                canJump = true;
                r = 0.3f; g = 0.4f; b = 0.6f;  // Blue tint
                size = 18.0f;
                break;
        }
        
        // Add components
        registry.addComponent(wolf, Transform3D{
            Vec3{x, 20.0f, z},
            size, size, size,
            r, g, b
        });
        registry.addComponent(wolf, Velocity3D{ Vec3{0,0,0} });
        registry.addComponent(wolf, WolfTag{});
        
        WolfComponent wolfComp;
        wolfComp.type = type;
        wolfComp.chaseForce = chaseForce;
        wolfComp.maxSpeed = maxSpeed;
        wolfComp.detectionRange = detectionRange;
        wolfComp.canJump = canJump;
        wolfComp.jumpCooldown = 0.0f;
        registry.addComponent(wolf, wolfComp);
        
        registry.addComponent(wolf, PhysicsTag{ true });
        registry.addComponent(wolf, EnemyTag{});
        registry.addComponent(wolf, Health{ maxHealth, maxHealth });
        
        // Add weapon for Sniper type
        if (type == WolfType::Sniper) {
            WeaponInventory inventory;
            Weapon gun;
            gun.type = WeaponType::MachineGun;
            gun.name = "Wolf Gun";
            gun.damage = 8.0f;
            gun.fireRate = 0.5f;
            gun.currentCooldown = 0.0f;
            gun.projectileSpeed = 400.0f;
            gun.projectileSize = 5.0f;
            gun.projectileLife = 2000.0f;
            gun.explosionRadius = 0.0f;
            gun.r = 1.0f;
            gun.g = 0.0f;
            gun.b = 0.0f;
            inventory.weapons.push_back(gun);
            registry.addComponent(wolf, inventory);
        }
    }

    void InitWolves(EntityManager& registry, float startX, float startZ, int count) {
        for (int i = 0; i < count; i++) {
            // Random starting position within a range
            float offsetX = (rand() % 300 - 150.0f);
            float offsetZ = (rand() % 300 - 150.0f);
            
            // Create basic type wolves
            InitWolfOfType(registry, startX + offsetX, startZ + offsetZ, WolfType::Basic);
        }
    }

    void Update(EntityManager& registry, float dtMs) {
        float dt = dtMs / 100.0f;
        if (dt <= 0) return;

        // Get all potential targets (players and sheep)
        std::vector<Vec3> targetPositions;
        std::vector<bool> isPlayerTarget;  // Track if target is player (true) or sheep (false)

        // Add all players as potential targets
        View<PlayerTag, Transform3D> playerView(registry);
        for (auto id : playerView) {
            targetPositions.push_back(playerView.get<Transform3D>(id).pos);
            isPlayerTarget.push_back(true);
        }

        // Add all sheep as potential targets
        View<SheepTag, Transform3D> sheepView(registry);
        for (auto id : sheepView) {
            targetPositions.push_back(sheepView.get<Transform3D>(id).pos);
            isPlayerTarget.push_back(false);
        }

        // If no targets exist, wolves just stay idle
        if (targetPositions.empty()) {
            return;
        }

        // Update each wolf
        View<WolfTag, Transform3D, Velocity3D, WolfComponent> wolfView(registry);

        // Store all wolf positions and sizes for collision detection
        std::vector<EntityID> wolfIDs;
        std::vector<Vec3> wolfPositions;
        std::vector<float> wolfSizes;
        
        for (auto id : wolfView) {
            wolfIDs.push_back(id);
            wolfPositions.push_back(wolfView.get<Transform3D>(id).pos);
            wolfSizes.push_back(wolfView.get<Transform3D>(id).width);
        }

        for (size_t idx = 0; idx < wolfIDs.size(); idx++) {
            EntityID id = wolfIDs[idx];
            auto& t = wolfView.get<Transform3D>(id);
            auto& v = wolfView.get<Velocity3D>(id);
            auto& params = wolfView.get<WolfComponent>(id);

            Vec3 pos = t.pos;
            Vec3 vel = v.vel;

            // Find nearest target
            float nearestDistSq = std::numeric_limits<float>::max();
            Vec3 nearestTarget = pos;
            bool foundTarget = false;

            for (size_t i = 0; i < targetPositions.size(); i++) {
                float dx = targetPositions[i].x - pos.x;
                float dz = targetPositions[i].z - pos.z;
                float distSq = dx * dx + dz * dz;

                if (distSq < nearestDistSq) {
                    nearestDistSq = distSq;
                    nearestTarget = targetPositions[i];
                    foundTarget = true;
                }
            }

            if (!foundTarget) {
                continue;
            }

            float nearestDist = std::sqrt(nearestDistSq);

            // Calculate chase force toward nearest target
            Vec3 chaseForce = { 0, 0, 0 };

            if (nearestDist > params.minChaseDistance) {
                // Move toward target
                float dx = nearestTarget.x - pos.x;
                float dz = nearestTarget.z - pos.z;
                chaseForce = WolfNorm({ dx, 0, dz });

                // Apply stronger force when target is farther
                float forceMagnitude = params.chaseForce;
                if (nearestDist > params.detectionRange * 0.5f) {
                    forceMagnitude *= 1.5f;  // Speed up when far away
                }

                chaseForce.x *= forceMagnitude;
                chaseForce.z *= forceMagnitude;
            }
            else {
                // Too close, slow down or circle
                chaseForce.x = vel.x * -0.5f;  // Damping force
                chaseForce.z = vel.z * -0.5f;
            }

            // Add separation force to avoid overlapping with other wolves
            Vec3 separationForce = { 0, 0, 0 };
            float mySize = wolfSizes[idx];
            
            for (size_t j = 0; j < wolfPositions.size(); j++) {
                if (idx == j) continue;  // Skip self
                
                float dx = pos.x - wolfPositions[j].x;
                float dz = pos.z - wolfPositions[j].z;
                float distSq = dx * dx + dz * dz;
                
                // Separation radius = sum of radii + small buffer
                float otherSize = wolfSizes[j];
                float minDist = (mySize + otherSize) * 0.6f;  // 0.6 to allow some overlap buffer
                float minDistSq = minDist * minDist;
                
                if (distSq < minDistSq && distSq > 0.0001f) {
                    float dist = std::sqrt(distSq);
                    // Stronger repulsion when closer
                    float repulsionStrength = (minDist - dist) / dist * 300.0f;
                    separationForce.x += (dx / dist) * repulsionStrength;
                    separationForce.z += (dz / dist) * repulsionStrength;
                }
            }

            // Apply force to velocity
            vel.x += chaseForce.x * dt;
            vel.z += chaseForce.z * dt;
            vel.x += separationForce.x * dt;
            vel.z += separationForce.z * dt;

            // Limit maximum speed
            WolfLimit(vel, params.maxSpeed);

            // Apply damping to prevent excessive speed
            vel.x *= 0.95f;
            vel.z *= 0.95f;

            // Update velocity component
            v.vel.x = vel.x;
            v.vel.z = vel.z;

            // Simple rotation toward movement direction (optional, for visual feedback)
            // Could be expanded to actually rotate the wolf model
        }
    }
}