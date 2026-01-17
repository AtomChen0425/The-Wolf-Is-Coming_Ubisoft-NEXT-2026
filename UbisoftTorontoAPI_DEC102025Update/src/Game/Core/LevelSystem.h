#pragma once
#include "GameLevelData.h"
#include "GameConfig.h"
#include "../../System/ECS/ECS.h"

// LevelSystem: Manages round progression and level-related game logic
namespace LevelSystem {
    // Upgrade types available
    enum class UpgradeType {
        // Player attribute upgrades
        HealthBoost,
        SpeedBoost,
        JumpBoost,
        GravityReduction,
        BulletSpeed,
        
        // Entity upgrades
        AddSheep,  // Add more sheep
        
        // Weapon upgrades
        PlayerMachineGun,  // Give player a machine gun
        PlayerCannon,      // Give player a cannon
        SheepMachineGun,   // Give sheep machine guns
        SheepCannon        // Give sheep cannons
    };
    
    // Update level timer and check for round completion
    // Returns true if the round just completed (for triggering upgrade scene)
    bool Update(GameLevelData& levelData, float deltaTimeMs);
    
    // Check if game should end (all sheep dead)
    // Returns true if game over condition is met
    bool CheckGameOver(EntityManager& registry);
    
    // Get count of alive sheep
    int GetSheepCount(EntityManager& registry);
    
    // Generate 3 random unique upgrades
    void GenerateRandomUpgrades(UpgradeType upgradeOptions[3]);
    
    // Apply an upgrade to the player
    void ApplyUpgrade(EntityManager& registry, const GameConfig& config, UpgradeType type);
    
    // Get upgrade name for display
    const char* GetUpgradeName(UpgradeType type);
    
    // Get upgrade description for display
    const char* GetUpgradeDescription(UpgradeType type, const GameConfig& config);
}
