#include "GameConfig.h"
#include <fstream>
#include <sstream>
#include <iostream>

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
}

bool GameConfig::LoadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Config file not found: " << filename << ". Using defaults." << std::endl;
        LoadDefaults();
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }
        
        // Find the '=' separator
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            continue;
        }
        
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        
        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        // Parse the value
        try {
            if (key == "gravity") gravity = std::stof(value);
            else if (key == "jumpVelocity") jumpVelocity = std::stof(value);
            else if (key == "forwardSpeed") forwardSpeed = std::stof(value);
            else if (key == "strafeSpeed") strafeSpeed = std::stof(value);
            else if (key == "acceleration") acceleration = std::stof(value);
            else if (key == "damping") damping = std::stof(value);
            else if (key == "rotationSpeed") rotationSpeed = std::stof(value);
            else if (key == "mouseSensitivity") mouseSensitivity = std::stof(value);
            else if (key == "maxPitch") maxPitch = std::stof(value);
            else if (key == "minPitch") minPitch = std::stof(value);
            else if (key == "blockSize") blockSize = std::stof(value);
            else if (key == "renderDistance") renderDistance = std::stof(value);
            else if (key == "deleteDistance") deleteDistance = std::stof(value);
            else if (key == "roadWidth") roadWidth = std::stoi(value);
            else if (key == "wallHeight") wallHeight = std::stof(value);
            else if (key == "normalFloorChance") normalFloorChance = std::stof(value);
            else if (key == "obstacleChance") obstacleChance = std::stof(value);
            else if (key == "scorePointChance") scorePointChance = std::stof(value);
            else if (key == "gapChance") gapChance = std::stof(value);
            else if (key == "gapFloorChance") gapFloorChance = std::stof(value);
            else if (key == "floorHeight") floorHeight = std::stof(value);
            else if (key == "tallBlockHeight") tallBlockHeight = std::stof(value);
            else if (key == "scorePointHeight") scorePointHeight = std::stof(value);
            else if (key == "bulletSpeed") bulletSpeed = std::stof(value);
            else if (key == "playerSpawnX") playerSpawnX = std::stof(value);
            else if (key == "playerSpawnY") playerSpawnY = std::stof(value);
            else if (key == "playerSpawnZ") playerSpawnZ = std::stof(value);
            else if (key == "fov") fov = std::stof(value);
            else if (key == "nearZ") nearZ = std::stof(value);
            else if (key == "farZ") farZ = std::stof(value);
            // Color parameters
            else if (key == "playerColorR") playerColorR = std::stof(value);
            else if (key == "playerColorG") playerColorG = std::stof(value);
            else if (key == "playerColorB") playerColorB = std::stof(value);
            else if (key == "bulletColorR") bulletColorR = std::stof(value);
            else if (key == "bulletColorG") bulletColorG = std::stof(value);
            else if (key == "bulletColorB") bulletColorB = std::stof(value);
            else if (key == "floorColor1R") floorColor1R = std::stof(value);
            else if (key == "floorColor1G") floorColor1G = std::stof(value);
            else if (key == "floorColor1B") floorColor1B = std::stof(value);
            else if (key == "floorColor2R") floorColor2R = std::stof(value);
            else if (key == "floorColor2G") floorColor2G = std::stof(value);
            else if (key == "floorColor2B") floorColor2B = std::stof(value);
            else if (key == "wallColorR") wallColorR = std::stof(value);
            else if (key == "wallColorG") wallColorG = std::stof(value);
            else if (key == "wallColorB") wallColorB = std::stof(value);
            else if (key == "tallBlockColorR") tallBlockColorR = std::stof(value);
            else if (key == "tallBlockColorG") tallBlockColorG = std::stof(value);
            else if (key == "tallBlockColorB") tallBlockColorB = std::stof(value);
            else if (key == "scorePointColorR") scorePointColorR = std::stof(value);
            else if (key == "scorePointColorG") scorePointColorG = std::stof(value);
            else if (key == "scorePointColorB") scorePointColorB = std::stof(value);
            // Upgrade and chunk parameters
            else if (key == "upgradePointChance") upgradePointChance = std::stof(value);
            else if (key == "chunkSize") chunkSize = std::stoi(value);
            else if (key == "chunkRenderRadius") chunkRenderRadius = std::stoi(value);
            else if (key == "upgradePointColorR") upgradePointColorR = std::stof(value);
            else if (key == "upgradePointColorG") upgradePointColorG = std::stof(value);
            else if (key == "upgradePointColorB") upgradePointColorB = std::stof(value);
            else if (key == "healthUpgradeAmount") healthUpgradeAmount = std::stof(value);
            else if (key == "speedUpgradeAmount") speedUpgradeAmount = std::stof(value);
            else if (key == "jumpUpgradeAmount") jumpUpgradeAmount = std::stof(value);
            else if (key == "gravityUpgradeAmount") gravityUpgradeAmount = std::stof(value);
            else if (key == "bulletSpeedUpgradeAmount") bulletSpeedUpgradeAmount = std::stof(value);
            //
			else if (key == "roundDurationMs") roundDurationMs = std::stof(value);
            else if (key == "baseWolfSpawnIntervalMs") baseWolfSpawnIntervalMs = std::stof(value);
            else if (key == "wolfSpawnReductionPerRound") wolfSpawnReductionPerRound = std::stof(value);
            else if (key == "minWolfSpawnIntervalMs") minWolfSpawnIntervalMs = std::stof(value);
            else if (key == "sheepAddedPerUpgrade") sheepAddedPerUpgrade = std::stof(value);
            else if (key == "sheepSpawnOffsetZ") sheepSpawnOffsetZ = std::stof(value);
        } catch (const std::exception& e) {
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
    
    file.close();
    std::cout << "Config saved to: " << filename << std::endl;
    return true;
}
