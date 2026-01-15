#include "ECSSystem.h"
#include "Math/Vec2.h"
#include <cmath>
#include "../Game/Core/RenderSystem.h"
#include "../Game/Core/MovementSystem.h"
#include "../Game/Core/GenerateSystem.h"
#include "../Game/Core/ControlSystem.h"
#include "../Game/Core/CollisionSystem.h"
#include "../Game/Core/CameraSystem.h"
#include "../Game/Core/PhysicsSystem.h"
#include "../Game/Core/EnemyAISystem.h"
#include "../Game/Core/ParticleSystem.h"
#include "../ContestAPI/app.h"
#include "Component/Component.h"
#include "Scene/GameScenes.h"

EngineSystem::EngineSystem()
    : registry(std::make_unique<EntityManager>()), gameState(GameState::StartScreen)
{
    InitializeScenes();
}

float Lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

void EngineSystem::InitializeScenes() {
    // Register all game scenes
    sceneManager.RegisterScene("StartScreen", std::make_unique<StartScene>(this));
    sceneManager.RegisterScene("Playing", std::make_unique<PlayingScene>(this));
    sceneManager.RegisterScene("GameOver", std::make_unique<GameOverScene>(this));
    sceneManager.RegisterScene("Settings", std::make_unique<SettingsScene>(this));
    
    // Start with the start screen
    sceneManager.SwitchToScene("StartScreen");
}

void EngineSystem::InitializeGame() {
    // Initialize the game world without starting gameplay
    registry = std::make_unique<EntityManager>();
    gSpawnTimerMs = 0.0f;
    gScore = 0;
    nextSpawnZ = 0.0f;

    // Initialize camera position and offsets for 3D view
    camera.followOffsetX = 0.0f;
    camera.followOffsetY = 200.0f;
    camera.followOffsetZ = -400.0f;
    camera.x = 0.0f;
    camera.y = camera.followOffsetY;
    camera.z = camera.followOffsetZ;

    // Create the player
    GenerateSystem::CreatePlayer3D(*registry);

    // Generate initial map
    GenerateSystem::MapGenerationSystem(*registry, 0.0f, nextSpawnZ);
}

void EngineSystem::StartGame() {
    // Start the game - this is called when player presses a key on start screen
    gameState = GameState::Playing;
    sceneManager.SwitchToScene("Playing");
}

void EngineSystem::ResetGame() {
    // Reset to start screen
    InitializeGame();
    gameState = GameState::StartScreen;
    sceneManager.SwitchToScene("StartScreen");
}

void EngineSystem::Update(const float deltaTimeMs) {
    if (!registry) return;
    sceneManager.Update(deltaTimeMs);
    // Handle input based on game state
    if (gameState == GameState::StartScreen) {
        // Check for space or enter to start
        if (App::IsKeyPressed(App::KEY_SPACE) || App::IsKeyPressed(App::KEY_ENTER)) {
            StartGame();
        }
        // Check for S to go to settings
        static bool sKeyWasPressed = false;
        bool sKeyPressed = App::IsKeyPressed(App::KEY_S);
        if (sKeyPressed && !sKeyWasPressed) {
            gameState = GameState::Settings;
            sceneManager.SwitchToScene("Settings");
        }
        sKeyWasPressed = sKeyPressed;
        // No physics/collision updates on start screen
        return;
    } else if (gameState == GameState::Settings) {
        // Settings scene handles its own input
        // Check if we returned to start screen
        if (sceneManager.GetCurrentSceneName() == "StartScreen") {
            gameState = GameState::StartScreen;
        }
        return;
    } else if (gameState == GameState::GameOver) {
        // Check for R to reset
        if (App::IsKeyPressed(App::KEY_R)) {
            ResetGame();
        }
        // No physics/collision updates on game over screen
        return;
    }
    
    // Playing state - normal game updates
    if (gameState == GameState::Playing) {
        // Check for R to reset even during gameplay
        if (App::IsKeyPressed(App::KEY_R)) {
            ResetGame();
            return;
        }
        
        // Update player control (handles input and movement, and camera control)
        ControlSystem::Update(*registry, deltaTimeMs, nextSpawnZ, camera, settings);
        // Update enemy AI (movement, shooting, bullets)
        //EnemyAISystem::Update(*registry, deltaTimeMs);
        
        // Check and resolve collisions (after movement is applied)
        CollisionSystem::Update(*registry);
        MovementSystem::Update(*registry, deltaTimeMs);
        PhysicsSystem::Update(*registry, deltaTimeMs);
        ParticleSystem::Update(*registry, deltaTimeMs);
        // Check for game over conditions (player fell off the world)
        View<PlayerTag, Transform3D> playerView(*registry);
        for (EntityID id : playerView) {
            auto& playerTransform = playerView.get<Transform3D>(id);
            if (playerTransform.pos.y < -500.0f) {
                gameState = GameState::GameOver;
                sceneManager.SwitchToScene("GameOver");
            }
        }
    }
}

void EngineSystem::Render() {
    if (!registry) return;
    
    // Update camera position for all states to ensure proper view
    CameraSystem::Update(*registry, camera);
    
    // Render the 3D scene with camera
    RenderSystem::Render(*registry, camera);
    
    // Render UI using Scene Manager
    sceneManager.Render();
}

void EngineSystem::Shutdown() {
    // Clean up resources
    View<SpriteComponent> view(*registry);
    for (EntityID id : view) {
        auto& spr = view.get<SpriteComponent>(id);
        delete spr.sprite;
    }
    registry.reset();
}
