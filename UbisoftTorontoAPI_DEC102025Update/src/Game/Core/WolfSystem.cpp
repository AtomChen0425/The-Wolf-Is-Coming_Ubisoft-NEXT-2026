#include "WolfSystem.h"
#include "../../System/Component/Component.h"
#include "../../System/Math/Math.h"
#include "../ContestAPI/app.h"
#include <cmath>
#include <vector>
#include <limits>
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
void WolfShoot(EntityManager& registry, float dt, Vec3& targetPosition) {
    View<WolfTag, WeaponInventory, Transform3D> wolfView(registry);
    for (auto id : wolfView) {
        auto& inventory = wolfView.get<WeaponInventory>(id);
        auto& wolfPos = wolfView.get<Transform3D>(id).pos;

        Vec3 nearestTarget;
        bool foundTarget = false;

        float dx = targetPosition.x - wolfPos.x;
        float dz = targetPosition.z - wolfPos.z;
        float dy = targetPosition.y - wolfPos.y;
        Vec3 bulletDirection = Normalize3D(Vec3{ dx, dy, dz });

        size_t weaponCount = inventory.weapons.size();

        const float totalArc = 1.5f * PI;
        const float radius = 10.0f; 

        float currentYaw = std::atan2(bulletDirection.x, bulletDirection.z);
        float startAngle = currentYaw - (totalArc / 2.0f);

        float angleStep = (weaponCount > 1) ? (totalArc / (weaponCount - 1)) : 0.0f;

        for (int i = 0; i < weaponCount; ++i) {
            Weapon& weapon = inventory.weapons[i];
            weapon.currentCooldown -= dt;

            if (weapon.currentCooldown > 0.0f) continue;
            float theta = (weaponCount > 1) ? (startAngle + i * angleStep) : currentYaw;

            Vec3 circleOffset = Vec3{
                std::sin(theta) * radius,
                0.0f,
                std::cos(theta) * radius
            };

            Vec3 bulletPosition = wolfPos + circleOffset;
            //Vec3 bulletPosition = sheepPos + bulletDirection * 2.0f; // Spawn bullet slightly in front of player
            // Create bullet entity
            Entity bullet = registry.createEntity();
            registry.addComponent(bullet, Bullet{ bulletDirection ,weapon.projectileSpeed, weapon.projectileLife,weapon.damage,false,weapon.explosionRadius,weapon.projectileSize,weapon.knockback });
            registry.addComponent(bullet, Transform3D{ bulletPosition, weapon.projectileSize, weapon.projectileSize, weapon.projectileSize, weapon.r, weapon.g, weapon.b });
            registry.addComponent(bullet, Velocity3D{ bulletDirection * weapon.projectileSpeed });
            registry.addComponent(bullet, EnemyBulletTag{}); // Enemy bullet
            registry.addComponent(bullet, TrailEmitter{ 50.0f, 0.0f, 300.0f, 5.0f, 0.0f, 0.5f, 0.0f });
            weapon.currentCooldown = weapon.fireRate;
        }
    }
}
void WolfShootByID(WeaponInventory& inventory, Transform3D& wolfTransform, float dt, Vec3& targetPosition, EntityManager& registry) {
    auto& wolfPos = wolfTransform.pos;
    Vec3 nearestTarget;
    bool foundTarget = false;

    float dx = targetPosition.x - wolfPos.x;
    float dz = targetPosition.z - wolfPos.z;
    float dy = targetPosition.y - wolfPos.y;
    Vec3 bulletDirection = Normalize3D(Vec3{ dx, dy, dz });

    size_t weaponCount = inventory.weapons.size();

    const float totalArc = 1.5f * 3.14159265f;
    const float radius = 10.0f; //

    float currentYaw = std::atan2(bulletDirection.x, bulletDirection.z);
    float startAngle = currentYaw - (totalArc / 2.0f);

    float angleStep = (weaponCount > 1) ? (totalArc / (weaponCount - 1)) : 0.0f;

    for (int i = 0; i < weaponCount; ++i) {
        Weapon& weapon = inventory.weapons[i];
        weapon.currentCooldown -= dt;

        if (weapon.currentCooldown > 0.0f) continue;
        float theta = (weaponCount > 1) ? (startAngle + i * angleStep) : currentYaw;

        Vec3 circleOffset = Vec3{
            std::sin(theta) * radius,
            0.0f,
            std::cos(theta) * radius
        };

        Vec3 bulletPosition = wolfPos + circleOffset;
        //Vec3 bulletPosition = sheepPos + bulletDirection * 2.0f; // Spawn bullet slightly in front of player
        // Create bullet entity
        Entity bullet = registry.createEntity();
        registry.addComponent(bullet, Bullet{ bulletDirection ,weapon.projectileSpeed, weapon.projectileLife,weapon.damage,false,weapon.explosionRadius,weapon.projectileSize,weapon.knockback });
        registry.addComponent(bullet, Transform3D{ bulletPosition, weapon.projectileSize, weapon.projectileSize, weapon.projectileSize, weapon.r, weapon.g, weapon.b });
        registry.addComponent(bullet, Velocity3D{ bulletDirection * weapon.projectileSpeed });
        registry.addComponent(bullet, EnemyBulletTag{}); // Enemy bullet
        if (weapon.type == WeaponType::MagicWand) {
            registry.addComponent(bullet, MagicTag{});
        }

        registry.addComponent(bullet, TrailEmitter{ 50.0f, 0.0f, 300.0f, 5.0f, 0.0f, 0.5f, 0.0f });


        weapon.currentCooldown = weapon.fireRate;
    }
}

