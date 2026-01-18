#include "GameConfig.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <functional>

void GameConfig::LoadDefaults() {
    // Player physics
    gravity = -980.0f;
    jumpVelocity = 400.0f;
    forwardSpeed = 200.0f;
    strafeSpeed = 300.0f;
    acceleration = 2000.0f;
    damping = 8.0f;
    rotationSpeed = 2.0f;

    // Camera controls
    mouseSensitivity = 0.003f;
    maxPitch = 1.4f;
    minPitch = -1.4f;

    // Map generation
    blockSize = 100.0f;
    renderDistance = 1000.0f;
    deleteDistance = 1000.0f;
    roadWidth = 5;
    wallHeight = 100.0f;

    // Map generation probabilities
    normalFloorChance = 0.6f;
    obstacleChance = 0.15f;
    scorePointChance = 0.15f;
    upgradePointChance = 0.05f;
    gapChance = 0.10f;
    gapFloorChance = 0.3f;

    // Chunk-based generation
    chunkSize = 5;
    chunkRenderRadius = 3;

    // Block heights
    floorHeight = 10.0f;
    tallBlockHeight = 60.0f;
    scorePointHeight = 30.0f;

    // Bullet physics
    bulletSpeed = 600.0f;

    // Player spawn
    playerSpawnX = 0.0f;
    playerSpawnY = 0.0f;
    playerSpawnZ = 0.0f;

    // Rendering
    fov = 600.0f;
    nearZ = 1.0f;
    farZ = 3000.0f;

    // Colors
    playerColorR = 1.0f;
    playerColorG = 0.0f;
    playerColorB = 0.0f;

    bulletColorR = 0.5f;
    bulletColorG = 0.0f;
    bulletColorB = 0.5f;

    floorColor1R = 0.2f;
    floorColor1G = 0.6f;
    floorColor1B = 0.2f;
    floorColor2R = 0.3f;
    floorColor2G = 0.8f;
    floorColor2B = 0.3f;

    wallColorR = 0.6f;
    wallColorG = 0.3f;
    wallColorB = 0.1f;

    tallBlockColorR = 0.1f;
    tallBlockColorG = 0.3f;
    tallBlockColorB = 0.9f;

    scorePointColorR = 1.0f;
    scorePointColorG = 1.0f;
    scorePointColorB = 0.0f;

    upgradePointColorR = 1.0f;
    upgradePointColorG = 0.5f;
    upgradePointColorB = 0.0f;

    // Upgrade values
    healthUpgradeAmount = 20.0f;
    speedUpgradeAmount = 50.0f;
    jumpUpgradeAmount = 100.0f;
    gravityUpgradeAmount = -100.0f;
    bulletSpeedUpgradeAmount = 100.0f;

    // Level/Round system parameters
    roundDurationMs = 10000.0f;           // Duration of each round (60 seconds)
    baseWolfSpawnIntervalMs = 1000.0f;    // Initial wolf spawn interval (1 second)
    wolfSpawnReductionPerRound = 200.0f;  // Spawn interval reduction per round
    minWolfSpawnIntervalMs = 1000.0f;     // Minimum wolf spawn interval (1 second)
    sheepAddedPerUpgrade = 10;              // Number of sheep added with "Add Sheep" upgrade
    sheepSpawnOffsetZ = 100.0f;           // Z offset for spawning new sheep

    // Wolf type spawn intervals (different wolves spawn at different rates)
    sniperWolfSpawnIntervalMs = 8000.0f;   // Sniper wolves spawn every 8 seconds
    tankWolfSpawnIntervalMs = 12000.0f;    // Tank wolves spawn every 12 seconds
    fastWolfSpawnIntervalMs = 5000.0f;     // Fast wolves spawn every 5 seconds
    hunterWolfSpawnIntervalMs = 10000.0f;  // Hunter wolves spawn every 10 seconds

    // Wolf stats - Basic
    wolfBasicHealth = 100.0f;
    wolfBasicSpeed = 300.0f;
    wolfBasicChaseForce = 500.0f;
    wolfBasicDetectionRange = 300.0f;
    wolfBasicSize = 20.0f;

    // Wolf stats - Sniper
    wolfSniperHealth = 100.0f;
    wolfSniperSpeed = 150.0f;
    wolfSniperChaseForce = 300.0f;
    wolfSniperDetectionRange = 400.0f;
    wolfSniperSize = 20.0f;

    // Wolf stats - Tank
    wolfTankHealth = 300.0f;
    wolfTankSpeed = 100.0f;
    wolfTankChaseForce = 400.0f;
    wolfTankDetectionRange = 250.0f;
    wolfTankSize = 30.0f;

    // Wolf stats - Fast
    wolfFastHealth = 60.0f;
    wolfFastSpeed = 500.0f;
    wolfFastChaseForce = 600.0f;
    wolfFastDetectionRange = 350.0f;
    wolfFastSize = 15.0f;

    // Wolf stats - Hunter
    wolfHunterHealth = 80.0f;
    wolfHunterSpeed = 450.0f;
    wolfHunterChaseForce = 550.0f;
    wolfHunterDetectionRange = 300.0f;
    wolfHunterSize = 20.0f;
    wolfHunterJumpCooldown = 3.0f;
    wolfHunterJumpVelocity = 350.0f;
    wolfHunterJumpBoost = 200.0f;
    wolfHunterJumpMinDistance = 80.0f;
    wolfHunterJumpMaxDistance = 200.0f;

    // Wolf weapon - Sniper gun
    wolfSniperGunDamage = 8.0f;
    wolfSniperGunFireRate = 500.0f;
    wolfSniperGunProjectileSpeed = 600.0f;
    wolfSniperGunProjectileSize = 5.0f;
    wolfSniperGunProjectileLife = 2000.0f;
    wolfSniperGunKnockback = 150.0f;

    // Player weapon - Machine Gun
    playerMachineGunDamage = 10.0f;
    playerMachineGunFireRate = 50.0f;
    playerMachineGunProjectileSpeed = 500.0f;
    playerMachineGunProjectileSize = 5.0f;
    playerMachineGunProjectileLife = 2000.0f;
    playerMachineGunExplosionRadius = 0.0f;
    playerMachineGunKnockback = 100.0f;
    playerMachineGunColorR = 1.0f;
    playerMachineGunColorG = 1.0f;
    playerMachineGunColorB = 0.0f;

    // Player weapon - Cannon
    playerCannonDamage = 50.0f;
    playerCannonFireRate = 1000.0f;
    playerCannonProjectileSpeed = 300.0f;
    playerCannonProjectileSize = 15.0f;
    playerCannonProjectileLife = 3000.0f;
    playerCannonExplosionRadius = 50.0f;
    playerCannonKnockback = 500.0f;
    playerCannonColorR = 1.0f;
    playerCannonColorG = 0.5f;
    playerCannonColorB = 0.0f;

    // Sheep weapon - Machine Gun
    sheepMachineGunDamage = 5.0f;
    sheepMachineGunFireRate = 50.0f;
    sheepMachineGunProjectileSpeed = 400.0f;
    sheepMachineGunProjectileSize = 3.0f;
    sheepMachineGunProjectileLife = 2000.0f;
    sheepMachineGunExplosionRadius = 0.0f;
    sheepMachineGunKnockback = 50.0f;
    sheepMachineGunColorR = 0.5f;
    sheepMachineGunColorG = 1.0f;
    sheepMachineGunColorB = 0.5f;

    // Sheep weapon - Cannon
    sheepCannonDamage = 30.0f;
    sheepCannonFireRate = 1000.0f;
    sheepCannonProjectileSpeed = 250.0f;
    sheepCannonProjectileSize = 10.0f;
    sheepCannonProjectileLife = 3000.0f;
    sheepCannonExplosionRadius = 50.0f;
    sheepCannonKnockback = 300.0f;
    sheepCannonColorR = 0.5f;
    sheepCannonColorG = 1.0f;
    sheepCannonColorB = 1.0f;

    // Wolf spawn settings
    wolfSpawnRadius = 1700.0f;
    wolfSpawnYPosition = 20.0f;
    wolfSpawnMinX = -200.0f;
    wolfSpawnMaxX = 200.0f;
    wolfSpawnMinZ = -50.0f;
    wolfSpawnMaxZ = -30.0f;

    // Sheep spawn settings
    sheepSpawnRadius = 100.0f;
    sheepInitialCount = 10;
    sheepSpawnYPosition = 30.0f;
    sheepMoveSpeed = 50.0f;
    sheepSpawnOffsetMin = -100.0f;
    sheepSpawnOffsetMax = 100.0f;
    sheepSize = 15.0f;
    sheepColorR = 0.9f;
    sheepColorG = 0.9f;
    sheepColorB = 0.9f;
    sheepSpriteScale = 0.3f;
    sheepAnimationSpeed = 1.0f / 15.0f;

    // Sheep Boids behavior
    sheepViewRadius = 70.0f;
    sheepMaxSpeed = 60.0f;
    sheepMaxForce = 0.6f;
    sheepSeparationWeight = 1.5f;
    sheepAlignmentWeight = 1.0f;
    sheepCohesionWeight = 1.0f;
    sheepTargetWeight = 0.5f;
    sheepFearWeight = 4.0f;
    sheepEnemyDetectRange = 150.0f;

    // Sheep shooting
    sheepWeaponArcAngle = 1.5f * 3.14159265f;
    sheepWeaponSpawnRadius = 10.0f;

    // Sheep targeting
    sheepTargetFarDistance = 60.0f;
    sheepTargetNearDistance = 30.0f;

    // Sheep physics
    sheepVelocityDamping = 0.98f;
    sheepSeparationSizeMultiplier = 0.55f;
    sheepSeparationRepulsionStrength = 2.0f;

    // Optimization
    spatialGridCellSize = 80.0f;

    // Enemy AI
    maxBulletDistance = 5000.0f;

    // Particle trails
    sheepBulletTrailEmissionRate = 50.0f;
    sheepBulletTrailMinSize = 0.0f;
    sheepBulletTrailMaxSize = 150.0f;
    sheepBulletTrailLife = 3.0f;
    sheepBulletTrailColorR = 1.0f;
    sheepBulletTrailColorG = 0.5f;
    sheepBulletTrailColorB = 0.0f;
}

