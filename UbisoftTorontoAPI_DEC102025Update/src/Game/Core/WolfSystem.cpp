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

    void InitWolves(EntityManager& registry, float startX, float startZ, int count) {
        for (int i = 0; i < count; i++) {
            Entity wolf = registry.createEntity();

            // Random starting position within a range
            float offsetX = (rand() % 300 - 150.0f);
            float offsetZ = (rand() % 300 - 150.0f);

            registry.addComponent(wolf, Transform3D{
                Vec3{startX + offsetX, 20.0f, startZ + offsetZ},
                20.0f, 20.0f, 20.0f,  // Size (slightly bigger than sheep)
                0.4f, 0.2f, 0.1f      // Color (dark brown/gray)
                });
            registry.addComponent(wolf, Velocity3D{ Vec3{0,0,0} });
            registry.addComponent(wolf, WolfTag{});
            registry.addComponent(wolf, WolfComponent{}); // Use default parameters
            registry.addComponent(wolf, PhysicsTag{ true }); // Enable physics for ground collision
        }
    }

    void Update(EntityManager& registry, float dtMs) {
        float dt = dtMs / 1000.0f;
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

        for (auto id : wolfView) {
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

            // Apply force to velocity
            vel.x += chaseForce.x * dt;
            vel.z += chaseForce.z * dt;

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
