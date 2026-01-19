#pragma once
#include "GameConfig.h"

// GameLevelData: Tracks game statistics and round progression
// Default values match those in GameConfig for consistency
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

    float sniperWolfSpawnIntervalMs;		// Sniper wolf spawn interval (set from config)
    float tankWolfSpawnIntervalMs;		// Tank wolf spawn interval (set from config)
    float fastWolfSpawnIntervalMs;		// Fast wolf spawn interval (set from config)
    float hunterWolfSpawnIntervalMs;		// Hunter wolf spawn interval (set from config)
    // Default constructor - must call Initialize() before use
    GameLevelData()
        : roundDurationMs(0.0f),
        baseWolfSpawnIntervalMs(0.0f),
        currentWolfSpawnIntervalMs(0.0f),
        wolfSpawnReductionPerRound(0.0f),
        minWolfSpawnIntervalMs(0.0f),
        sniperWolfSpawnIntervalMs(0.0f),
        tankWolfSpawnIntervalMs(0.0f),
        fastWolfSpawnIntervalMs(0.0f),
        hunterWolfSpawnIntervalMs(0.0f) {
    }

    // Initialize from config (must be called after construction)
    void Initialize() {
        roundDurationMs = config.roundDurationMs;
        baseWolfSpawnIntervalMs = config.baseWolfSpawnIntervalMs;
        currentWolfSpawnIntervalMs = config.baseWolfSpawnIntervalMs;
        wolfSpawnReductionPerRound = config.wolfSpawnReductionPerRound;
        minWolfSpawnIntervalMs = config.minWolfSpawnIntervalMs;
        sniperWolfSpawnIntervalMs = config.sniperWolfSpawnIntervalMs;
        tankWolfSpawnIntervalMs = config.tankWolfSpawnIntervalMs;
        fastWolfSpawnIntervalMs = config.fastWolfSpawnIntervalMs;
        hunterWolfSpawnIntervalMs = config.hunterWolfSpawnIntervalMs;
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
        if (currentRound > 10) {
            sniperWolfSpawnIntervalMs *= 0.9f;
			sniperWolfSpawnIntervalMs = std::max(minWolfSpawnIntervalMs, sniperWolfSpawnIntervalMs);
        }
        if (currentRound > 12) {
            tankWolfSpawnIntervalMs *= 0.9f;
            tankWolfSpawnIntervalMs = std::max(minWolfSpawnIntervalMs, tankWolfSpawnIntervalMs);
        }
        if (currentRound > 15) {
            fastWolfSpawnIntervalMs *= 0.9f;
            fastWolfSpawnIntervalMs = std::max(minWolfSpawnIntervalMs, fastWolfSpawnIntervalMs);
        }
        if (currentRound > 18) {
            hunterWolfSpawnIntervalMs *= 0.9f;
            hunterWolfSpawnIntervalMs = std::max(minWolfSpawnIntervalMs, hunterWolfSpawnIntervalMs);
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
