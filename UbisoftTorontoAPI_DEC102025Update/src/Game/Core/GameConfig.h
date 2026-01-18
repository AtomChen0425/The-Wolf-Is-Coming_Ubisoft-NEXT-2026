#pragma once
#include <string>

// Game configuration loaded from file
struct GameConfig {
    // Player physics
    float gravity = -980.0f;
    float jumpVelocity = 400.0f;
    float forwardSpeed = 200.0f;
    float strafeSpeed = 300.0f;
    float acceleration = 2000.0f;
    float damping = 8.0f;
    float rotationSpeed = 2.0f;
    
    // Camera controls
    float mouseSensitivity = 0.003f;
    float maxPitch = 1.4f;  // ~80 degrees
    float minPitch = -1.4f;
    
    // Map generation
    float blockSize = 100.0f;
    float renderDistance = 1000.0f;
    float deleteDistance = 1000.0f;
    int roadWidth = 5;
    float wallHeight = 100.0f;
    
    // Map generation probabilities
    float normalFloorChance = 0.6f;
    float obstacleChance = 0.15f;
    float scorePointChance = 0.15f;
    float upgradePointChance = 0.05f;  // Chance for upgrade points
    float gapChance = 0.10f;
    float gapFloorChance = 0.3f;
    
    // Chunk-based generation
    int chunkSize = 5;           // Blocks per chunk (width and depth)
    int chunkRenderRadius = 3;   // How many chunks to render in each direction
    
    // Block heights
    float floorHeight = 30.0f;
    float tallBlockHeight = 60.0f;
    float scorePointHeight = 30.0f;
    
    // Bullet physics
    float bulletSpeed = 600.0f;
    
    // Player spawn
    float playerSpawnX = 0.0f;
    float playerSpawnY = 0.0f;
    float playerSpawnZ = 50.0f;
    
    // Rendering
    float fov = 600.0f;
    float nearZ = 1.0f;
    float farZ = 3000.0f;
    
    // Colors (RGB values, 0.0 to 1.0)
    // Player color
    float playerColorR = 1.0f;
    float playerColorG = 0.0f;
    float playerColorB = 0.0f;
    
    // Bullet color
    float bulletColorR = 0.5f;
    float bulletColorG = 0.0f;
    float bulletColorB = 0.5f;
    
    // Floor colors (alternating)
    float floorColor1R = 0.2f;
    float floorColor1G = 0.6f;
    float floorColor1B = 0.2f;
    float floorColor2R = 0.3f;
    float floorColor2G = 0.8f;
    float floorColor2B = 0.3f;
    
    // Wall color
    float wallColorR = 0.6f;
    float wallColorG = 0.3f;
    float wallColorB = 0.1f;
    
    // Tall block color
    float tallBlockColorR = 0.1f;
    float tallBlockColorG = 0.3f;
    float tallBlockColorB = 0.9f;
    
    // Score point color
    float scorePointColorR = 1.0f;
    float scorePointColorG = 1.0f;
    float scorePointColorB = 0.0f;
    
    // Upgrade point color
    float upgradePointColorR = 1.0f;
    float upgradePointColorG = 0.5f;
    float upgradePointColorB = 0.0f;
    
    // Upgrade values
    float healthUpgradeAmount = 20.0f;
    float speedUpgradeAmount = 50.0f;
    float jumpUpgradeAmount = 100.0f;
    float gravityUpgradeAmount = -100.0f;  // Negative makes you lighter
    float bulletSpeedUpgradeAmount = 100.0f;
    
    // Level/Round system parameters
    float roundDurationMs = 10000.0f;           // Duration of each round (60 seconds)
    float baseWolfSpawnIntervalMs = 1000.0f;    // Initial wolf spawn interval (1 second)
    float wolfSpawnReductionPerRound = 200.0f;  // Spawn interval reduction per round
    float minWolfSpawnIntervalMs = 1000.0f;     // Minimum wolf spawn interval (1 second)
    int sheepAddedPerUpgrade = 10;              // Number of sheep added with "Add Sheep" upgrade
    float sheepSpawnOffsetZ = 100.0f;           // Z offset for spawning new sheep
    
