#pragma once
#include "SceneManager.h"
#include "../UI/UISystem.h"
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
    std::string GetName() const override { return "PlayingScene"; }
    
private:
    EngineSystem* engineSystem;
    UIManager uiManager;
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
