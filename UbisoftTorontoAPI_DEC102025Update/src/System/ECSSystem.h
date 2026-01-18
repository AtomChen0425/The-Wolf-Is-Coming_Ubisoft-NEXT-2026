#pragma once
#include "ECS/ECS.h"
#include "../Game/Core/Scene/SceneManager.h"
#include "../Game/Core/GameConfig.h"
#include "../Game/Core/GameLevelData.h"
#include <memory>
#include <set>
#include <utility>
//#include <unordered_set>

struct Camera25D {
    float x; // World X coordinate
    float y; // World Z coordinate (note: maps to z)

    // Viewport size
    float width = 1024.0f;
    float height = 768.0f;

    // World boundaries
    float worldMinX = 0.0f;
    float worldMaxX = 2000.0f;
    float worldMinZ = 0.0f;
    float worldMaxZ = 2000.0f;
};

struct Camera3D {
    float x, y, z; // Camera position in world space

    // Fixed offset for following target (in camera space)
    float followOffsetY = 0.0f;
    float followOffsetZ = -700.0f;
    float followOffsetX = 0.0f;

    // Camera rotation angle in radians (rotation around Y axis - yaw)
    float yawAngle = 0.0f;

    // Camera pitch angle in radians (up/down viewing angle)
    float pitchAngle = 0.0f;

    // Mouse control state
    float lastMouseX = 0.0f;
    float lastMouseY = 0.0f;
    bool mouseInitialized = false;
    float screenWidth = 1024.0f;
    float screenHeight = 768.0f;

    float fov = 600.0f;
};
enum class GameState {
    StartScreen,
    Playing,
    Upgrading,
    GameOver,
    Settings
};

enum class CameraControlMode {
    Mouse,
    ArrowKeys
};

struct GenerationTimer
{
    float gSpawnTimerMs = 0.0f;
    float sniperWolfSpawnTimer = 0.0f;
    float tankWolfSpawnTimer = 0.0f;
    float fastWolfSpawnTimer = 0.0f;
    float hunterWolfSpawnTimer = 0.0f;
};

struct GameSettings {
    CameraControlMode cameraControlMode = CameraControlMode::Mouse;
};

class EngineSystem
{
public:
    EngineSystem();
    ~EngineSystem() = default;
    float gTimerMs = 0.0f;

    float gSpawnIntervalMs = 2000.0f;
    int   gScore = 0;

    // Spawn timers for different wolf types

    GenerationTimer generationTimers;
    void Update(const float deltaTimeMs);
    void Render();
    void Shutdown();
    void ResetGame();
    void InitializeGame();  // Initialize world without starting gameplay
    void StartGame();       // Start gameplay

    GameState GetGameState() const { return gameState; }
    SceneManager& GetSceneManager() { return sceneManager; }
    EntityManager& GetRegistry() { return *registry; }
    EntityManager* GetEntityManager() { return registry.get(); }  // Add this for UpgradeScene
    Camera3D& GetCamera() { return camera; }
    GameSettings& GetSettings() { return settings; }
    GameLevelData& GetLevelData() { return levelData; }  // Access to level/round data
    void SetGameState(GameState newState);
private:
    std::unique_ptr<EntityManager> registry;
    Camera3D camera;
    float nextSpawnZ = 0.0f;
    std::set<std::pair<int, int>> loadedChunks;  // Track loaded map chunks
    GameState gameState = GameState::StartScreen;
    SceneManager sceneManager;
    GameSettings settings;
    GameLevelData levelData;  // Track level progression and round data

    void InitializeScenes();
};