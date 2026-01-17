#include "GameScenes.h"
#include "../System/ECSSystem.h"
#include "../ContestAPI/app.h"
#include "../System/Component/Component.h"
#include "../System/ECS/ECS.h"
#include "../Game/Core/LevelSystem.h"

// StartScene Implementation
StartScene::StartScene(EngineSystem* engine) 
    : engineSystem(engine) {
}

void StartScene::OnEnter() {
    // Clear and setup UI
    uiManager.Clear();
    
    // Title
    uiManager.AddText("3D RUNNER GAME", -80, -100, 1.0f, 1.0f, 1.0f, UIAlignment::MiddleCenter);
    
    // Instructions
    uiManager.AddText("Press SPACE to Start", -100, -50, 1.0f, 1.0f, 0.0f, UIAlignment::MiddleCenter);
    uiManager.AddText("Press S for Settings", -90, -10, 0.9f, 0.9f, 0.0f, UIAlignment::MiddleCenter);
    uiManager.AddText("Controls: WASD - Move, SPACE - Jump", -150, 40, 0.8f, 0.8f, 0.8f, UIAlignment::MiddleCenter);
    uiManager.AddText("Press R anytime to Reset", -100, 80, 0.6f, 0.6f, 0.6f, UIAlignment::MiddleCenter);
}

void StartScene::OnExit() {
    uiManager.Clear();
}

void StartScene::Update(float deltaTimeMs) {
    // No game logic updates on start screen
    // Scene transition is handled by EngineSystem
}

void StartScene::Render() {
    uiManager.Render();
}

// PlayingScene Implementation
PlayingScene::PlayingScene(EngineSystem* engine) 
    : engineSystem(engine), m_lastScore(-1), scoreText(nullptr), 
      roundText(nullptr), timeText(nullptr), sheepText(nullptr) {
}

void PlayingScene::OnEnter() {
    uiManager.Clear();
    // Add UI elements
    scoreText = uiManager.AddText("Score: 0", 10, 10, 1.0f, 1.0f, 1.0f, UIAlignment::TopLeft);
    roundText = uiManager.AddText("Round: 1", 10, 35, 1.0f, 1.0f, 0.5f, UIAlignment::TopLeft);
    timeText = uiManager.AddText("Time: 60s", 10, 60, 1.0f, 1.0f, 0.5f, UIAlignment::TopLeft);
    sheepText = uiManager.AddText("Sheep: 0", 10, 85, 0.5f, 1.0f, 0.5f, UIAlignment::TopLeft);

    m_lastScore = -1;
}

void PlayingScene::OnExit() {
    uiManager.Clear();
}

void PlayingScene::Update(float deltaTimeMs) {
    // Game logic updates are handled by EngineSystem
    EntityManager& registry = engineSystem->GetRegistry();
    GameLevelData& levelData = engineSystem->GetLevelData();
    
    // Update score display
    View<PlayerTag> playerView(registry);
    for (EntityID id : playerView) {
        auto& player = playerView.get<PlayerTag>(id);

        if (player.score != m_lastScore) {
            m_lastScore = player.score;
            if (scoreText) {
                scoreText->SetText("Score: " + std::to_string(m_lastScore));
            }
        }
    }
    
    // Update round display
    if (roundText) {
        roundText->SetText("Round: " + std::to_string(levelData.currentRound));
    }
    
    // Update time display (show remaining time in round)
    if (timeText) {
        int remainingSeconds = static_cast<int>(levelData.GetRemainingRoundTime());
        timeText->SetText("Time: " + std::to_string(remainingSeconds) + "s");
    }
    
    // Update sheep count display
    if (sheepText) {
        int sheepCount = LevelSystem::GetSheepCount(registry);
        sheepText->SetText("Sheep: " + std::to_string(sheepCount));
    }
}

void PlayingScene::Render() {
    // Optional: Render HUD elements like score
    uiManager.Render();

}

// GameOverScene Implementation
GameOverScene::GameOverScene(EngineSystem* engine) 
    : engineSystem(engine), finalScore(0), scoreText(nullptr) {
}

void GameOverScene::OnEnter() {
    uiManager.Clear();
    
    // Get final score from player
    EntityManager& registry = engineSystem->GetRegistry();
    View<PlayerTag> playerView(registry);
    for (EntityID id : playerView) {
        auto& playerTag = playerView.get<PlayerTag>(id);
        finalScore = playerTag.score;
    }
    
    // Title
    uiManager.AddText("GAME OVER", -70, -100, 1.0f, 0.0f, 0.0f, UIAlignment::MiddleCenter);
    
    // Score
    std::string scoreStr = "Score: " + std::to_string(finalScore);
    scoreText = uiManager.AddText(scoreStr, -50, -50, 1.0f, 1.0f, 0.0f, UIAlignment::MiddleCenter);
    
    // Instructions
    uiManager.AddText("Press R to Restart", -90, 50, 1.0f, 1.0f, 1.0f, UIAlignment::MiddleCenter);
}

void GameOverScene::OnExit() {
    uiManager.Clear();
    scoreText = nullptr;
}

void GameOverScene::Update(float deltaTimeMs) {
    // No updates needed on game over screen
    // Scene transition is handled by EngineSystem
}

void GameOverScene::Render() {
    uiManager.Render();
}

// SettingsScene Implementation
SettingsScene::SettingsScene(EngineSystem* engine)
    : engineSystem(engine), selectedOption(0) {
}

