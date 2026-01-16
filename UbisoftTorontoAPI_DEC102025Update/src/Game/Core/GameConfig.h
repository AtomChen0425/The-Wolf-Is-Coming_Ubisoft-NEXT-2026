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
    float gapChance = 0.10f;
    float gapFloorChance = 0.3f;
    
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
    
    // Load config from file (returns true if successful)
    bool LoadFromFile(const std::string& filename);
    
    // Save config to file
    bool SaveToFile(const std::string& filename) const;
    
    // Load default values
    void LoadDefaults();
};