    // Wolf type spawn intervals (different wolves spawn at different rates)
    float sniperWolfSpawnIntervalMs = 8000.0f;   // Sniper wolves spawn every 8 seconds
    float tankWolfSpawnIntervalMs = 12000.0f;    // Tank wolves spawn every 12 seconds
    float fastWolfSpawnIntervalMs = 5000.0f;     // Fast wolves spawn every 5 seconds
    float hunterWolfSpawnIntervalMs = 10000.0f;  // Hunter wolves spawn every 10 seconds
    
    // Wolf stats - Basic
    int wolfBasicHealth = 100;
    float wolfBasicSpeed = 200.0f;
    float wolfBasicChaseForce = 300.0f;
    float wolfBasicDetectionRange = 400.0f;
    float wolfBasicSize = 20.0f;
    
    // Wolf stats - Sniper
    int wolfSniperHealth = 100;
    float wolfSniperSpeed = 150.0f;
    float wolfSniperChaseForce = 100.0f;
    float wolfSniperDetectionRange = 400.0f;
    float wolfSniperSize = 18.0f;
    
    // Wolf stats - Tank
    int wolfTankHealth = 300;
    float wolfTankSpeed = 100.0f;
    float wolfTankChaseForce = 150.0f;
    float wolfTankDetectionRange = 400.0f;
    float wolfTankSize = 30.0f;
    
    // Wolf stats - Fast
    int wolfFastHealth = 60;
    float wolfFastSpeed = 500.0f;
    float wolfFastChaseForce = 300.0f;
    float wolfFastDetectionRange = 400.0f;
    float wolfFastSize = 15.0f;
    
    // Wolf stats - Hunter
    int wolfHunterHealth = 80;
    float wolfHunterSpeed = 450.0f;
    float wolfHunterChaseForce = 280.0f;
    float wolfHunterDetectionRange = 400.0f;
    float wolfHunterSize = 18.0f;
    float wolfHunterJumpCooldown = 3.0f;
    float wolfHunterJumpVelocity = 350.0f;
    float wolfHunterJumpBoost = 200.0f;
    float wolfHunterJumpMinDist = 80.0f;
    float wolfHunterJumpMaxDist = 200.0f;
    
    // Wolf stats - Magic
    int wolfMagicHealth = 100;
    float wolfMagicSpeed = 150.0f;
    float wolfMagicChaseForce = 100.0f;
    float wolfMagicDetectionRange = 400.0f;
    float wolfMagicSize = 18.0f;
    
    // Wolf weapons - Sniper gun
    float wolfSniperGunDamage = 8.0f;
    float wolfSniperGunFireRate = 70.0f;
    float wolfSniperGunProjectileSpeed = 1000.0f;
    float wolfSniperGunProjectileSize = 10.0f;
    float wolfSniperGunProjectileLife = 8000.0f;
    float wolfSniperGunKnockback = 10.0f;
    
    // Wolf weapons - Magic wand
    float wolfMagicWandDamage = 12.0f;
    float wolfMagicWandFireRate = 80.0f;
    float wolfMagicWandProjectileSpeed = 600.0f;
    float wolfMagicWandProjectileSize = 12.0f;
    float wolfMagicWandProjectileLife = 7000.0f;
    float wolfMagicWandKnockback = 15.0f;
    
    // Player weapons - Machine Gun
    float playerMachineGunDamage = 10.0f;
    float playerMachineGunFireRate = 50.0f;
    float playerMachineGunProjectileSpeed = 500.0f;
    float playerMachineGunProjectileSize = 5.0f;
    float playerMachineGunProjectileLife = 2000.0f;
    float playerMachineGunKnockback = 100.0f;
    