bool GameConfig::LoadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Config file not found: " << filename << ". Using defaults." << std::endl;
        LoadDefaults();
        return false;
    }

    auto setFloat = [&](float& field, std::string_view v) { field = std::stof(std::string(v)); };
    auto setInt = [&](int& field, std::string_view v) { field = std::stoi(std::string(v)); };

    const std::unordered_map<std::string, std::function<void(std::string_view)>> setters = {
        // Player physics
        {"gravity", [&](std::string_view v) { setFloat(gravity, v); }},
        {"jumpVelocity", [&](std::string_view v) { setFloat(jumpVelocity, v); }},
        {"forwardSpeed", [&](std::string_view v) { setFloat(forwardSpeed, v); }},
        {"strafeSpeed", [&](std::string_view v) { setFloat(strafeSpeed, v); }},
        {"acceleration", [&](std::string_view v) { setFloat(acceleration, v); }},
        {"damping", [&](std::string_view v) { setFloat(damping, v); }},
        {"rotationSpeed", [&](std::string_view v) { setFloat(rotationSpeed, v); }},

        // Camera
        {"mouseSensitivity", [&](std::string_view v) { setFloat(mouseSensitivity, v); }},
        {"maxPitch", [&](std::string_view v) { setFloat(maxPitch, v); }},
        {"minPitch", [&](std::string_view v) { setFloat(minPitch, v); }},

        // Map generation
        {"blockSize", [&](std::string_view v) { setFloat(blockSize, v); }},
        {"renderDistance", [&](std::string_view v) { setFloat(renderDistance, v); }},
        {"deleteDistance", [&](std::string_view v) { setFloat(deleteDistance, v); }},
        {"roadWidth", [&](std::string_view v) { setInt(roadWidth, v); }},
        {"wallHeight", [&](std::string_view v) { setFloat(wallHeight, v); }},

        // Probabilities
        {"normalFloorChance", [&](std::string_view v) { setFloat(normalFloorChance, v); }},
        {"obstacleChance", [&](std::string_view v) { setFloat(obstacleChance, v); }},
        {"scorePointChance", [&](std::string_view v) { setFloat(scorePointChance, v); }},
        {"upgradePointChance", [&](std::string_view v) { setFloat(upgradePointChance, v); }},
        {"gapChance", [&](std::string_view v) { setFloat(gapChance, v); }},
        {"gapFloorChance", [&](std::string_view v) { setFloat(gapFloorChance, v); }},

        // Chunk
        {"chunkSize", [&](std::string_view v) { setInt(chunkSize, v); }},
        {"chunkRenderRadius", [&](std::string_view v) { setInt(chunkRenderRadius, v); }},

        // Heights / bullet
        {"floorHeight", [&](std::string_view v) { setFloat(floorHeight, v); }},
        {"tallBlockHeight", [&](std::string_view v) { setFloat(tallBlockHeight, v); }},
        {"scorePointHeight", [&](std::string_view v) { setFloat(scorePointHeight, v); }},
        {"bulletSpeed", [&](std::string_view v) { setFloat(bulletSpeed, v); }},

        // Spawn / rendering
        {"playerSpawnX", [&](std::string_view v) { setFloat(playerSpawnX, v); }},
        {"playerSpawnY", [&](std::string_view v) { setFloat(playerSpawnY, v); }},
        {"playerSpawnZ", [&](std::string_view v) { setFloat(playerSpawnZ, v); }},
        {"fov", [&](std::string_view v) { setFloat(fov, v); }},
        {"nearZ", [&](std::string_view v) { setFloat(nearZ, v); }},
        {"farZ", [&](std::string_view v) { setFloat(farZ, v); }},

        // Colors
        {"playerColorR", [&](std::string_view v) { setFloat(playerColorR, v); }},
        {"playerColorG", [&](std::string_view v) { setFloat(playerColorG, v); }},
        {"playerColorB", [&](std::string_view v) { setFloat(playerColorB, v); }},
        {"bulletColorR", [&](std::string_view v) { setFloat(bulletColorR, v); }},
        {"bulletColorG", [&](std::string_view v) { setFloat(bulletColorG, v); }},
        {"bulletColorB", [&](std::string_view v) { setFloat(bulletColorB, v); }},
        {"floorColor1R", [&](std::string_view v) { setFloat(floorColor1R, v); }},
        {"floorColor1G", [&](std::string_view v) { setFloat(floorColor1G, v); }},
        {"floorColor1B", [&](std::string_view v) { setFloat(floorColor1B, v); }},
        {"floorColor2R", [&](std::string_view v) { setFloat(floorColor2R, v); }},
        {"floorColor2G", [&](std::string_view v) { setFloat(floorColor2G, v); }},
        {"floorColor2B", [&](std::string_view v) { setFloat(floorColor2B, v); }},
        {"wallColorR", [&](std::string_view v) { setFloat(wallColorR, v); }},
        {"wallColorG", [&](std::string_view v) { setFloat(wallColorG, v); }},
        {"wallColorB", [&](std::string_view v) { setFloat(wallColorB, v); }},
        {"tallBlockColorR", [&](std::string_view v) { setFloat(tallBlockColorR, v); }},
        {"tallBlockColorG", [&](std::string_view v) { setFloat(tallBlockColorG, v); }},
        {"tallBlockColorB", [&](std::string_view v) { setFloat(tallBlockColorB, v); }},
        {"scorePointColorR", [&](std::string_view v) { setFloat(scorePointColorR, v); }},
        {"scorePointColorG", [&](std::string_view v) { setFloat(scorePointColorG, v); }},
        {"scorePointColorB", [&](std::string_view v) { setFloat(scorePointColorB, v); }},
        {"upgradePointColorR", [&](std::string_view v) { setFloat(upgradePointColorR, v); }},
        {"upgradePointColorG", [&](std::string_view v) { setFloat(upgradePointColorG, v); }},
        {"upgradePointColorB", [&](std::string_view v) { setFloat(upgradePointColorB, v); }},

        // Upgrades
        {"healthUpgradeAmount", [&](std::string_view v) { setFloat(healthUpgradeAmount, v); }},
        {"speedUpgradeAmount", [&](std::string_view v) { setFloat(speedUpgradeAmount, v); }},
        {"jumpUpgradeAmount", [&](std::string_view v) { setFloat(jumpUpgradeAmount, v); }},
        {"gravityUpgradeAmount", [&](std::string_view v) { setFloat(gravityUpgradeAmount, v); }},
        {"bulletSpeedUpgradeAmount", [&](std::string_view v) { setFloat(bulletSpeedUpgradeAmount, v); }},

        // Rounds / spawn pacing
        {"roundDurationMs", [&](std::string_view v) { setFloat(roundDurationMs, v); }},
        {"baseWolfSpawnIntervalMs", [&](std::string_view v) { setFloat(baseWolfSpawnIntervalMs, v); }},
        {"wolfSpawnReductionPerRound", [&](std::string_view v) { setFloat(wolfSpawnReductionPerRound, v); }},
        {"minWolfSpawnIntervalMs", [&](std::string_view v) { setFloat(minWolfSpawnIntervalMs, v); }},
        {"sheepAddedPerUpgrade", [&](std::string_view v) { setInt(sheepAddedPerUpgrade, v); }},
        {"sheepSpawnOffsetZ", [&](std::string_view v) { setFloat(sheepSpawnOffsetZ, v); }},

        // Wolf stats
        {"wolfBasicHealth", [&](std::string_view v) { setInt(wolfBasicHealth, v); }},
        {"wolfBasicSpeed", [&](std::string_view v) { setFloat(wolfBasicSpeed, v); }},
        {"wolfBasicChaseForce", [&](std::string_view v) { setFloat(wolfBasicChaseForce, v); }},
        {"wolfBasicDetectionRange", [&](std::string_view v) { setFloat(wolfBasicDetectionRange, v); }},
        {"wolfBasicSize", [&](std::string_view v) { setFloat(wolfBasicSize, v); }},

        {"wolfSniperHealth", [&](std::string_view v) { setInt(wolfSniperHealth, v); }},
        {"wolfSniperSpeed", [&](std::string_view v) { setFloat(wolfSniperSpeed, v); }},
        {"wolfSniperChaseForce", [&](std::string_view v) { setFloat(wolfSniperChaseForce, v); }},
        {"wolfSniperDetectionRange", [&](std::string_view v) { setFloat(wolfSniperDetectionRange, v); }},
        {"wolfSniperSize", [&](std::string_view v) { setFloat(wolfSniperSize, v); }},

        {"wolfTankHealth", [&](std::string_view v) { setInt(wolfTankHealth, v); }},
        {"wolfTankSpeed", [&](std::string_view v) { setFloat(wolfTankSpeed, v); }},
        {"wolfTankChaseForce", [&](std::string_view v) { setFloat(wolfTankChaseForce, v); }},
        {"wolfTankDetectionRange", [&](std::string_view v) { setFloat(wolfTankDetectionRange, v); }},
        {"wolfTankSize", [&](std::string_view v) { setFloat(wolfTankSize, v); }},

        {"wolfFastHealth", [&](std::string_view v) { setInt(wolfFastHealth, v); }},
        {"wolfFastSpeed", [&](std::string_view v) { setFloat(wolfFastSpeed, v); }},
        {"wolfFastChaseForce", [&](std::string_view v) { setFloat(wolfFastChaseForce, v); }},
        {"wolfFastDetectionRange", [&](std::string_view v) { setFloat(wolfFastDetectionRange, v); }},
        {"wolfFastSize", [&](std::string_view v) { setFloat(wolfFastSize, v); }},

        {"wolfHunterHealth", [&](std::string_view v) { setInt(wolfHunterHealth, v); }},
        {"wolfHunterSpeed", [&](std::string_view v) { setFloat(wolfHunterSpeed, v); }},
        {"wolfHunterChaseForce", [&](std::string_view v) { setFloat(wolfHunterChaseForce, v); }},
        {"wolfHunterDetectionRange", [&](std::string_view v) { setFloat(wolfHunterDetectionRange, v); }},
        {"wolfHunterSize", [&](std::string_view v) { setFloat(wolfHunterSize, v); }},
        {"wolfHunterJumpCooldown", [&](std::string_view v) { setFloat(wolfHunterJumpCooldown, v); }},
        {"wolfHunterJumpVelocity", [&](std::string_view v) { setFloat(wolfHunterJumpVelocity, v); }},
        {"wolfHunterJumpBoost", [&](std::string_view v) { setFloat(wolfHunterJumpBoost, v); }},
        {"wolfHunterJumpMinDistance", [&](std::string_view v) { setFloat(wolfHunterJumpMinDistance, v); }},
        {"wolfHunterJumpMaxDistance", [&](std::string_view v) { setFloat(wolfHunterJumpMaxDistance, v); }},

        // Wolf sniper gun
        {"wolfSniperGunDamage", [&](std::string_view v) { setFloat(wolfSniperGunDamage, v); }},
        {"wolfSniperGunFireRate", [&](std::string_view v) { setFloat(wolfSniperGunFireRate, v); }},
        {"wolfSniperGunProjectileSpeed", [&](std::string_view v) { setFloat(wolfSniperGunProjectileSpeed, v); }},
        {"wolfSniperGunProjectileSize", [&](std::string_view v) { setFloat(wolfSniperGunProjectileSize, v); }},
        {"wolfSniperGunProjectileLife", [&](std::string_view v) { setFloat(wolfSniperGunProjectileLife, v); }},
        {"wolfSniperGunKnockback", [&](std::string_view v) { setFloat(wolfSniperGunKnockback, v); }},

        // Player weapons
        {"playerMachineGunDamage", [&](std::string_view v) { setFloat(playerMachineGunDamage, v); }},
        {"playerMachineGunFireRate", [&](std::string_view v) { setFloat(playerMachineGunFireRate, v); }},
        {"playerMachineGunProjectileSpeed", [&](std::string_view v) { setFloat(playerMachineGunProjectileSpeed, v); }},
        {"playerMachineGunProjectileSize", [&](std::string_view v) { setFloat(playerMachineGunProjectileSize, v); }},
        {"playerMachineGunProjectileLife", [&](std::string_view v) { setFloat(playerMachineGunProjectileLife, v); }},
        {"playerMachineGunExplosionRadius", [&](std::string_view v) { setFloat(playerMachineGunExplosionRadius, v); }},
        {"playerMachineGunKnockback", [&](std::string_view v) { setFloat(playerMachineGunKnockback, v); }},
        {"playerMachineGunColorR", [&](std::string_view v) { setFloat(playerMachineGunColorR, v); }},
        {"playerMachineGunColorG", [&](std::string_view v) { setFloat(playerMachineGunColorG, v); }},
        {"playerMachineGunColorB", [&](std::string_view v) { setFloat(playerMachineGunColorB, v); }},

        {"playerCannonDamage", [&](std::string_view v) { setFloat(playerCannonDamage, v); }},
        {"playerCannonFireRate", [&](std::string_view v) { setFloat(playerCannonFireRate, v); }},
        {"playerCannonProjectileSpeed", [&](std::string_view v) { setFloat(playerCannonProjectileSpeed, v); }},
        {"playerCannonProjectileSize", [&](std::string_view v) { setFloat(playerCannonProjectileSize, v); }},
        {"playerCannonProjectileLife", [&](std::string_view v) { setFloat(playerCannonProjectileLife, v); }},
        {"playerCannonExplosionRadius", [&](std::string_view v) { setFloat(playerCannonExplosionRadius, v); }}, // fixed
        {"playerCannonKnockback", [&](std::string_view v) { setFloat(playerCannonKnockback, v); }},
        {"playerCannonColorR", [&](std::string_view v) { setFloat(playerCannonColorR, v); }},
        {"playerCannonColorG", [&](std::string_view v) { setFloat(playerCannonColorG, v); }},
        {"playerCannonColorB", [&](std::string_view v) { setFloat(playerCannonColorB, v); }},

        // Sheep weapons
        {"sheepMachineGunDamage", [&](std::string_view v) { setFloat(sheepMachineGunDamage, v); }},
        {"sheepMachineGunFireRate", [&](std::string_view v) { setFloat(sheepMachineGunFireRate, v); }},
        {"sheepMachineGunProjectileSpeed", [&](std::string_view v) { setFloat(sheepMachineGunProjectileSpeed, v); }},
        {"sheepMachineGunProjectileSize", [&](std::string_view v) { setFloat(sheepMachineGunProjectileSize, v); }},
        {"sheepMachineGunProjectileLife", [&](std::string_view v) { setFloat(sheepMachineGunProjectileLife, v); }},
        {"sheepMachineGunExplosionRadius", [&](std::string_view v) { setFloat(sheepMachineGunExplosionRadius, v); }},
        {"sheepMachineGunKnockback", [&](std::string_view v) { setFloat(sheepMachineGunKnockback, v); }},
        {"sheepMachineGunColorR", [&](std::string_view v) { setFloat(sheepMachineGunColorR, v); }},
        {"sheepMachineGunColorG", [&](std::string_view v) { setFloat(sheepMachineGunColorG, v); }},
        {"sheepMachineGunColorB", [&](std::string_view v) { setFloat(sheepMachineGunColorB, v); }},

        {"sheepCannonDamage", [&](std::string_view v) { setFloat(sheepCannonDamage, v); }},
        {"sheepCannonFireRate", [&](std::string_view v) { setFloat(sheepCannonFireRate, v); }},
        {"sheepCannonProjectileSpeed", [&](std::string_view v) { setFloat(sheepCannonProjectileSpeed, v); }},
        {"sheepCannonProjectileSize", [&](std::string_view v) { setFloat(sheepCannonProjectileSize, v); }},
        {"sheepCannonProjectileLife", [&](std::string_view v) { setFloat(sheepCannonProjectileLife, v); }},
        {"sheepCannonExplosionRadius", [&](std::string_view v) { setFloat(sheepCannonExplosionRadius, v); }},
        {"sheepCannonKnockback", [&](std::string_view v) { setFloat(sheepCannonKnockback, v); }},
        {"sheepCannonColorR", [&](std::string_view v) { setFloat(sheepCannonColorR, v); }},
        {"sheepCannonColorG", [&](std::string_view v) { setFloat(sheepCannonColorG, v); }},
        {"sheepCannonColorB", [&](std::string_view v) { setFloat(sheepCannonColorB, v); }},

        // Spawn settings
        {"wolfSpawnRadius", [&](std::string_view v) { setFloat(wolfSpawnRadius, v); }},
        {"wolfSpawnYPosition", [&](std::string_view v) { setFloat(wolfSpawnYPosition, v); }},
        {"wolfSpawnMinX", [&](std::string_view v) { setFloat(wolfSpawnMinX, v); }},
        {"wolfSpawnMaxX", [&](std::string_view v) { setFloat(wolfSpawnMaxX, v); }},
        {"wolfSpawnMinZ", [&](std::string_view v) { setFloat(wolfSpawnMinZ, v); }},
        {"wolfSpawnMaxZ", [&](std::string_view v) { setFloat(wolfSpawnMaxZ, v); }},

        {"sheepSpawnRadius", [&](std::string_view v) { setFloat(sheepSpawnRadius, v); }},
        {"sheepInitialCount", [&](std::string_view v) { setFloat(sheepInitialCount, v); }},
        {"sheepSpawnYPosition", [&](std::string_view v) { setFloat(sheepSpawnYPosition, v); }},
        {"sheepMoveSpeed", [&](std::string_view v) { setFloat(sheepMoveSpeed, v); }},
        {"sheepSpawnOffsetMin", [&](std::string_view v) { setFloat(sheepSpawnOffsetMin, v); }},
        {"sheepSpawnOffsetMax", [&](std::string_view v) { setFloat(sheepSpawnOffsetMax, v); }},
        {"sheepSize", [&](std::string_view v) { setFloat(sheepSize, v); }},
        {"sheepColorR", [&](std::string_view v) { setFloat(sheepColorR, v); }},
        {"sheepColorG", [&](std::string_view v) { setFloat(sheepColorG, v); }},
        {"sheepColorB", [&](std::string_view v) { setFloat(sheepColorB, v); }},
        {"sheepSpriteScale", [&](std::string_view v) { setFloat(sheepSpriteScale, v); }},
        {"sheepAnimationSpeed", [&](std::string_view v) { setFloat(sheepAnimationSpeed, v); }},

        // Boids / AI / trails
        {"sheepViewRadius", [&](std::string_view v) { setFloat(sheepViewRadius, v); }},
        {"sheepMaxSpeed", [&](std::string_view v) { setFloat(sheepMaxSpeed, v); }},
        {"sheepMaxForce", [&](std::string_view v) { setFloat(sheepMaxForce, v); }},
        {"sheepSeparationWeight", [&](std::string_view v) { setFloat(sheepSeparationWeight, v); }},
        {"sheepAlignmentWeight", [&](std::string_view v) { setFloat(sheepAlignmentWeight, v); }},
        {"sheepCohesionWeight", [&](std::string_view v) { setFloat(sheepCohesionWeight, v); }},
        {"sheepTargetWeight", [&](std::string_view v) { setFloat(sheepTargetWeight, v); }},
        {"sheepFearWeight", [&](std::string_view v) { setFloat(sheepFearWeight, v); }},
        {"sheepEnemyDetectRange", [&](std::string_view v) { setFloat(sheepEnemyDetectRange, v); }},

        {"sheepWeaponArcAngle", [&](std::string_view v) { setFloat(sheepWeaponArcAngle, v); }},
        {"sheepWeaponSpawnRadius", [&](std::string_view v) { setFloat(sheepWeaponSpawnRadius, v); }},

        {"sheepTargetFarDistance", [&](std::string_view v) { setFloat(sheepTargetFarDistance, v); }},
        {"sheepTargetNearDistance", [&](std::string_view v) { setFloat(sheepTargetNearDistance, v); }},

        {"sheepVelocityDamping", [&](std::string_view v) { setFloat(sheepVelocityDamping, v); }},
        {"sheepSeparationSizeMultiplier", [&](std::string_view v) { setFloat(sheepSeparationSizeMultiplier, v); }},
        {"sheepSeparationRepulsionStrength", [&](std::string_view v) { setFloat(sheepSeparationRepulsionStrength, v); }},

        {"spatialGridCellSize", [&](std::string_view v) { setFloat(spatialGridCellSize, v); }},
        {"maxBulletDistance", [&](std::string_view v) { setFloat(maxBulletDistance, v); }},

        {"sheepBulletTrailEmissionRate", [&](std::string_view v) { setFloat(sheepBulletTrailEmissionRate, v); }},
        {"sheepBulletTrailMinSize", [&](std::string_view v) { setFloat(sheepBulletTrailMinSize, v); }},
        {"sheepBulletTrailMaxSize", [&](std::string_view v) { setFloat(sheepBulletTrailMaxSize, v); }},
        {"sheepBulletTrailLife", [&](std::string_view v) { setFloat(sheepBulletTrailLife, v); }},
        {"sheepBulletTrailColorR", [&](std::string_view v) { setFloat(sheepBulletTrailColorR, v); }},
        {"sheepBulletTrailColorG", [&](std::string_view v) { setFloat(sheepBulletTrailColorG, v); }},
        {"sheepBulletTrailColorB", [&](std::string_view v) { setFloat(sheepBulletTrailColorB, v); }},
    };

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }

        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            continue;
        }

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        try {
            auto it = setters.find(key);
            if (it != setters.end()) {
                it->second(value);
            }
        }
        catch (const std::exception&) {
            std::cout << "Error parsing config value for key: " << key << std::endl;
        }
    }

    file.close();
    std::cout << "Config loaded from: " << filename << std::endl;
    return true;
}

