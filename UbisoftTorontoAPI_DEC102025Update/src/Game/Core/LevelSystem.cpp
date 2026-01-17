#include "LevelSystem.h"
#include "../../System/ECS/ECS.h"
#include "../../System/Component/Component.h"
#include "SheepSystem.h"
#include "../../System/Math/Math.h"
#include <vector>
#include <algorithm>

namespace LevelSystem {
    
    bool Update(GameLevelData& levelData, float deltaTimeMs) {
        // Update timers
        levelData.totalGameTimeMs += deltaTimeMs;
        levelData.currentRoundTimeMs += deltaTimeMs;
        
        // Check if round is complete
        if (levelData.IsRoundComplete()) {
            return true;  // Signal that round is complete
        }
        
        return false;
    }
    
    int GetSheepCount(EntityManager& registry) {
        int count = 0;
        View<SheepTag> sheepView(registry);
        for (auto id : sheepView) {
            count++;
        }
        return count;
    }
    
    bool CheckGameOver(EntityManager& registry) {
        // Game is over if there are no sheep left
		View<PlayerTag,Transform3D> playerView(registry);
        for (auto id : playerView) {
            auto& playerPos = playerView.get<Transform3D>(id).pos;
            if (playerPos.y < -500.0f) {
                return true; // Game over if player falls below -500
            }
		}
        return GetSheepCount(registry) == 0;
    }
    
    void GenerateRandomUpgrades(UpgradeType upgradeOptions[3]) {
        // Generate 3 random unique upgrades
        std::vector<UpgradeType> allUpgrades = {
            UpgradeType::HealthBoost,
            UpgradeType::SpeedBoost,
            UpgradeType::JumpBoost,
            UpgradeType::GravityReduction,
            UpgradeType::BulletSpeed,
            UpgradeType::AddSheep,
            UpgradeType::PlayerMachineGun,
            UpgradeType::PlayerCannon,
            UpgradeType::SheepMachineGun,
            UpgradeType::SheepCannon
        };
        
        // Simple shuffle for 3 picks
        for (int i = 0; i < 3; i++) {
            int randomIndex = i + (int)(Rand01() * (allUpgrades.size() - i));
            upgradeOptions[i] = allUpgrades[randomIndex];
            // Swap to avoid duplicates
            std::swap(allUpgrades[i], allUpgrades[randomIndex]);
        }
    }
    