void SettingsScene::OnEnter() {
    uiManager.Clear();
    
    // Get current setting
    GameSettings& settings = engineSystem->GetSettings();
    selectedOption = (settings.cameraControlMode == CameraControlMode::Mouse) ? 0 : 1;
    
    // Title
    uiManager.AddText("SETTINGS", -50, -150, 1.0f, 1.0f, 1.0f, UIAlignment::MiddleCenter);
    
    // Camera control options
    uiManager.AddText("Camera Control Mode:", -90, -80, 0.9f, 0.9f, 0.9f, UIAlignment::MiddleCenter);
    uiManager.AddText("Use UP/DOWN arrow keys to select, ENTER to confirm", -200, 100, 0.7f, 0.7f, 0.7f, UIAlignment::MiddleCenter);
    uiManager.AddText("Press ESC to return to main menu", -140, 130, 0.7f, 0.7f, 0.7f, UIAlignment::MiddleCenter);
}

void SettingsScene::OnExit() {
    uiManager.Clear();
}

void SettingsScene::Update(float deltaTimeMs) {
    // Handle UP/DOWN to change selection
    static bool upKeyWasPressed = false;
    static bool downKeyWasPressed = false;
    static bool enterKeyWasPressed = false;
    static bool escKeyWasPressed = false;
    
    bool upKeyPressed = App::IsKeyPressed(App::KEY_UP);
    bool downKeyPressed = App::IsKeyPressed(App::KEY_DOWN);
    bool enterPressed = App::IsKeyPressed(App::KEY_ENTER);
    bool escPressed = App::IsKeyPressed(App::KEY_ESC);
    
    // UP key - select previous option
    if (upKeyPressed && !upKeyWasPressed) {
        selectedOption = (selectedOption == 0) ? 1 : 0;
    }
    
    // DOWN key - select next option
    if (downKeyPressed && !downKeyWasPressed) {
        selectedOption = (selectedOption == 0) ? 1 : 0;
    }
    
    // ENTER - confirm selection
    if (enterPressed && !enterKeyWasPressed) {
        GameSettings& settings = engineSystem->GetSettings();
        settings.cameraControlMode = (selectedOption == 0) ? CameraControlMode::Mouse : CameraControlMode::ArrowKeys;
        
        // Return to start screen
        engineSystem->GetSceneManager().SwitchToScene("StartScreen");
    }
    
    // ESC - return to start screen without changing
    if (escPressed && !escKeyWasPressed) {
        engineSystem->GetSceneManager().SwitchToScene("StartScreen");
    }
    
    upKeyWasPressed = upKeyPressed;
    downKeyWasPressed = downKeyPressed;
    enterKeyWasPressed = enterPressed;
    escKeyWasPressed = escPressed;
}

void SettingsScene::Render() {
    // Render options with highlighting
    float mouseColor = (selectedOption == 0) ? 1.0f : 0.5f;
    float arrowColor = (selectedOption == 1) ? 1.0f : 0.5f;
    
    uiManager.Clear();
    
    // Title
    uiManager.AddText("SETTINGS", -50, -150, 1.0f, 1.0f, 1.0f, UIAlignment::MiddleCenter);
    
    // Camera control options
    uiManager.AddText("Camera Control Mode:", -90, -80, 0.9f, 0.9f, 0.9f, UIAlignment::MiddleCenter);
    
    // Option 1: Mouse
    std::string mouseOption = (selectedOption == 0) ? "> Mouse Control" : "  Mouse Control";
    uiManager.AddText(mouseOption, -80, -20, mouseColor, mouseColor, 0.0f, UIAlignment::MiddleCenter);
    
    // Option 2: Arrow Keys
    std::string arrowOption = (selectedOption == 1) ? "> Arrow Keys Control" : "  Arrow Keys Control";
    uiManager.AddText(arrowOption, -95, 20, arrowColor, arrowColor, 0.0f, UIAlignment::MiddleCenter);
    
    // Instructions
    uiManager.AddText("Use UP/DOWN arrow keys to select, ENTER to confirm", -200, 100, 0.7f, 0.7f, 0.7f, UIAlignment::MiddleCenter);
    uiManager.AddText("Press ESC to return to main menu", -140, 130, 0.7f, 0.7f, 0.7f, UIAlignment::MiddleCenter);
    
    uiManager.Render();
}

// UpgradeScene Implementation
UpgradeScene::UpgradeScene(EngineSystem* engine)
    : engineSystem(engine) {
}

void UpgradeScene::OnEnter() {
    uiManager.Clear();
    
    // Simple placeholder UI - actual upgrade logic will be handled by LevelSystem
    uiManager.AddText("LEVEL UP!", -50, -150, 1.0f, 1.0f, 0.0f, UIAlignment::MiddleCenter);
    uiManager.AddText("Upgrade scene - logic in LevelSystem", -150, -50, 0.9f, 0.9f, 0.9f, UIAlignment::MiddleCenter);
    uiManager.AddText("Press SPACE to continue", -100, 50, 0.7f, 0.7f, 0.7f, UIAlignment::MiddleCenter);
}

void UpgradeScene::OnExit() {
    uiManager.Clear();
}

void UpgradeScene::Update(float deltaTimeMs) {
    // Minimal update - just check for continue input
    static bool spaceWasPressed = false;
    bool spacePressed = App::IsKeyPressed(App::KEY_SPACE);
    
    if (spacePressed && !spaceWasPressed) {
        // Advance to next round (this will eventually be moved to LevelSystem too)
        engineSystem->GetLevelData().NextRound();
        // Return to playing scene
        engineSystem->GetSceneManager().SwitchToScene("Playing");
    }
    
    spaceWasPressed = spacePressed;
}

void UpgradeScene::Render() {
    uiManager.Render();
}
