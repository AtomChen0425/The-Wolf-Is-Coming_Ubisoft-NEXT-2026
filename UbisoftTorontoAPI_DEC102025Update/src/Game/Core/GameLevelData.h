#pragma once
#include "GameConfig.h"

// GameLevelData: Tracks game statistics and round progression
struct GameLevelData {
    // Time tracking
    float totalGameTimeMs = 0.0f;           // Total time played since game start
    float currentRoundTimeMs = 0.0f;        // Time in current round
    float roundDurationMs;                  // Duration of each round (set from config)
    
    // Round tracking
    int currentRound = 1;                   // Current round number (starts at 1)
    
    // Wolf spawn parameters
    float baseWolfSpawnIntervalMs;          // Initial wolf spawn interval (set from config)
    float currentWolfSpawnIntervalMs;       // Current wolf spawn interval
    float wolfSpawnReductionPerRound;       // How much to reduce spawn interval each round (set from config)
    float minWolfSpawnIntervalMs;           // Minimum wolf spawn interval (set from config)
    
    // Default constructor with hardcoded defaults
    GameLevelData() 
        : roundDurationMs(60000.0f),
          baseWolfSpawnIntervalMs(5000.0f),
          currentWolfSpawnIntervalMs(5000.0f),
          wolfSpawnReductionPerRound(200.0f),
          minWolfSpawnIntervalMs(1000.0f) {}
    
    // Initialize from config (call this after loading config)
    void Initialize(const GameConfig& config) {
        roundDurationMs = config.roundDurationMs;
        baseWolfSpawnIntervalMs = config.baseWolfSpawnIntervalMs;
        currentWolfSpawnIntervalMs = config.baseWolfSpawnIntervalMs;
        wolfSpawnReductionPerRound = config.wolfSpawnReductionPerRound;
        minWolfSpawnIntervalMs = config.minWolfSpawnIntervalMs;
    }
    
    // Reset to initial state
    void Reset() {
        totalGameTimeMs = 0.0f;
        currentRoundTimeMs = 0.0f;
        currentRound = 1;
        currentWolfSpawnIntervalMs = baseWolfSpawnIntervalMs;
    }
    
    // Advance to next round
    void NextRound() {
        currentRound++;
        currentRoundTimeMs = 0.0f;
        
        // Reduce wolf spawn interval (wolves spawn faster each round)
        currentWolfSpawnIntervalMs -= wolfSpawnReductionPerRound;
        if (currentWolfSpawnIntervalMs < minWolfSpawnIntervalMs) {
            currentWolfSpawnIntervalMs = minWolfSpawnIntervalMs;
        }
    }
    
    // Check if current round is complete
    bool IsRoundComplete() const {
        return currentRoundTimeMs >= roundDurationMs;
    }
    
    // Get remaining time in current round (in seconds)
    float GetRemainingRoundTime() const {
        float remaining = roundDurationMs - currentRoundTimeMs;
        return remaining > 0.0f ? remaining / 1000.0f : 0.0f;
    }
};
