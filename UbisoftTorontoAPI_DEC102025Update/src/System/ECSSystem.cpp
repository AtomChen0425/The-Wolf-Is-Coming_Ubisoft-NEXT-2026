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
    // Load game configuration from file
    if (!config.LoadFromFile("game_config.txt")) {
        // If loading fails, save default config for next time
        config.SaveToFile("game_config.txt");
    }
    
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
    sceneManager.RegisterScene("UpgradeScene", std::make_unique<UpgradeScene>(this));  // Add upgrade scene
    
    // Start with the start screen
    sceneManager.SwitchToScene("StartScreen");
}

void EngineSystem::InitializeGame() {
    // Initialize the game world without starting gameplay
    registry = std::make_unique<EntityManager>();
    gSpawnTimerMs = 0.0f;
    gScore = 0;
    nextSpawnZ = 0.0f;
    loadedChunks.clear();  // Clear loaded chunks

    // Initialize camera position and offsets for 3D view
    camera.followOffsetX = 0.0f;
    camera.followOffsetY = 200.0f;
    camera.followOffsetZ = -400.0f;
    camera.x = 0.0f;
    camera.y = camera.followOffsetY;
    camera.z = camera.followOffsetZ;

    // Create the player using config values
    GenerateSystem::CreatePlayer3D(*registry, config);

    // Generate initial chunks around spawn point using chunk-based system
    GenerateSystem::ChunkGenerationSystem(*registry, config.playerSpawnX, config.playerSpawnZ, loadedChunks, config);
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
        ControlSystem::Update(*registry, deltaTimeMs, nextSpawnZ, camera, settings, config);
        // Update enemy AI (movement, shooting, bullets)
        //EnemyAISystem::Update(*registry, deltaTimeMs);
        
        // Check and resolve collisions (after movement is applied)
        PhysicsSystem::Update(*registry, deltaTimeMs);
        
        MovementSystem::Update(*registry, deltaTimeMs);
        ParticleSystem::Update(*registry, deltaTimeMs);
        CollisionSystem::Update(*registry);
        
        // Generate chunks based on player position
        View<PlayerTag, Transform3D> playerView(*registry);
        for (EntityID id : playerView) {
            auto& playerTransform = playerView.get<Transform3D>(id);
            GenerateSystem::ChunkGenerationSystem(*registry, playerTransform.pos.x, playerTransform.pos.z, loadedChunks, config);
            
            // Check if player fell off the world
            if (playerTransform.pos.y < -500.0f) {
                gameState = GameState::GameOver;
                sceneManager.SwitchToScene("GameOver");
                return;
            }
        }
        
        // Check for upgrade point collection
        View<UpgradePointTag, Transform3D> upgradeView(*registry);
        std::vector<EntityID> collectedUpgrades;
        
        for (EntityID upgradeId : upgradeView) {
            auto& upgradeTag = upgradeView.get<UpgradePointTag>(upgradeId);
            auto& upgradeTransform = upgradeView.get<Transform3D>(upgradeId);
            
            if (!upgradeTag.collected) {
                // Check collision with player
                for (EntityID playerId : playerView) {
                    auto& playerTransform = playerView.get<Transform3D>(playerId);
                    
                    float dx = playerTransform.pos.x - upgradeTransform.pos.x;
                    float dy = playerTransform.pos.y - upgradeTransform.pos.y;
                    float dz = playerTransform.pos.z - upgradeTransform.pos.z;
                    float distSq = dx*dx + dy*dy + dz*dz;
                    
                    if (distSq < 50.0f * 50.0f) {  // Collection radius
                        upgradeTag.collected = true;
                        collectedUpgrades.push_back(upgradeId);
                        
                        // Switch to upgrade scene
                        sceneManager.SwitchToScene("UpgradeScene");
                        break;
                    }
                }
            }
        }
        
        // Remove collected upgrade points
        for (EntityID id : collectedUpgrades) {
            registry->destroyEntity(id);
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