    // Player weapons - Cannon
    float playerCannonDamage = 50.0f;
    float playerCannonFireRate = 1000.0f;
    float playerCannonProjectileSpeed = 300.0f;
    float playerCannonProjectileSize = 15.0f;
    float playerCannonProjectileLife = 3000.0f;
    float playerCannonExplosionRadius = 50.0f;
    float playerCannonKnockback = 500.0f;
    
    // Sheep weapons - Machine Gun
    float sheepMachineGunDamage = 5.0f;
    float sheepMachineGunFireRate = 50.0f;
    float sheepMachineGunProjectileSpeed = 400.0f;
    float sheepMachineGunProjectileSize = 3.0f;
    float sheepMachineGunProjectileLife = 2000.0f;
    float sheepMachineGunKnockback = 50.0f;
    
    // Sheep weapons - Cannon
    float sheepCannonDamage = 30.0f;
    float sheepCannonFireRate = 1000.0f;
    float sheepCannonProjectileSpeed = 250.0f;
    float sheepCannonProjectileSize = 10.0f;
    float sheepCannonProjectileLife = 2500.0f;
    float sheepCannonExplosionRadius = 30.0f;
    float sheepCannonKnockback = 200.0f;
    
    // Wolf spawn settings
    float wolfSpawnRadius = 1700.0f;
    float wolfSpawnYPosition = 20.0f;
    
    // Sheep spawn settings
    float sheepSpawnYPosition = 30.0f;
    float sheepMoveSpeed = 50.0f;
    float sheepSpawnOffsetMin = -100.0f;
    float sheepSpawnOffsetMax = 100.0f;
    float sheepSize = 15.0f;
    float sheepColorR = 0.9f;
    float sheepColorG = 0.9f;
    float sheepColorB = 0.9f;
    float sheepSpriteScale = 0.3f;
    float sheepAnimationSpeed = 1.0f / 15.0f;
    
    // Sheep Boids behavior parameters (SheepComponent defaults)
    float sheepViewRadius = 70.0f;
    float sheepMaxSpeed = 60.0f;
    float sheepMaxForce = 0.6f;
    float sheepSeparationWeight = 1.5f;
    float sheepAlignmentWeight = 1.0f;
    float sheepCohesionWeight = 1.0f;
    float sheepTargetWeight = 0.5f;
    float sheepFearWeight = 4.0f;
    float sheepEnemyDetectRange = 150.0f;
    
    // Sheep shooting parameters
    float sheepWeaponArcAngle = 1.5f * 3.14159265f;  // Total arc for spreading weapons
    float sheepWeaponSpawnRadius = 10.0f;  // Radius for weapon spawn circle
    
    // Sheep targeting parameters
    float sheepTargetFarDistance = 60.0f;   // Distance at which sheep move toward player
    float sheepTargetNearDistance = 30.0f;  // Distance at which sheep move away from player
    
    // Sheep velocity damping
    float sheepVelocityDamping = 0.98f;
    
    // Sheep separation parameters
    float sheepSeparationSizeMultiplier = 0.55f;  // Minimum safe distance multiplier
    float sheepSeparationRepulsionStrength = 2.0f;  // Repulsion strength for very close neighbors
    
    // Spatial grid optimization
    float spatialGridCellSize = 80.0f;  // Cell size for spatial grid optimization
    
    // Enemy AI parameters
    float maxBulletDistance = 5000.0f;  // Maximum distance bullets can travel from origin
    
    // Particle trail parameters (for sheep bullets)
    float sheepBulletTrailEmissionRate = 50.0f;
    float sheepBulletTrailMinSize = 0.0f;
    float sheepBulletTrailMaxSize = 150.0f;
    float sheepBulletTrailLife = 3.0f;
    float sheepBulletTrailColorR = 1.0f;
    float sheepBulletTrailColorG = 0.5f;
    float sheepBulletTrailColorB = 0.0f;
    
    // Load config from file (returns true if successful)
    bool LoadFromFile(const std::string& filename);
    
    // Save config to file
    bool SaveToFile(const std::string& filename) const;
    
    // Load default values
    void LoadDefaults();
};
