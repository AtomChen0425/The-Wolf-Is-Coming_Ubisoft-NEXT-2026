#pragma once
#include "../Game/Core/Scene/SceneManager.h"
#include "../System/UI/UISystem.h"
#include "../Game/Core/LevelSystem.h"
#include <memory>

// Forward declarations
class EngineSystem;
class EntityManager;
struct Camera3D;

// Start Screen Scene
class StartScene : public IScene {
public:
    StartScene(EngineSystem* engine);
    
    void OnEnter() override;
    void OnExit() override;
    void Update(float deltaTimeMs) override;
    void Render() override;
    std::string GetName() const override { return "StartScene"; }
    
private:
    EngineSystem* engineSystem;
    UIManager uiManager;
};

// Playing Scene
class PlayingScene : public IScene {
public:
    PlayingScene(EngineSystem* engine);
    
    void OnEnter() override;
    void OnExit() override;
    void Update(float deltaTimeMs) override;
    void Render() override;
    void RenderBackground();  // Render background before 3D game content
    std::string GetName() const override { return "PlayingScene"; }
    
private:
    EngineSystem* engineSystem;
    UIManager uiManager;
    int m_lastScore;
    UIText* scoreText;
    UIText* roundText;
    UIText* timeText;
    UIText* sheepText;
    
    // Entity count displays
    UIText* totalEntityText;
    UIText* bulletEntityText;
    UIText* wolfEntityText;
    UIText* sheepEntityText;
    UIText* chunkEntityText;
};

// Game Over Scene
class GameOverScene : public IScene {
public:
    GameOverScene(EngineSystem* engine);
    
    void OnEnter() override;
    void OnExit() override;
    void Update(float deltaTimeMs) override;
    void Render() override;
    std::string GetName() const override { return "GameOverScene"; }
    
    void SetFinalScore(int score) { finalScore = score; }
    
private:
    EngineSystem* engineSystem;
    UIManager uiManager;
    int finalScore;
    UIText* scoreText;  // Keep a pointer to update score dynamically
};

// Settings Scene
class SettingsScene : public IScene {
public:
    SettingsScene(EngineSystem* engine);
    
    void OnEnter() override;
    void OnExit() override;
    void Update(float deltaTimeMs) override;
    void Render() override;
    std::string GetName() const override { return "SettingsScene"; }
    
private:
    EngineSystem* engineSystem;
    UIManager uiManager;
    int selectedOption;  // 0 = Mouse, 1 = Arrow Keys
};

// Upgrade Scene for Roguelike progression
class UpgradeScene : public IScene {
public:
    UpgradeScene(EngineSystem* engine);
    
    void OnEnter() override;
    void OnExit() override;
    void Update(float deltaTimeMs) override;
    void Render() override;
    std::string GetName() const override { return "UpgradeScene"; }
    
private:
    EngineSystem* engineSystem;
    UIManager uiManager;
    int selectedUpgrade;  // Currently selected upgrade (0-2)
    LevelSystem::UpgradeType upgradeOptions[3];  // 3 random upgrades to choose from
    
    // UI element pointers
    UIText* upgradeNames[3];
    UIText* upgradeDescs[3];
    UIText* leftBrackets[3];
    UIText* rightBrackets[3];
};
