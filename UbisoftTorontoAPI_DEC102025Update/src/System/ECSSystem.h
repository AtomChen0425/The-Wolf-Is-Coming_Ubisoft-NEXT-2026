#pragma once
#include "ECS/ECS.h"
#include "Scene/SceneManager.h"
#include <memory>

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
    // For example: 200 units up, 300 units back
    float followOffsetY = 0.0f;
    float followOffsetZ = -700.0f;
    float followOffsetX = 0.0f;
    
    // Camera rotation angle in radians (rotation around Y axis - yaw)
    // 0 = looking along +Z, PI/2 = looking along +X
    float rotationAngle = 0.0f;
    
    // Camera pitch angle in radians (up/down viewing angle)
    // 0 = looking straight ahead, positive = looking up, negative = looking down
    float pitchAngle = 0.0f;
    
    // Mouse control state
    float lastMouseX = 0.0f;
    float lastMouseY = 0.0f;
    bool mouseInitialized = false;
};

enum class GameState {
    StartScreen,
    Playing,
    GameOver,
    Settings
};

enum class CameraControlMode {
    Mouse,
    ArrowKeys
};

struct GameSettings {
    CameraControlMode cameraControlMode = CameraControlMode::Mouse;
};

class EngineSystem
{
public:
    EngineSystem();
    ~EngineSystem() = default;
    
    float gSpawnTimerMs = 0.0f;
    int   gScore = 0;
    
    void Update(const float deltaTimeMs);
    void Render();
    void Shutdown();
    void ResetGame();
    void InitializeGame();  // Initialize world without starting gameplay
    void StartGame();       // Start gameplay
    
    GameState GetGameState() const { return gameState; }
    SceneManager& GetSceneManager() { return sceneManager; }
    EntityManager& GetRegistry() { return *registry; }
    Camera3D& GetCamera() { return camera; }
    GameSettings& GetSettings() { return settings; }
    
private:
    std::unique_ptr<EntityManager> registry;
    Camera3D camera;
    float nextSpawnZ = 0.0f;
    GameState gameState = GameState::StartScreen;
    SceneManager sceneManager;
    GameSettings settings;
    
    void InitializeScenes();
};