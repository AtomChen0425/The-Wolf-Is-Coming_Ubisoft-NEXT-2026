#include "GameScenes.h"
#include "../ECSSystem.h"
#include "../../ContestAPI/app.h"
#include "../Component/Component.h"
#include "../ECS/ECS.h"

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
    : engineSystem(engine), m_lastScore(-1), scoreText(nullptr) {
}

void PlayingScene::OnEnter() {
    uiManager.Clear();
    // No persistent UI in playing scene (could add score display here)
    scoreText = uiManager.AddText("Score: 0", 10, 10, 1.0f, 1.0f, 1.0f, UIAlignment::TopLeft);

    // ���û���ķ���
    m_lastScore = -1;
    
}

void PlayingScene::OnExit() {
    uiManager.Clear();
}

void PlayingScene::Update(float deltaTimeMs) {
    // Game logic updates are handled by EngineSystem
    EntityManager& registry = engineSystem->GetRegistry();
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
    : engineSystem(engine), selectedUpgrade(0) {
}

void UpgradeScene::OnEnter() {
    uiManager.Clear();
    selectedUpgrade = 0;
    GenerateRandomUpgrades();
    
    // Title
    uiManager.AddText("LEVEL UP!", -50, -150, 1.0f, 1.0f, 0.0f, UIAlignment::MiddleCenter);
    uiManager.AddText("Choose an Upgrade:", -80, -110, 0.9f, 0.9f, 0.9f, UIAlignment::MiddleCenter);
}

void UpgradeScene::OnExit() {
    uiManager.Clear();
}

void UpgradeScene::Update(float deltaTimeMs) {
    // Handle input for upgrade selection
    static bool leftWasPressed = false;
    static bool rightWasPressed = false;
    static bool enterWasPressed = false;
    
    bool leftPressed = App::GetController().GetLeftThumbStickX() < -0.5f || App::IsKeyPressed(VK_LEFT);
    bool rightPressed = App::GetController().GetLeftThumbStickX() > 0.5f || App::IsKeyPressed(VK_RIGHT);
    bool enterPressed = App::IsKeyPressed(VK_RETURN) || App::IsKeyPressed(VK_SPACE);
    
    // Navigate left
    if (leftPressed && !leftWasPressed) {
        selectedUpgrade--;
        if (selectedUpgrade < 0) selectedUpgrade = 2;
    }
    
    // Navigate right
    if (rightPressed && !rightWasPressed) {
        selectedUpgrade++;
        if (selectedUpgrade > 2) selectedUpgrade = 0;
    }
    
    // Confirm selection
    if (enterPressed && !enterWasPressed) {
        ApplyUpgrade(upgradeOptions[selectedUpgrade]);
        // Return to playing scene
        engineSystem->GetSceneManager().SwitchToScene("PlayingScene");
    }
    
    leftWasPressed = leftPressed;
    rightWasPressed = rightPressed;
    enterWasPressed = enterPressed;
}

void UpgradeScene::Render() {
    // Render the 3 upgrade options horizontally
    uiManager.Clear();
    
    // Title
    uiManager.AddText("LEVEL UP!", -50, -150, 1.0f, 1.0f, 0.0f, UIAlignment::MiddleCenter);
    uiManager.AddText("Choose an Upgrade:", -80, -110, 0.9f, 0.9f, 0.9f, UIAlignment::MiddleCenter);
    
    // Render each option horizontally (3 boxes side by side)
    float spacing = 200.0f; // Space between options
    float startX = -spacing; // Center the 3 options
    
    for (int i = 0; i < 3; i++) {
        float xPos = startX + i * spacing;
        float yPos = -20.0f; // Center vertically
        
        // Highlight selected option
        float brightness = (i == selectedUpgrade) ? 1.0f : 0.5f;
        
        // Draw selection indicator (box-like with brackets)
        if (i == selectedUpgrade) {
            uiManager.AddText("[", xPos - 100, yPos - 40, 1.0f, 1.0f, 0.0f, UIAlignment::MiddleCenter);
            uiManager.AddText("]", xPos + 80, yPos - 40, 1.0f, 1.0f, 0.0f, UIAlignment::MiddleCenter);
        }
        
        // Upgrade name (larger, more prominent)
        std::string optionText = GetUpgradeName(upgradeOptions[i]);
        uiManager.AddText(optionText, xPos - 60, yPos - 40, brightness, brightness, 0.0f, UIAlignment::MiddleCenter);
        
        // Description (smaller, below name)
        std::string descText = GetUpgradeDescription(upgradeOptions[i]);
        uiManager.AddText(descText, xPos - 80, yPos + 10, 0.7f * brightness, 0.7f * brightness, 0.7f * brightness, UIAlignment::MiddleCenter);
    }
    
    // Instructions
    uiManager.AddText("Use LEFT/RIGHT to select, ENTER to confirm", -180, 150, 0.7f, 0.7f, 0.7f, UIAlignment::MiddleCenter);
    
    uiManager.Render();
}

void UpgradeScene::GenerateRandomUpgrades() {
    // Generate 3 random unique upgrades
    std::vector<UpgradeType> allUpgrades = {
        UpgradeType::HealthBoost,
        UpgradeType::SpeedBoost,
        UpgradeType::JumpBoost,
        UpgradeType::GravityReduction,
        UpgradeType::BulletSpeed
    };
    
    // Simple shuffle for 3 picks
    for (int i = 0; i < 3; i++) {
        int randomIndex = i + (int)(Rand01() * (allUpgrades.size() - i));
        upgradeOptions[i] = allUpgrades[randomIndex];
        // Swap to avoid duplicates
        std::swap(allUpgrades[i], allUpgrades[randomIndex]);
    }
}

void UpgradeScene::ApplyUpgrade(UpgradeType type) {
    // Find player and apply upgrade
    View<PlayerTag, PlayerStats> view(*engineSystem->GetEntityManager());
    
    for (EntityID id : view) {
        auto& stats = view.get<PlayerStats>(id);
        auto& config = engineSystem->GetGameConfig();
        
        switch (type) {
            case UpgradeType::HealthBoost:
                stats.healthBonus += config.healthUpgradeAmount;
                // Also heal player
                if (view.has<Health>(id)) {
                    auto& health = view.get<Health>(id);
                    health.maxHealth += (int)config.healthUpgradeAmount;
                    health.currentHealth += (int)config.healthUpgradeAmount;
                }
                break;
                
            case UpgradeType::SpeedBoost:
                stats.speedBonus += config.speedUpgradeAmount;
                break;
                
            case UpgradeType::JumpBoost:
                stats.jumpBonus += config.jumpUpgradeAmount;
                break;
                
            case UpgradeType::GravityReduction:
                stats.gravityBonus += config.gravityUpgradeAmount;
                break;
                
            case UpgradeType::BulletSpeed:
                stats.bulletSpeedBonus += config.bulletSpeedUpgradeAmount;
                break;
        }
    }
}

std::string UpgradeScene::GetUpgradeName(UpgradeType type) {
    switch (type) {
        case UpgradeType::HealthBoost: return "Health Boost";
        case UpgradeType::SpeedBoost: return "Speed Boost";
        case UpgradeType::JumpBoost: return "Jump Boost";
        case UpgradeType::GravityReduction: return "Gravity Reduction";
        case UpgradeType::BulletSpeed: return "Bullet Speed";
        default: return "Unknown";
    }
}

std::string UpgradeScene::GetUpgradeDescription(UpgradeType type) {
    switch (type) {
        case UpgradeType::HealthBoost: return "Increase max health +20";
        case UpgradeType::SpeedBoost: return "Movement speed +50";
        case UpgradeType::JumpBoost: return "Jump velocity +100";
        case UpgradeType::GravityReduction: return "Lighter jumps (gravity -100)";
        case UpgradeType::BulletSpeed: return "Bullet speed +100";
        default: return "Unknown effect";
    }
}
