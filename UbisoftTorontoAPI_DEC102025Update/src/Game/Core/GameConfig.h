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
    float floorHeight = 10.0f;
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
    
    // Load config from file (returns true if successful)
    bool LoadFromFile(const std::string& filename);
    
    // Save config to file
    bool SaveToFile(const std::string& filename) const;
    
    // Load default values
    void LoadDefaults();
};