bool GameConfig::SaveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "Failed to open config file for writing: " << filename << std::endl;
        return false;
    }

    file << "# Game Configuration File\n";
    file << "# Player Physics\n";
    file << "gravity=" << gravity << "\n";
    file << "jumpVelocity=" << jumpVelocity << "\n";
    file << "forwardSpeed=" << forwardSpeed << "\n";
    file << "strafeSpeed=" << strafeSpeed << "\n";
    file << "acceleration=" << acceleration << "\n";
    file << "damping=" << damping << "\n";
    file << "rotationSpeed=" << rotationSpeed << "\n";
    file << "\n# Camera Controls\n";
    file << "mouseSensitivity=" << mouseSensitivity << "\n";
    file << "maxPitch=" << maxPitch << "\n";
    file << "minPitch=" << minPitch << "\n";
    file << "\n# Map Generation\n";
    file << "blockSize=" << blockSize << "\n";
    file << "renderDistance=" << renderDistance << "\n";
    file << "deleteDistance=" << deleteDistance << "\n";
    file << "roadWidth=" << roadWidth << "\n";
    file << "wallHeight=" << wallHeight << "\n";
    file << "\n# Map Generation Probabilities\n";
    file << "normalFloorChance=" << normalFloorChance << "\n";
    file << "obstacleChance=" << obstacleChance << "\n";
    file << "scorePointChance=" << scorePointChance << "\n";
    file << "upgradePointChance=" << upgradePointChance << "\n";
    file << "gapChance=" << gapChance << "\n";
    file << "gapFloorChance=" << gapFloorChance << "\n";
    file << "\n# Chunk Generation\n";
    file << "chunkSize=" << chunkSize << "\n";
    file << "chunkRenderRadius=" << chunkRenderRadius << "\n";
    file << "\n# Block Heights\n";
    file << "floorHeight=" << floorHeight << "\n";
    file << "tallBlockHeight=" << tallBlockHeight << "\n";
    file << "scorePointHeight=" << scorePointHeight << "\n";
    file << "\n# Bullet Physics\n";
    file << "bulletSpeed=" << bulletSpeed << "\n";
    file << "\n# Player Spawn\n";
    file << "playerSpawnX=" << playerSpawnX << "\n";
    file << "playerSpawnY=" << playerSpawnY << "\n";
    file << "playerSpawnZ=" << playerSpawnZ << "\n";
    file << "\n# Rendering\n";
    file << "fov=" << fov << "\n";
    file << "nearZ=" << nearZ << "\n";
    file << "farZ=" << farZ << "\n";
    file << "\n# Colors (RGB values, 0.0 to 1.0)\n";
    file << "# Player Color\n";
    file << "playerColorR=" << playerColorR << "\n";
    file << "playerColorG=" << playerColorG << "\n";
    file << "playerColorB=" << playerColorB << "\n";
    file << "\n# Bullet Color\n";
    file << "bulletColorR=" << bulletColorR << "\n";
    file << "bulletColorG=" << bulletColorG << "\n";
    file << "bulletColorB=" << bulletColorB << "\n";
    file << "\n# Floor Colors (Alternating)\n";
    file << "floorColor1R=" << floorColor1R << "\n";
    file << "floorColor1G=" << floorColor1G << "\n";
    file << "floorColor1B=" << floorColor1B << "\n";
    file << "floorColor2R=" << floorColor2R << "\n";
    file << "floorColor2G=" << floorColor2G << "\n";
    file << "floorColor2B=" << floorColor2B << "\n";
    file << "\n# Wall Color\n";
    file << "wallColorR=" << wallColorR << "\n";
    file << "wallColorG=" << wallColorG << "\n";
    file << "wallColorB=" << wallColorB << "\n";
    file << "\n# Tall Block Color\n";
    file << "tallBlockColorR=" << tallBlockColorR << "\n";
    file << "tallBlockColorG=" << tallBlockColorG << "\n";
    file << "tallBlockColorB=" << tallBlockColorB << "\n";
    file << "\n# Score Point Color\n";
    file << "scorePointColorR=" << scorePointColorR << "\n";
    file << "scorePointColorG=" << scorePointColorG << "\n";
    file << "scorePointColorB=" << scorePointColorB << "\n";
    file << "\n# Upgrade Point Color\n";
    file << "upgradePointColorR=" << upgradePointColorR << "\n";
    file << "upgradePointColorG=" << upgradePointColorG << "\n";
    file << "upgradePointColorB=" << upgradePointColorB << "\n";
    file << "\n# Upgrade Values\n";
    file << "healthUpgradeAmount=" << healthUpgradeAmount << "\n";
    file << "speedUpgradeAmount=" << speedUpgradeAmount << "\n";
    file << "jumpUpgradeAmount=" << jumpUpgradeAmount << "\n";
    file << "gravityUpgradeAmount=" << gravityUpgradeAmount << "\n";
    file << "bulletSpeedUpgradeAmount=" << bulletSpeedUpgradeAmount << "\n";

    file << "\n# Level/Round system parameters\n";
    file << "roundDurationMs=" << roundDurationMs << "\n";
    file << "baseWolfSpawnIntervalMs=" << baseWolfSpawnIntervalMs << "\n";
    file << "wolfSpawnReductionPerRound=" << wolfSpawnReductionPerRound << "\n";
    file << "minWolfSpawnIntervalMs=" << minWolfSpawnIntervalMs << "\n";
    file << "sheepAddedPerUpgrade=" << sheepAddedPerUpgrade << "\n";
    file << "sheepSpawnOffsetZ=" << sheepSpawnOffsetZ << "\n";

    file << "\n# Wolf type spawn intervals (different wolves spawn at different rates)\n";
    file << "sniperWolfSpawnIntervalMs=" << sniperWolfSpawnIntervalMs << "\n";
    file << "tankWolfSpawnIntervalMs=" << tankWolfSpawnIntervalMs << "\n";
    file << "fastWolfSpawnIntervalMs=" << fastWolfSpawnIntervalMs << "\n";
    file << "hunterWolfSpawnIntervalMs=" << hunterWolfSpawnIntervalMs << "\n";

    file << "\n# Wolf stats - Basic\n";
    file << "wolfBasicHealth=" << wolfBasicHealth << "\n";
    file << "wolfBasicSpeed=" << wolfBasicSpeed << "\n";
    file << "wolfBasicChaseForce=" << wolfBasicChaseForce << "\n";
    file << "wolfBasicDetectionRange=" << wolfBasicDetectionRange << "\n";
    file << "wolfBasicSize=" << wolfBasicSize << "\n";

    file << "\n# Wolf stats - Sniper\n";
    file << "wolfSniperHealth=" << wolfSniperHealth << "\n";
    file << "wolfSniperSpeed=" << wolfSniperSpeed << "\n";
    file << "wolfSniperChaseForce=" << wolfSniperChaseForce << "\n";
    file << "wolfSniperDetectionRange=" << wolfSniperDetectionRange << "\n";
    file << "wolfSniperSize=" << wolfSniperSize << "\n";

    file << "\n# Wolf stats - Tank\n";
    file << "wolfTankHealth=" << wolfTankHealth << "\n";
    file << "wolfTankSpeed=" << wolfTankSpeed << "\n";
    file << "wolfTankChaseForce=" << wolfTankChaseForce << "\n";
    file << "wolfTankDetectionRange=" << wolfTankDetectionRange << "\n";
    file << "wolfTankSize=" << wolfTankSize << "\n";

    file << "\n# Wolf stats - Fast\n";
    file << "wolfFastHealth=" << wolfFastHealth << "\n";
    file << "wolfFastSpeed=" << wolfFastSpeed << "\n";
    file << "wolfFastChaseForce=" << wolfFastChaseForce << "\n";
    file << "wolfFastDetectionRange=" << wolfFastDetectionRange << "\n";
    file << "wolfFastSize=" << wolfFastSize << "\n";

    file << "\n# Wolf stats - Hunter\n";
    file << "wolfHunterHealth=" << wolfHunterHealth << "\n";
    file << "wolfHunterSpeed=" << wolfHunterSpeed << "\n";
    file << "wolfHunterChaseForce=" << wolfHunterChaseForce << "\n";
    file << "wolfHunterDetectionRange=" << wolfHunterDetectionRange << "\n";
    file << "wolfHunterSize=" << wolfHunterSize << "\n";
    file << "wolfHunterJumpCooldown=" << wolfHunterJumpCooldown << "\n";
    file << "wolfHunterJumpVelocity=" << wolfHunterJumpVelocity << "\n";
    file << "wolfHunterJumpBoost=" << wolfHunterJumpBoost << "\n";
    file << "wolfHunterJumpMinDistance=" << wolfHunterJumpMinDistance << "\n";
    file << "wolfHunterJumpMaxDistance=" << wolfHunterJumpMaxDistance << "\n";

    file << "\n# Wolf weapon - Sniper gun\n";
    file << "wolfSniperGunDamage=" << wolfSniperGunDamage << "\n";
    file << "wolfSniperGunFireRate=" << wolfSniperGunFireRate << "\n";
    file << "wolfSniperGunProjectileSpeed=" << wolfSniperGunProjectileSpeed << "\n";
    file << "wolfSniperGunProjectileSize=" << wolfSniperGunProjectileSize << "\n";
    file << "wolfSniperGunProjectileLife=" << wolfSniperGunProjectileLife << "\n";
    file << "wolfSniperGunKnockback=" << wolfSniperGunKnockback << "\n";

    file << "\n# Player weapon - Machine Gun\n";
    file << "playerMachineGunDamage=" << playerMachineGunDamage << "\n";
    file << "playerMachineGunFireRate=" << playerMachineGunFireRate << "\n";
    file << "playerMachineGunProjectileSpeed=" << playerMachineGunProjectileSpeed << "\n";
    file << "playerMachineGunProjectileSize=" << playerMachineGunProjectileSize << "\n";
    file << "playerMachineGunProjectileLife=" << playerMachineGunProjectileLife << "\n";
    file << "playerMachineGunExplosionRadius=" << playerMachineGunExplosionRadius << "\n";
    file << "playerMachineGunKnockback=" << playerMachineGunKnockback << "\n";
    file << "playerMachineGunColorR=" << playerMachineGunColorR << "\n";
    file << "playerMachineGunColorG=" << playerMachineGunColorG << "\n";
    file << "playerMachineGunColorB=" << playerMachineGunColorB << "\n";

    file << "\n# Player weapon - Cannon\n";
    file << "playerCannonDamage=" << playerCannonDamage << "\n";
    file << "playerCannonFireRate=" << playerCannonFireRate << "\n";
    file << "playerCannonProjectileSpeed=" << playerCannonProjectileSpeed << "\n";
    file << "playerCannonProjectileSize=" << playerCannonProjectileSize << "\n";
    file << "playerCannonProjectileLife=" << playerCannonProjectileLife << "\n";
    file << "playerCannonExplosionRadius=" << playerCannonExplosionRadius << "\n";
    file << "playerCannonKnockback=" << playerCannonKnockback << "\n";
    file << "playerCannonColorR=" << playerCannonColorR << "\n";
    file << "playerCannonColorG=" << playerCannonColorG << "\n";
    file << "playerCannonColorB=" << playerCannonColorB << "\n";

    file << "\n# Sheep weapon - Machine Gun\n";
    file << "sheepMachineGunDamage=" << sheepMachineGunDamage << "\n";
    file << "sheepMachineGunFireRate=" << sheepMachineGunFireRate << "\n";
    file << "sheepMachineGunProjectileSpeed=" << sheepMachineGunProjectileSpeed << "\n";
    file << "sheepMachineGunProjectileSize=" << sheepMachineGunProjectileSize << "\n";
    file << "sheepMachineGunProjectileLife=" << sheepMachineGunProjectileLife << "\n";
    file << "sheepMachineGunExplosionRadius=" << sheepMachineGunExplosionRadius << "\n";
    file << "sheepMachineGunKnockback=" << sheepMachineGunKnockback << "\n";
    file << "sheepMachineGunColorR=" << sheepMachineGunColorR << "\n";
    file << "sheepMachineGunColorG=" << sheepMachineGunColorG << "\n";
    file << "sheepMachineGunColorB=" << sheepMachineGunColorB << "\n";

    file << "\n# Sheep weapon - Cannon\n";
    file << "sheepCannonDamage=" << sheepCannonDamage << "\n";
    file << "sheepCannonFireRate=" << sheepCannonFireRate << "\n";
    file << "sheepCannonProjectileSpeed=" << sheepCannonProjectileSpeed << "\n";
    file << "sheepCannonProjectileSize=" << sheepCannonProjectileSize << "\n";
    file << "sheepCannonProjectileLife=" << sheepCannonProjectileLife << "\n";
    file << "sheepCannonExplosionRadius=" << sheepCannonExplosionRadius << "\n";
    file << "sheepCannonKnockback=" << sheepCannonKnockback << "\n";
    file << "sheepCannonColorR=" << sheepCannonColorR << "\n";
    file << "sheepCannonColorG=" << sheepCannonColorG << "\n";
    file << "sheepCannonColorB=" << sheepCannonColorB << "\n";

    file << "\n# Wolf spawn settings\n";
    file << "wolfSpawnRadius=" << wolfSpawnRadius << "\n";
    file << "wolfSpawnYPosition=" << wolfSpawnYPosition << "\n";
    file << "wolfSpawnMinX=" << wolfSpawnMinX << "\n";
    file << "wolfSpawnMaxX=" << wolfSpawnMaxX << "\n";
    file << "wolfSpawnMinZ=" << wolfSpawnMinZ << "\n";
    file << "wolfSpawnMaxZ=" << wolfSpawnMaxZ << "\n";

    file << "\n# Sheep spawn settings\n";
    file << "sheepSpawnRadius=" << sheepSpawnRadius << "\n";
    file << "sheepInitialCount=" << sheepInitialCount << "\n";
    file << "sheepSpawnYPosition=" << sheepSpawnYPosition << "\n";
    file << "sheepMoveSpeed=" << sheepMoveSpeed << "\n";
    file << "sheepSpawnOffsetMin=" << sheepSpawnOffsetMin << "\n";
    file << "sheepSpawnOffsetMax=" << sheepSpawnOffsetMax << "\n";
    file << "sheepSize=" << sheepSize << "\n";
    file << "sheepColorR=" << sheepColorR << "\n";
    file << "sheepColorG=" << sheepColorG << "\n";
    file << "sheepColorB=" << sheepColorB << "\n";
    file << "sheepSpriteScale=" << sheepSpriteScale << "\n";
    file << "sheepAnimationSpeed=" << sheepAnimationSpeed << "\n";

    file << "\n# Sheep Boids behavior\n";
    file << "sheepViewRadius=" << sheepViewRadius << "\n";
    file << "sheepMaxSpeed=" << sheepMaxSpeed << "\n";
    file << "sheepMaxForce=" << sheepMaxForce << "\n";
    file << "sheepSeparationWeight=" << sheepSeparationWeight << "\n";
    file << "sheepAlignmentWeight=" << sheepAlignmentWeight << "\n";
    file << "sheepCohesionWeight=" << sheepCohesionWeight << "\n";
    file << "sheepTargetWeight=" << sheepTargetWeight << "\n";
    file << "sheepFearWeight=" << sheepFearWeight << "\n";
    file << "sheepEnemyDetectRange=" << sheepEnemyDetectRange << "\n";

    file << "\n# Sheep shooting\n";
    file << "sheepWeaponArcAngle=" << sheepWeaponArcAngle << "\n";
    file << "sheepWeaponSpawnRadius=" << sheepWeaponSpawnRadius << "\n";

    file << "\n# Sheep targeting\n";
    file << "sheepTargetFarDistance=" << sheepTargetFarDistance << "\n";
    file << "sheepTargetNearDistance=" << sheepTargetNearDistance << "\n";

    file << "\n# Sheep physics\n";
    file << "sheepVelocityDamping=" << sheepVelocityDamping << "\n";
    file << "sheepSeparationSizeMultiplier=" << sheepSeparationSizeMultiplier << "\n";
    file << "sheepSeparationRepulsionStrength=" << sheepSeparationRepulsionStrength << "\n";

    file << "\n# Optimization\n";
    file << "spatialGridCellSize=" << spatialGridCellSize << "\n";

    file << "\n# Enemy AI\n";
    file << "maxBulletDistance=" << maxBulletDistance << "\n";

    file << "\n# Particle trails\n";
    file << "sheepBulletTrailEmissionRate=" << sheepBulletTrailEmissionRate << "\n";
    file << "sheepBulletTrailMinSize=" << sheepBulletTrailMinSize << "\n";
    file << "sheepBulletTrailMaxSize=" << sheepBulletTrailMaxSize << "\n";
    file << "sheepBulletTrailLife=" << sheepBulletTrailLife << "\n";
    file << "sheepBulletTrailColorR=" << sheepBulletTrailColorR << "\n";
    file << "sheepBulletTrailColorG=" << sheepBulletTrailColorG << "\n";
    file << "sheepBulletTrailColorB=" << sheepBulletTrailColorB << "\n";;
    file.close();
    std::cout << "Config saved to: " << filename << std::endl;
    return true;
}
