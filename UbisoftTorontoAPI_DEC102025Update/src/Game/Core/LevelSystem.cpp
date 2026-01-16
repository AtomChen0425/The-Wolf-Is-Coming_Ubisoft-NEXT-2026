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
            UpgradeType::AddSheep
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
        
        // Find player and apply upgrade
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
                    
                case UpgradeType::AddSheep:
                    // Already handled above
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
            default: return "Unknown effect";
        }
    }
}