    void ApplyUpgrade(EntityManager& registry, const GameConfig& config, UpgradeType type) {
        // Handle AddSheep separately (doesn't need player stats)
        if (type == UpgradeType::AddSheep) {
            // Add sheep near the player
            View<PlayerTag, Transform3D> playerView(registry);
            for (EntityID id : playerView) {
                auto& playerTransform = playerView.get<Transform3D>(id);
                // Spawn new sheep near the player's position
                SheepSystem::InitSheep(registry, 
                                       playerTransform.pos.x, 
                                       playerTransform.pos.z + config.sheepSpawnOffsetZ, 
                                       config.sheepAddedPerUpgrade);
                break;  // Only need to do this once
            }
            return;
        }
        
        // Handle player weapon upgrades
        if (type == UpgradeType::PlayerMachineGun || type == UpgradeType::PlayerCannon) {
            View<PlayerTag, WeaponInventory> playerView(registry);
            for (EntityID id : playerView) {
                auto& inventory = playerView.get<WeaponInventory>(id);
                
                Weapon weapon;
                if (type == UpgradeType::PlayerMachineGun) {
                    weapon.type = WeaponType::MachineGun;
                    weapon.name = "Machine Gun";
                    weapon.damage = 10.0f;
                    weapon.fireRate = 50.0f;  // Fast fire rate
                    weapon.currentCooldown = 0.0f;
                    weapon.projectileSpeed = 500.0f;
                    weapon.projectileSize = 5.0f;
                    weapon.projectileLife = 2000.0f;
                    weapon.explosionRadius = 0.0f;
                    weapon.r = 1.0f;
                    weapon.g = 1.0f;
                    weapon.b = 0.0f;
                } else {  // PlayerCannon
                    weapon.type = WeaponType::Cannon;
                    weapon.name = "Cannon";
                    weapon.damage = 50.0f;
                    weapon.fireRate = 1000.0f;  // Slow fire rate
                    weapon.currentCooldown = 0.0f;
                    weapon.projectileSpeed = 300.0f;
                    weapon.projectileSize = 15.0f;
                    weapon.projectileLife = 3000.0f;
                    weapon.explosionRadius = 50.0f;
                    weapon.r = 1.0f;
                    weapon.g = 0.5f;
                    weapon.b = 0.0f;
                }
                inventory.weapons.push_back(weapon);
                break;
            }
            return;
        }
        
        // Handle sheep weapon upgrades
        if (type == UpgradeType::SheepMachineGun || type == UpgradeType::SheepCannon) {
            View<SheepTag, WeaponInventory> sheepView(registry);
            
            Weapon weapon;
            if (type == UpgradeType::SheepMachineGun) {
                weapon.type = WeaponType::MachineGun;
                weapon.name = "Sheep MG";
                weapon.damage = 5.0f;
                weapon.fireRate = 50.0f;
                weapon.currentCooldown = 0.0f;
                weapon.projectileSpeed = 400.0f;
                weapon.projectileSize = 3.0f;
                weapon.projectileLife = 2000.0f;
                weapon.explosionRadius = 0.0f;
                weapon.r = 0.5f;
                weapon.g = 1.0f;
                weapon.b = 0.5f;
            } else {  // SheepCannon
                weapon.type = WeaponType::Cannon;
                weapon.name = "Sheep Cannon";
                weapon.damage = 30.0f;
                weapon.fireRate = 1000.0f;
                weapon.currentCooldown = 0.0f;
                weapon.projectileSpeed = 250.0f;
                weapon.projectileSize = 10.0f;
                weapon.projectileLife = 3000.0f;
                weapon.explosionRadius = 40.0f;
                weapon.r = 0.5f;
                weapon.g = 1.0f;
                weapon.b = 1.0f;
            }

            // Add weapon to 10% of sheep
            int sheepCount = 0;
            for (EntityID id : sheepView) {
                auto& inventory = sheepView.get<WeaponInventory>(id);
                sheepCount++;
                inventory.weapons.push_back(weapon);
                if (sheepCount > static_cast<int>(GetSheepCount(registry) * 0.1f)) {
                    break;
                }
                
            }
            return;
        }
        
        // Find player and apply stat upgrade
        View<PlayerTag, PlayerStats> view(registry);
        
        for (EntityID id : view) {
            auto& stats = view.get<PlayerStats>(id);
            
            switch (type) {
                case UpgradeType::HealthBoost:
                    stats.healthBonus += config.healthUpgradeAmount;
                    break;
                    
                case UpgradeType::SpeedBoost:
                    stats.speedBonus += config.speedUpgradeAmount;
                    break;
                    
                case UpgradeType::JumpBoost:
                    stats.jumpBonus += config.jumpUpgradeAmount;
                    break;
                    
                case UpgradeType::GravityReduction:
                    stats.gravityBonus += config.gravityUpgradeAmount;
                    break;
                    
                case UpgradeType::BulletSpeed:
                    stats.bulletSpeedBonus += config.bulletSpeedUpgradeAmount;
                    break;
                    
                default:
                    break;
            }
        }
    }
    
    const char* GetUpgradeName(UpgradeType type) {
        switch (type) {
            case UpgradeType::HealthBoost: return "Health Boost";
            case UpgradeType::SpeedBoost: return "Speed Boost";
            case UpgradeType::JumpBoost: return "Jump Boost";
            case UpgradeType::GravityReduction: return "Gravity Reduction";
            case UpgradeType::BulletSpeed: return "Bullet Speed";
            case UpgradeType::AddSheep: return "Add Sheep";
            case UpgradeType::PlayerMachineGun: return "Player Machine Gun";
            case UpgradeType::PlayerCannon: return "Player Cannon";
            case UpgradeType::SheepMachineGun: return "Sheep Machine Gun";
            case UpgradeType::SheepCannon: return "Sheep Cannon";
            default: return "Unknown";
        }
    }
    
    const char* GetUpgradeDescription(UpgradeType type, const GameConfig& config) {
        // Static buffers for dynamic descriptions
        static char sheepDesc[64];
        
        switch (type) {
            case UpgradeType::HealthBoost: return "Increase max health +20";
            case UpgradeType::SpeedBoost: return "Movement speed +50";
            case UpgradeType::JumpBoost: return "Jump velocity +100";
            case UpgradeType::GravityReduction: return "Lighter jumps (gravity -100)";
            case UpgradeType::BulletSpeed: return "Bullet speed +100";
            case UpgradeType::AddSheep:
                snprintf(sheepDesc, sizeof(sheepDesc), "Add %d more sheep", config.sheepAddedPerUpgrade);
                return sheepDesc;
            case UpgradeType::PlayerMachineGun: return "Give yourself a machine gun";
            case UpgradeType::PlayerCannon: return "Give yourself a cannon";
            case UpgradeType::SheepMachineGun: return "Arm 10% sheep with machine guns";
            case UpgradeType::SheepCannon: return "Arm 10% sheep with cannons";
            default: return "Unknown effect";
        }
    }
}