namespace WolfSystem {

    void InitWolfOfType(EntityManager& registry, float x, float z, WolfType type, const GameConfig& config) {
        Entity wolf = registry.createEntity();

        // Base stats that will be modified by type
        float size = config.wolfBasicSize;
        float r = 0.4f, g = 0.2f, b = 0.1f;  // Color
        int maxHealth = config.wolfBasicHealth;
        float chaseForce = config.wolfBasicChaseForce;
        float maxSpeed = config.wolfBasicSpeed;
        float detectionRange = config.wolfBasicDetectionRange;
        bool canJump = false;
        CSimpleSprite* pSprite = nullptr;
        const float animationSpeed = 1.0f / 10.0f;
        // Configure stats based on wolf type
        switch (type) {
        case WolfType::Basic:
            // Standard stats (already set above from config)
            pSprite = App::CreateSprite("data/TestData/Wolf5.png", 2, 1);

            pSprite->CreateAnimation(ANIM_BACKWARDS, animationSpeed, { 0,1 });
            pSprite->CreateAnimation(ANIM_LEFT, animationSpeed, { 0,1 });
            pSprite->CreateAnimation(ANIM_RIGHT, animationSpeed, { 0,1 });
            pSprite->CreateAnimation(ANIM_FORWARDS, animationSpeed, { 0,1 });
            pSprite->SetScale(0.3f);
            break;

        case WolfType::Sniper:
            // Has gun, slower movement
            maxSpeed = config.wolfSniperSpeed;
            chaseForce = config.wolfSniperChaseForce;
            maxHealth = config.wolfSniperHealth;
            detectionRange = config.wolfSniperDetectionRange;
            r = 0.5f; g = 0.3f; b = 0.5f;  // Purple tint
            size = config.wolfSniperSize;
            pSprite = App::CreateSprite("data/TestData/Wolf4.png", 2, 1);
            pSprite->CreateAnimation(ANIM_BACKWARDS, animationSpeed, { 0,1 });
            pSprite->CreateAnimation(ANIM_LEFT, animationSpeed, { 0,1 });
            pSprite->CreateAnimation(ANIM_RIGHT, animationSpeed, { 0,1 });
            pSprite->CreateAnimation(ANIM_FORWARDS, animationSpeed, { 0,1 });
            pSprite->SetScale(0.3f);
            break;
        case WolfType::Magic:
            // Has Magic wand, slower movement
            maxSpeed = config.wolfMagicSpeed;
            chaseForce = config.wolfMagicChaseForce;
            maxHealth = config.wolfMagicHealth;
            detectionRange = config.wolfMagicDetectionRange;
            r = 0.9f; g = 0.3f; b = 0.5f;
            size = config.wolfMagicSize;
            pSprite = App::CreateSprite("data/TestData/WolfMagic.png", 2, 1);
            pSprite->CreateAnimation(ANIM_BACKWARDS, animationSpeed, { 0,1 });
            pSprite->CreateAnimation(ANIM_LEFT, animationSpeed, { 0,1 });
            pSprite->CreateAnimation(ANIM_RIGHT, animationSpeed, { 0,1 });
            pSprite->CreateAnimation(ANIM_FORWARDS, animationSpeed, { 0,1 });
            pSprite->SetScale(0.3f);
            break;
        case WolfType::Tank:
            // High health, very slow
            maxHealth = config.wolfTankHealth;
            maxSpeed = config.wolfTankSpeed;
            chaseForce = config.wolfTankChaseForce;
            detectionRange = config.wolfTankDetectionRange;
            r = 0.6f; g = 0.1f; b = 0.1f;  // Dark red
            size = config.wolfTankSize;
            pSprite = App::CreateSprite("data/TestData/WolfBig.png", 2, 1);
            pSprite->CreateAnimation(ANIM_BACKWARDS, animationSpeed, { 0,1 });
            pSprite->CreateAnimation(ANIM_LEFT, animationSpeed, { 0,1 });
            pSprite->CreateAnimation(ANIM_RIGHT, animationSpeed, { 0,1 });
            pSprite->CreateAnimation(ANIM_FORWARDS, animationSpeed, { 0,1 });
            pSprite->SetScale(0.3f);
            break;

        case WolfType::Fast:
            // Fast movement
            maxSpeed = config.wolfFastSpeed;
            chaseForce = config.wolfFastChaseForce;
            maxHealth = config.wolfFastHealth;
            detectionRange = config.wolfFastDetectionRange;
            r = 0.2f; g = 0.6f; b = 0.2f;  // Green tint
            size = config.wolfFastSize;
            pSprite = App::CreateSprite("data/TestData/Wolf3.png", 2, 1);
            pSprite->CreateAnimation(ANIM_BACKWARDS, animationSpeed, { 0,1 });
            pSprite->CreateAnimation(ANIM_LEFT, animationSpeed, { 0,1 });
            pSprite->CreateAnimation(ANIM_RIGHT, animationSpeed, { 0,1 });
            pSprite->CreateAnimation(ANIM_FORWARDS, animationSpeed, { 0,1 });
            pSprite->SetScale(0.3f);
            break;

        case WolfType::Hunter:
            // Fast with jumping
            maxSpeed = config.wolfHunterSpeed;
            chaseForce = config.wolfHunterChaseForce;
            maxHealth = config.wolfHunterHealth;
            detectionRange = config.wolfHunterDetectionRange;
            canJump = true;
            r = 0.3f; g = 0.4f; b = 0.6f;  // Blue tint
            size = config.wolfHunterSize;
            pSprite = App::CreateSprite("data/TestData/Wolf2.png", 2, 1);
            pSprite->CreateAnimation(ANIM_BACKWARDS, animationSpeed, { 0,1 });
            pSprite->CreateAnimation(ANIM_LEFT, animationSpeed, { 0,1 });
            pSprite->CreateAnimation(ANIM_RIGHT, animationSpeed, { 0,1 });
            pSprite->CreateAnimation(ANIM_FORWARDS, animationSpeed, { 0,1 });
            pSprite->SetScale(0.3f);
            break;
        }
        size *= 1.5f;
        registry.addComponent(wolf, SpriteComponent{ pSprite, 0 });
        // Add components
        registry.addComponent(wolf, Transform3D{
            Vec3{x, config.wolfSpawnYPosition, z},
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
        registry.addComponent(wolf, AnimalTag{});
        // Add weapon for Sniper type
        if (type == WolfType::Sniper) {
            WeaponInventory inventory;
            Weapon gun;
            gun.type = WeaponType::MachineGun;
            gun.name = "Wolf Gun";
            gun.damage = config.wolfSniperGunDamage;
            gun.fireRate = config.wolfSniperGunFireRate;
            gun.currentCooldown = 0.0f;
            gun.projectileSpeed = config.wolfSniperGunProjectileSpeed;
            gun.projectileSize = config.wolfSniperGunProjectileSize;
            gun.projectileLife = config.wolfSniperGunProjectileLife;
            gun.explosionRadius = 0.0f;
            gun.knockback = config.wolfSniperGunKnockback;
            gun.r = 1.0f;
            gun.g = 0.0f;
            gun.b = 0.0f;
            inventory.weapons.push_back(gun);
            registry.addComponent(wolf, inventory);
        }
        if (type == WolfType::Magic) {
            WeaponInventory inventory;
            Weapon wand;
            wand.type = WeaponType::MagicWand;
            wand.name = "Magic Wand";
            wand.damage = config.wolfMagicWandDamage;
            wand.fireRate = config.wolfMagicWandFireRate;
            wand.currentCooldown = 0.0f;
            wand.projectileSpeed = config.wolfMagicWandProjectileSpeed;
            wand.projectileSize = config.wolfMagicWandProjectileSize;
            wand.projectileLife = config.wolfMagicWandProjectileLife;
            wand.explosionRadius = 0.0f;
            wand.knockback = 10.0f;
            wand.r = 0.7f;
            wand.g = 0.0f;
            wand.b = 0.5f;
            inventory.weapons.push_back(wand);
            registry.addComponent(wolf, inventory);
        }
    }

    void InitWolves(EntityManager& registry, float startX, float startZ, int count, const GameConfig& config) {
        for (int i = 0; i < count; i++) {
            // Random starting position within a range
            float offsetX = (rand() % 300 - 150.0f);
            float offsetZ = (rand() % 300 - 150.0f);

            // Create basic type wolves
            InitWolfOfType(registry, startX + offsetX, startZ + offsetZ, WolfType::Basic, config);
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
            //WolfShoot(registry, dt, targetPositions[0]);
            isPlayerTarget.push_back(true);
            break; // Only target the first player found
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

            // Update jump cooldown
            if (params.jumpCooldown > 0.0f) {
                params.jumpCooldown -= dt;
            }
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
            if (wolfView.has<WeaponInventory>(id)) {
                // Only chase if within detection range
                WolfShootByID(wolfView.get<WeaponInventory>(id), wolfView.get<Transform3D>(id), dt, nearestTarget, registry);
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

            // Hunter wolf jumping logic
            if (params.canJump && params.jumpCooldown <= 0.0f) {
                // Check if wolf is on ground (y position close to ground level)
                bool isOnGround = (pos.y <= 25.0f);  // Ground level + small tolerance

                if (isOnGround && foundTarget) {
                    // Jump conditions:
                    // 1. Target is at medium distance (not too close, not too far)
                    // 2. Moving toward target (chasing)
                    float jumpMinDist = 80.0f;   // Don't jump if too close
                    float jumpMaxDist = 2000.0f;  // Don't jump if too far

                    if (nearestDist >= jumpMinDist && nearestDist <= jumpMaxDist) {
                        // Apply upward velocity for jump
                        vel.y = 450.0f;  // Jump velocity (matches player-like jump)

                        // Also add forward momentum in chase direction
                        Vec3 jumpDir = WolfNorm({ nearestTarget.x - pos.x, 0, nearestTarget.z - pos.z });
                        vel.x += jumpDir.x * 200.0f;  // Extra forward boost during jump
                        vel.z += jumpDir.z * 200.0f;

                        // Set cooldown (3 seconds = 3 * 100 / 100 = 3.0)
                        params.jumpCooldown = 3.0f;
                    }
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
            v.vel.y = vel.y;
            v.vel.z = vel.z;

            // Simple rotation toward movement direction (optional, for visual feedback)
            // Could be expanded to actually rotate the wolf model
        }
    }
}