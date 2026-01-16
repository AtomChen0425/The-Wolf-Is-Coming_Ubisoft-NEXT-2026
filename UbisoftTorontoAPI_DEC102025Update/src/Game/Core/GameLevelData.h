#pragma once

// GameLevelData: Tracks game statistics and round progression
struct GameLevelData {
    // Time tracking
    float totalGameTimeMs = 0.0f;           // Total time played since game start
    float currentRoundTimeMs = 0.0f;        // Time in current round
    float roundDurationMs = 60000.0f;       // Duration of each round (60 seconds = 1 minute)
    
    // Round tracking
    int currentRound = 1;                   // Current round number (starts at 1)
    
    // Wolf spawn parameters
    float baseWolfSpawnIntervalMs = 5000.0f;    // Initial wolf spawn interval (5 seconds)
    float currentWolfSpawnIntervalMs = 5000.0f; // Current wolf spawn interval
    float wolfSpawnReductionPerRound = 200.0f;  // How much to reduce spawn interval each round
    float minWolfSpawnIntervalMs = 1000.0f;     // Minimum wolf spawn interval (1 second)
    
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
