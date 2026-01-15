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
#include "../ContestAPI/app.h"
#include "Component/Component.h"

EngineSystem::EngineSystem()
    : registry(std::make_unique<EntityManager>()), gameState(GameState::StartScreen)
{
}

float Lerp(float a, float b, float t) {
    return a + (b - a) * t;
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
}

void EngineSystem::ResetGame() {
    // Reset to start screen
    InitializeGame();
    gameState = GameState::StartScreen;
}

void EngineSystem::Update(const float deltaTimeMs) {
    if (!registry) return;
    
    // Handle input based on game state
    if (gameState == GameState::StartScreen) {
        // Check for space or enter to start
        if (App::IsKeyPressed(App::KEY_SPACE) || App::IsKeyPressed(App::KEY_ENTER)) {
            StartGame();
        }
        // No physics/collision updates on start screen
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
        
        // Update player control (handles input and movement)
        ControlSystem::Update(*registry, deltaTimeMs, nextSpawnZ);
        
        // Check and resolve collisions (after movement is applied)
        CollisionSystem::Update(*registry);

        PhysicsSystem::Update(*registry, deltaTimeMs);
        // Check for game over conditions (player fell off the world)
        View<PlayerTag, Transform3D> playerView(*registry);
        for (EntityID id : playerView) {
            auto& playerTransform = playerView.get<Transform3D>(id);
            if (playerTransform.pos.y < -500.0f) {
                gameState = GameState::GameOver;
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
    
    // Render UI based on game state
    if (gameState == GameState::StartScreen) {
        App::Print(350, 300, "3D RUNNER GAME", 1.0f, 1.0f, 1.0f);
        App::Print(300, 350, "Press SPACE to Start", 1.0f, 1.0f, 0.0f);
        App::Print(250, 400, "Controls: WASD - Move, SPACE - Jump", 0.8f, 0.8f, 0.8f);
        App::Print(250, 430, "Arrow Keys - Rotate Camera", 0.8f, 0.8f, 0.8f);
        App::Print(300, 480, "Press R anytime to Reset", 0.6f, 0.6f, 0.6f);
    } else if (gameState == GameState::GameOver) {
        View<PlayerTag> playerView(*registry);
        for (EntityID id : playerView) {
            auto& playerTag = playerView.get<PlayerTag>(id);
            App::Print(350, 350, ("Score: " + std::to_string(playerTag.score)).c_str(), 1.0f, 1.0f, 0.0f);
        }
        App::Print(380, 300, "GAME OVER", 1.0f, 0.0f, 0.0f);
        
        App::Print(300, 400, "Press R to Restart", 1.0f, 1.0f, 1.0f);
    }
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
