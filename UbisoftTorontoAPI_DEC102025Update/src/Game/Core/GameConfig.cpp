#include "GameConfig.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <functional>

GameConfig config;
GameConfig::GameConfig() {

    Header("Player Physics");
    Bind("gravity", gravity, -980.0f);
    Bind("jumpVelocity", jumpVelocity, 400.0f);
    Bind("forwardSpeed", forwardSpeed, 200.0f);
    Bind("strafeSpeed", strafeSpeed, 300.0f);
    Bind("acceleration", acceleration, 2000.0f);
    Bind("damping", damping, 8.0f);
    Bind("rotationSpeed", rotationSpeed, 2.0f);


    Header("Camera Controls");
    Bind("mouseSensitivity", mouseSensitivity, 0.003f);
    Bind("maxPitch", maxPitch, 1.4f);
    Bind("minPitch", minPitch, -1.4f);

    Header("Map Generation");
    Bind("blockSize", blockSize, 100.0f);
    Bind("renderDistance", renderDistance, 1000.0f);
    Bind("deleteDistance", deleteDistance, 1000.0f);
    Bind("roadWidth", roadWidth, 5); 
    Bind("wallHeight", wallHeight, 100.0f);

    Header("Map Generation Probabilities");
    Bind("normalFloorChance", normalFloorChance, 0.6f);
    Bind("obstacleChance", obstacleChance, 0.15f);
    Bind("scorePointChance", scorePointChance, 0.15f);
    Bind("upgradePointChance", upgradePointChance, 0.05f);
    Bind("gapChance", gapChance, 0.10f);
    Bind("gapFloorChance", gapFloorChance, 0.3f);

    Header("Chunk-based Generation");
    Bind("chunkSize", chunkSize, 5); 
    Bind("chunkRenderRadius", chunkRenderRadius, 3); 

    Header("Block Heights");
    Bind("floorHeight", floorHeight, 10.0f);
    Bind("tallBlockHeight", tallBlockHeight, 60.0f);
    Bind("scorePointHeight", scorePointHeight, 30.0f);


    Header("Bullet Physics");
    Bind("bulletSpeed", bulletSpeed, 600.0f);

    Header("Player Spawn");
    Bind("playerSpawnX", playerSpawnX, 0.0f);
    Bind("playerSpawnY", playerSpawnY, 0.0f);
    Bind("playerSpawnZ", playerSpawnZ, 0.0f);

    Header("Rendering");
    Bind("fov", fov, 600.0f);
    Bind("nearZ", nearZ, 1.0f);
    Bind("farZ", farZ, 3000.0f);


    Header("Colors");

    // Player Color
    Bind("playerColorR", playerColorR, 1.0f);
    Bind("playerColorG", playerColorG, 0.0f);
    Bind("playerColorB", playerColorB, 0.0f);

    // Bullet Color
    Bind("bulletColorR", bulletColorR, 0.5f);
    Bind("bulletColorG", bulletColorG, 0.0f);
    Bind("bulletColorB", bulletColorB, 0.5f);

    // Floor Colors
    Bind("floorColor1R", floorColor1R, 0.2f);
    Bind("floorColor1G", floorColor1G, 0.6f);
    Bind("floorColor1B", floorColor1B, 0.2f);
    Bind("floorColor2R", floorColor2R, 0.3f);
    Bind("floorColor2G", floorColor2G, 0.8f);
    Bind("floorColor2B", floorColor2B, 0.3f);

    // Wall & Block Colors
    Bind("wallColorR", wallColorR, 0.6f);
    Bind("wallColorG", wallColorG, 0.3f);
    Bind("wallColorB", wallColorB, 0.1f);

    Bind("tallBlockColorR", tallBlockColorR, 0.1f);
    Bind("tallBlockColorG", tallBlockColorG, 0.3f);
    Bind("tallBlockColorB", tallBlockColorB, 0.9f);

    Bind("scorePointColorR", scorePointColorR, 1.0f);
    Bind("scorePointColorG", scorePointColorG, 1.0f);
    Bind("scorePointColorB", scorePointColorB, 0.0f);

    Bind("upgradePointColorR", upgradePointColorR, 1.0f);
    Bind("upgradePointColorG", upgradePointColorG, 0.5f);
    Bind("upgradePointColorB", upgradePointColorB, 0.0f);


    Header("Upgrade Values");
    Bind("healthUpgradeAmount", healthUpgradeAmount, 20.0f);
    Bind("speedUpgradeAmount", speedUpgradeAmount, 50.0f);
    Bind("jumpUpgradeAmount", jumpUpgradeAmount, 100.0f);
    Bind("gravityUpgradeAmount", gravityUpgradeAmount, -100.0f);
    Bind("bulletSpeedUpgradeAmount", bulletSpeedUpgradeAmount, 100.0f);

    Header("Level/Round System");
    Bind("roundDurationMs", roundDurationMs, 10000.0f);
    Bind("baseWolfSpawnIntervalMs", baseWolfSpawnIntervalMs, 1000.0f);
    Bind("wolfSpawnReductionPerRound", wolfSpawnReductionPerRound, 200.0f);
    Bind("minWolfSpawnIntervalMs", minWolfSpawnIntervalMs, 1000.0f);
    Bind("sheepAddedPerUpgrade", sheepAddedPerUpgrade, 10); // int
    Bind("sheepSpawnOffsetZ", sheepSpawnOffsetZ, 100.0f);

    Header("Wolf Spawn Intervals");
    Bind("sniperWolfSpawnIntervalMs", sniperWolfSpawnIntervalMs, 8000.0f);
    Bind("tankWolfSpawnIntervalMs", tankWolfSpawnIntervalMs, 12000.0f);
    Bind("fastWolfSpawnIntervalMs", fastWolfSpawnIntervalMs, 5000.0f);
    Bind("hunterWolfSpawnIntervalMs", hunterWolfSpawnIntervalMs, 10000.0f);


    Header("Wolf Stats - Basic");
    Bind("wolfBasicHealth", wolfBasicHealth, 100);
    Bind("wolfBasicSpeed", wolfBasicSpeed, 300.0f);
    Bind("wolfBasicChaseForce", wolfBasicChaseForce, 500.0f);
    Bind("wolfBasicDetectionRange", wolfBasicDetectionRange, 300.0f);
    Bind("wolfBasicSize", wolfBasicSize, 20.0f);

    Header("Wolf Stats - Sniper");
    Bind("wolfSniperHealth", wolfSniperHealth, 100);
    Bind("wolfSniperSpeed", wolfSniperSpeed, 150.0f);
    Bind("wolfSniperChaseForce", wolfSniperChaseForce, 300.0f);
    Bind("wolfSniperDetectionRange", wolfSniperDetectionRange, 400.0f);
    Bind("wolfSniperSize", wolfSniperSize, 20.0f);\

    Header("Wolf Stats - Magic");
    Bind("wolfMagicHealth", wolfMagicHealth, 100);
    Bind("wolfMagicSpeed", wolfMagicSpeed, 100.0f);
    Bind("wolfMagicChaseForce", wolfMagicChaseForce, 300.0f);
    Bind("wolfMagicDetectionRange", wolfMagicDetectionRange, 400.0f);
    Bind("wolfMagicSize", wolfMagicSize, 20.0f);

    Header("Wolf Stats - Tank");
    Bind("wolfTankHealth", wolfTankHealth, 300);
    Bind("wolfTankSpeed", wolfTankSpeed, 100.0f);
    Bind("wolfTankChaseForce", wolfTankChaseForce, 400.0f);
    Bind("wolfTankDetectionRange", wolfTankDetectionRange, 250.0f);
    Bind("wolfTankSize", wolfTankSize, 30.0f);

    Header("Wolf Stats - Fast");
    Bind("wolfFastHealth", wolfFastHealth, 60);
    Bind("wolfFastSpeed", wolfFastSpeed, 500.0f);
    Bind("wolfFastChaseForce", wolfFastChaseForce, 600.0f);
    Bind("wolfFastDetectionRange", wolfFastDetectionRange, 350.0f);
    Bind("wolfFastSize", wolfFastSize, 15.0f);

    Header("Wolf Stats - Hunter");
    Bind("wolfHunterHealth", wolfHunterHealth, 80);
    Bind("wolfHunterSpeed", wolfHunterSpeed, 450.0f);
    Bind("wolfHunterChaseForce", wolfHunterChaseForce, 550.0f);
    Bind("wolfHunterDetectionRange", wolfHunterDetectionRange, 300.0f);
    Bind("wolfHunterSize", wolfHunterSize, 20.0f);
    Bind("wolfHunterJumpCooldown", wolfHunterJumpCooldown, 3.0f);
    Bind("wolfHunterJumpVelocity", wolfHunterJumpVelocity, 350.0f);
    Bind("wolfHunterJumpBoost", wolfHunterJumpBoost, 200.0f);
    Bind("wolfHunterJumpMinDistance", wolfHunterJumpMinDistance, 80.0f);
    Bind("wolfHunterJumpMaxDistance", wolfHunterJumpMaxDistance, 200.0f);

    // ==========================================
    // 8. Weapons
    // ==========================================
    Header("Wolf Weapon - Sniper Gun");
    Bind("wolfSniperGunDamage", wolfSniperGunDamage, 8.0f);
    Bind("wolfSniperGunFireRate", wolfSniperGunFireRate, 500.0f);
    Bind("wolfSniperGunProjectileSpeed", wolfSniperGunProjectileSpeed, 600.0f);
    Bind("wolfSniperGunProjectileSize", wolfSniperGunProjectileSize, 5.0f);
    Bind("wolfSniperGunProjectileLife", wolfSniperGunProjectileLife, 2000.0f);
    Bind("wolfSniperGunKnockback", wolfSniperGunKnockback, 150.0f);

    Header("Wolf Weapon - Magic Wand");
    Bind("wolfMagicWandDamage", wolfMagicWandDamage, 10.0f);
    Bind("wolfMagicWandFireRate", wolfMagicWandFireRate, 300.0f);
    Bind("wolfMagicWandProjectileSpeed", wolfMagicWandProjectileSpeed, 400.0f);
    Bind("wolfMagicWandProjectileSize", wolfMagicWandProjectileSize, 6.0f);
    Bind("wolfMagicWandProjectileLife", wolfMagicWandProjectileLife, 2500.0f);
    Bind("wolfMagicWandKnockback", wolfMagicWandKnockback, 100.0f);

    Header("Player Weapon - Machine Gun");
    Bind("playerMachineGunDamage", playerMachineGunDamage, 10.0f);
    Bind("playerMachineGunFireRate", playerMachineGunFireRate, 50.0f);
    Bind("playerMachineGunProjectileSpeed", playerMachineGunProjectileSpeed, 500.0f);
    Bind("playerMachineGunProjectileSize", playerMachineGunProjectileSize, 5.0f);
    Bind("playerMachineGunProjectileLife", playerMachineGunProjectileLife, 2000.0f);
    Bind("playerMachineGunExplosionRadius", playerMachineGunExplosionRadius, 0.0f);
    Bind("playerMachineGunKnockback", playerMachineGunKnockback, 100.0f);
    Bind("playerMachineGunColorR", playerMachineGunColorR, 1.0f);
    Bind("playerMachineGunColorG", playerMachineGunColorG, 1.0f);
    Bind("playerMachineGunColorB", playerMachineGunColorB, 0.0f);

    Header("Player Weapon - Cannon");
    Bind("playerCannonDamage", playerCannonDamage, 50.0f);
    Bind("playerCannonFireRate", playerCannonFireRate, 1000.0f);
    Bind("playerCannonProjectileSpeed", playerCannonProjectileSpeed, 300.0f);
    Bind("playerCannonProjectileSize", playerCannonProjectileSize, 15.0f);
    Bind("playerCannonProjectileLife", playerCannonProjectileLife, 3000.0f);
    Bind("playerCannonExplosionRadius", playerCannonExplosionRadius, 50.0f);
    Bind("playerCannonKnockback", playerCannonKnockback, 500.0f);
    Bind("playerCannonColorR", playerCannonColorR, 1.0f);
    Bind("playerCannonColorG", playerCannonColorG, 0.5f);
    Bind("playerCannonColorB", playerCannonColorB, 0.0f);

    Header("Sheep Weapon - Machine Gun");
    Bind("sheepMachineGunDamage", sheepMachineGunDamage, 5.0f);
    Bind("sheepMachineGunFireRate", sheepMachineGunFireRate, 50.0f);
    Bind("sheepMachineGunProjectileSpeed", sheepMachineGunProjectileSpeed, 400.0f);
    Bind("sheepMachineGunProjectileSize", sheepMachineGunProjectileSize, 3.0f);
    Bind("sheepMachineGunProjectileLife", sheepMachineGunProjectileLife, 2000.0f);
    Bind("sheepMachineGunExplosionRadius", sheepMachineGunExplosionRadius, 0.0f);
    Bind("sheepMachineGunKnockback", sheepMachineGunKnockback, 50.0f);
    Bind("sheepMachineGunColorR", sheepMachineGunColorR, 0.5f);
    Bind("sheepMachineGunColorG", sheepMachineGunColorG, 1.0f);
    Bind("sheepMachineGunColorB", sheepMachineGunColorB, 0.5f);

    Header("Sheep Weapon - Cannon");
    Bind("sheepCannonDamage", sheepCannonDamage, 30.0f);
    Bind("sheepCannonFireRate", sheepCannonFireRate, 1000.0f);
    Bind("sheepCannonProjectileSpeed", sheepCannonProjectileSpeed, 250.0f);
    Bind("sheepCannonProjectileSize", sheepCannonProjectileSize, 10.0f);
    Bind("sheepCannonProjectileLife", sheepCannonProjectileLife, 3000.0f);
    Bind("sheepCannonExplosionRadius", sheepCannonExplosionRadius, 50.0f);
    Bind("sheepCannonKnockback", sheepCannonKnockback, 300.0f);
    Bind("sheepCannonColorR", sheepCannonColorR, 0.5f);
    Bind("sheepCannonColorG", sheepCannonColorG, 1.0f);
    Bind("sheepCannonColorB", sheepCannonColorB, 1.0f);

    // ==========================================
    // 9. Entity Settings
    // ==========================================
    Header("Wolf Spawn Settings");
    Bind("wolfSpawnRadius", wolfSpawnRadius, 1700.0f);
    Bind("wolfSpawnYPosition", wolfSpawnYPosition, 20.0f);
    Bind("wolfSpawnMinX", wolfSpawnMinX, -200.0f);
    Bind("wolfSpawnMaxX", wolfSpawnMaxX, 200.0f);
    Bind("wolfSpawnMinZ", wolfSpawnMinZ, -50.0f);
    Bind("wolfSpawnMaxZ", wolfSpawnMaxZ, -30.0f);

    Header("Sheep Spawn Settings");
    Bind("sheepSpawnRadius", sheepSpawnRadius, 100.0f);
    Bind("sheepInitialCount", sheepInitialCount, 10); // int
    Bind("sheepSpawnYPosition", sheepSpawnYPosition, 30.0f);
    Bind("sheepSpawnOffsetMin", sheepSpawnOffsetMin, -100.0f);
    Bind("sheepSpawnOffsetMax", sheepSpawnOffsetMax, 100.0f);
    Bind("sheepSize", sheepSize, 15.0f);
    Bind("sheepColorR", sheepColorR, 0.9f);
    Bind("sheepColorG", sheepColorG, 0.9f);
    Bind("sheepColorB", sheepColorB, 0.9f);
    Bind("sheepSpriteScale", sheepSpriteScale, 0.3f);
    Bind("sheepAnimationSpeed", sheepAnimationSpeed, 1.0f / 15.0f);

    Header("Sheep Boids Behavior");
    Bind("sheepViewRadius", sheepViewRadius, 70.0f);
    Bind("sheepMaxSpeed", sheepMaxSpeed, 60.0f);
    Bind("sheepMaxForce", sheepMaxForce, 0.6f);
    Bind("sheepSeparationWeight", sheepSeparationWeight, 1.5f);
    Bind("sheepAlignmentWeight", sheepAlignmentWeight, 1.0f);
    Bind("sheepCohesionWeight", sheepCohesionWeight, 1.0f);
    Bind("sheepTargetWeight", sheepTargetWeight, 0.5f);
    Bind("sheepFearWeight", sheepFearWeight, 4.0f);
    Bind("sheepEnemyDetectRange", sheepEnemyDetectRange, 150.0f);

    Header("Sheep Combat");
    Bind("sheepWeaponArcAngle", sheepWeaponArcAngle, 1.5f * 3.14159265f);
    Bind("sheepWeaponSpawnRadius", sheepWeaponSpawnRadius, 10.0f);
    Bind("sheepTargetFarDistance", sheepTargetFarDistance, 60.0f);
    Bind("sheepTargetNearDistance", sheepTargetNearDistance, 30.0f);

    Header("Sheep Physics");
    Bind("sheepVelocityDamping", sheepVelocityDamping, 0.98f);
    Bind("sheepSeparationSizeMultiplier", sheepSeparationSizeMultiplier, 0.55f);
    Bind("sheepSeparationRepulsionStrength", sheepSeparationRepulsionStrength, 2.0f);

    Header("Particle Trails");
    Bind("sheepBulletTrailEmissionRate", sheepBulletTrailEmissionRate, 50.0f);
    Bind("sheepBulletTrailMinSize", sheepBulletTrailMinSize, 0.0f);
    Bind("sheepBulletTrailMaxSize", sheepBulletTrailMaxSize, 150.0f);
    Bind("sheepBulletTrailLife", sheepBulletTrailLife, 3.0f);
    Bind("sheepBulletTrailColorR", sheepBulletTrailColorR, 1.0f);
    Bind("sheepBulletTrailColorG", sheepBulletTrailColorG, 0.5f);
    Bind("sheepBulletTrailColorB", sheepBulletTrailColorB, 0.0f);

    LoadFromFile("game_config.txt");
}
bool GameConfig::LoadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Config file not found: " << filename << ". Using defaults." << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        auto trim = [](std::string& s) {
            s.erase(0, s.find_first_not_of(" \t\r\n"));
            s.erase(s.find_last_not_of(" \t\r\n") + 1);
            };
        trim(line);

        if (line.empty() || line[0] == '#' || line[0] == ';') continue;

        size_t pos = line.find('=');
        if (pos == std::string::npos) continue;

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        trim(key);
        trim(value);

        auto it = propertyMap.find(key);
        if (it != propertyMap.end()) {
            it->second->SetFromString(value);
        }
    }

    return true;
}

bool GameConfig::SaveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) return false;

    file << "# Auto-Generated Game Configuration File\n";

    for (const auto& item : savedItems) {
        if (item.isComment) {
            file << item.text << "\n";
        }
        else if (item.prop) {
            file << item.prop->key << "=" << item.prop->ToString() << "\n";
        }
    }

    std::cout << "Config saved to: " << filename << std::endl;
    return true;
}