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

}

void StartScene::Render() {

    CSimpleSprite* bgSprite = App::CreateSprite("data/TestData/SettingBackground.png", 1, 1);
    bgSprite->SetPosition(512.0f, 384.0f);
    bgSprite->SetScale(1.0f);
    bgSprite->Draw();

    uiManager.Render();
}

// PlayingScene Implementation
PlayingScene::PlayingScene(EngineSystem* engine)
    : engineSystem(engine), m_lastScore(-1), scoreText(nullptr),
    roundText(nullptr), timeText(nullptr), sheepText(nullptr),
    totalEntityText(nullptr), bulletEntityText(nullptr),
    wolfEntityText(nullptr), sheepEntityText(nullptr), chunkEntityText(nullptr) {
}

void PlayingScene::OnEnter() {
    uiManager.Clear();
    // Add UI elements
    scoreText = uiManager.AddText("Health: 100", 10, 10, 1.0f, 1.0f, 1.0f, UIAlignment::TopLeft);
    roundText = uiManager.AddText("Round: 1", 10, 35, 1.0f, 1.0f, 0.5f, UIAlignment::TopLeft);
    timeText = uiManager.AddText("Time: 60s", 10, 60, 1.0f, 1.0f, 0.5f, UIAlignment::TopLeft);
    sheepText = uiManager.AddText("Sheep: 0", 10, 85, 0.5f, 1.0f, 0.5f, UIAlignment::TopLeft);

    // Entity count displays on the right side
    totalEntityText = uiManager.AddText("Total Entities: 0", 150, 10, 0.8f, 0.8f, 0.8f, UIAlignment::TopRight);
    bulletEntityText = uiManager.AddText("Bullets: 0", 100, 35, 0.7f, 0.7f, 1.0f, UIAlignment::TopRight);
    wolfEntityText = uiManager.AddText("Wolves: 0", 100, 60, 1.0f, 0.5f, 0.5f, UIAlignment::TopRight);
    sheepEntityText = uiManager.AddText("Sheep: 0", 100, 85, 0.5f, 1.0f, 0.5f, UIAlignment::TopRight);
    chunkEntityText = uiManager.AddText("Chunks: 0", 100, 110, 0.7f, 0.7f, 0.5f, UIAlignment::TopRight);

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
        auto& player = playerView.get<Health>(id);

        if (player.currentHealth != m_lastScore) {
            m_lastScore = player.currentHealth;
            if (scoreText) {
                scoreText->SetText("Health: " + std::to_string(m_lastScore));
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

    // Count entities by type
    int totalEntities = registry.getAliveEntitiesCount();

    // Count bullets
    int bulletCount = 0;
    View<Bullet> bulletView(registry);
    for (EntityID id : bulletView) {
        bulletCount++;
    }

    // Count wolves
    int wolfCount = 0;
    View<WolfTag> wolfView(registry);
    for (EntityID id : wolfView) {
        wolfCount++;
    }

    // Count sheep entities
    int sheepEntityCount = 0;
    View<SheepTag> sheepView(registry);
    for (EntityID id : sheepView) {
        sheepEntityCount++;
    }

    // Count map blocks/chunks
    int chunkCount = 0;
    View<MapBlockTag> chunkView(registry);
    for (EntityID id : chunkView) {
        chunkCount++;
    }

    // Update entity count displays
    if (totalEntityText) {
        totalEntityText->SetText("Total Entities: " + std::to_string(totalEntities));
    }
    if (bulletEntityText) {
        bulletEntityText->SetText("Bullets: " + std::to_string(bulletCount));
    }
    if (wolfEntityText) {
        wolfEntityText->SetText("Wolves: " + std::to_string(wolfCount));
    }
    if (sheepEntityText) {
        sheepEntityText->SetText("Sheep: " + std::to_string(sheepEntityCount));
    }
    if (chunkEntityText) {
        chunkEntityText->SetText("Chunks: " + std::to_string(chunkCount));
    }
}

void PlayingScene::Render() {
    // Only render UI elements on top of everything
    // Background should be rendered before 3D game content via RenderBackground()
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

    CSimpleSprite* bgSprite = App::CreateSprite("data/TestData/SettingBackground.png", 1, 1);
    bgSprite->SetPosition(512.0f, 384.0f);
    bgSprite->SetScale(1.0f);
    bgSprite->Draw();

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
    for (int i = 0; i < 3; i++) {
        upgradeNames[i] = nullptr;
        upgradeDescs[i] = nullptr;
        leftBrackets[i] = nullptr;
        rightBrackets[i] = nullptr;
    }
}

void UpgradeScene::OnEnter() {
    uiManager.Clear();
    selectedUpgrade = 0;

    // Generate random upgrades using LevelSystem
    LevelSystem::GenerateRandomUpgrades(upgradeOptions);

    // Title
    uiManager.AddText("LEVEL UP!", -50, -150, 1.0f, 1.0f, 0.0f, UIAlignment::MiddleCenter);
    uiManager.AddText("Choose an Upgrade:", -80, -110, 0.9f, 0.9f, 0.9f, UIAlignment::MiddleCenter);

    // Render each option horizontally (3 boxes side by side)
    float spacing = 300.0f;
    float startX = -spacing;

    for (int i = 0; i < 3; i++) {
        float xPos = startX + i * spacing;
        float yPos = -20.0f;

        // Selection brackets (initially hidden for non-selected items)
        float bracketAlpha = (i == selectedUpgrade) ? 1.0f : 0.0f;
        leftBrackets[i] = uiManager.AddText("[", xPos - 130, yPos - 40, bracketAlpha, bracketAlpha, 0.0f, UIAlignment::MiddleCenter);
        rightBrackets[i] = uiManager.AddText("]", xPos + 100, yPos - 40, bracketAlpha, bracketAlpha, 0.0f, UIAlignment::MiddleCenter);

        // Upgrade name
        float brightness = (i == selectedUpgrade) ? 1.0f : 0.5f;
        std::string optionText = LevelSystem::GetUpgradeName(upgradeOptions[i]);
        upgradeNames[i] = uiManager.AddText(optionText, xPos - 80, yPos - 40, brightness, brightness, 0.0f, UIAlignment::MiddleCenter);

        // Description
        std::string descText = LevelSystem::GetUpgradeDescription(upgradeOptions[i], engineSystem->GetGameConfig());
        upgradeDescs[i] = uiManager.AddText(descText, xPos - 90, yPos + 10, 0.7f * brightness, 0.7f * brightness, 0.7f * brightness, UIAlignment::MiddleCenter);
    }

    // Instructions
    uiManager.AddText("Use LEFT/RIGHT to select, ENTER to confirm", -90, 150, 0.2f, 0.2f, 0.2f, UIAlignment::MiddleCenter);
}

void UpgradeScene::OnExit() {
    EntityManager& registry = engineSystem->GetRegistry();
    View<EnemyTag> enemyView(registry); // Preload EnemyTag view for performance
    View<Bullet> bulletView(registry); // Preload Bullet view for performance
    static std::vector<Entity> EnemyToRemove;
    static std::vector<Entity> BulletToRemove;
    for (EntityID id : enemyView) {
        EnemyToRemove.push_back({ id, registry.getEntityVersion(id) });

    }
    for (const Entity& e : EnemyToRemove) {
        registry.destroyEntity(e);
    }

    for (EntityID id : bulletView) {
        BulletToRemove.push_back({ id, registry.getEntityVersion(id) });

    }
    for (const Entity& e : BulletToRemove) {
        registry.destroyEntity(e);
    }

    uiManager.Clear();
}

void UpgradeScene::Update(float deltaTimeMs) {
    // Handle input for upgrade selection
    static bool leftWasPressed = false;
    static bool rightWasPressed = false;
    static bool enterWasPressed = false;

    bool leftPressed = App::GetController().GetLeftThumbStickX() < -0.5f || App::IsKeyPressed(App::KEY_LEFT);
    bool rightPressed = App::GetController().GetLeftThumbStickX() > 0.5f || App::IsKeyPressed(App::KEY_RIGHT);
    bool enterPressed = App::IsKeyPressed(App::KEY_ENTER) || App::IsKeyPressed(App::KEY_SPACE);

    int previousSelection = selectedUpgrade;

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

    // Update UI if selection changed
    if (previousSelection != selectedUpgrade) {
        // Update previous selection (dim it)
        if (upgradeNames[previousSelection]) {
            upgradeNames[previousSelection]->SetColor(0.5f, 0.5f, 0.0f);
        }
        if (upgradeDescs[previousSelection]) {
            upgradeDescs[previousSelection]->SetColor(0.35f, 0.35f, 0.35f);
        }
        if (leftBrackets[previousSelection]) {
            leftBrackets[previousSelection]->SetColor(0.0f, 0.0f, 0.0f);
        }
        if (rightBrackets[previousSelection]) {
            rightBrackets[previousSelection]->SetColor(0.0f, 0.0f, 0.0f);
        }

        // Update new selection (brighten it)
        if (upgradeNames[selectedUpgrade]) {
            upgradeNames[selectedUpgrade]->SetColor(1.0f, 1.0f, 0.0f);
        }
        if (upgradeDescs[selectedUpgrade]) {
            upgradeDescs[selectedUpgrade]->SetColor(0.7f, 0.7f, 0.7f);
        }
        if (leftBrackets[selectedUpgrade]) {
            leftBrackets[selectedUpgrade]->SetColor(1.0f, 1.0f, 0.0f);
        }
        if (rightBrackets[selectedUpgrade]) {
            rightBrackets[selectedUpgrade]->SetColor(1.0f, 1.0f, 0.0f);
        }
    }

    // Confirm selection
    if (enterPressed && !enterWasPressed) {
        // Apply upgrade using LevelSystem
        LevelSystem::ApplyUpgrade(engineSystem->GetRegistry(), engineSystem->GetGameConfig(), upgradeOptions[selectedUpgrade]);
        // Advance to next round
        engineSystem->GetLevelData().NextRound();
        // Clear UI before switching
        uiManager.Clear();
        // Return to playing scene
        engineSystem->GetSceneManager().SwitchToScene("Playing");
        engineSystem->SetGameState(GameState::Playing);
    }

    leftWasPressed = leftPressed;
    rightWasPressed = rightPressed;
    enterWasPressed = enterPressed;
}

void UpgradeScene::Render() {
    CSimpleSprite* bgSprite = App::CreateSprite("data/TestData/SettingBackground.png", 1, 1);
    bgSprite->SetPosition(512.0f, 384.0f);
    bgSprite->SetScale(1.0f);
    bgSprite->Draw();

    uiManager.Render();
}
