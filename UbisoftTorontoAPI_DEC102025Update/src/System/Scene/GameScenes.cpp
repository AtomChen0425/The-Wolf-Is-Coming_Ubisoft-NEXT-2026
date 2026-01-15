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
    uiManager.AddText("Controls: WASD - Move, SPACE - Jump", -150, 0, 0.8f, 0.8f, 0.8f, UIAlignment::MiddleCenter);
    uiManager.AddText("Mouse - Control Camera View (Pitch/Yaw)", -165, 30, 0.8f, 0.8f, 0.8f, UIAlignment::MiddleCenter);
    uiManager.AddText("Arrow Keys - Alternative Camera Rotation", -155, 60, 0.7f, 0.7f, 0.7f, UIAlignment::MiddleCenter);
    uiManager.AddText("Press R anytime to Reset", -100, 100, 0.6f, 0.6f, 0.6f, UIAlignment::MiddleCenter);
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

    // 2. ����������� (ͨ��ֻ��һ��)
    for (EntityID id : playerView) {
        auto& player = playerView.get<PlayerTag>(id);

        // 3. �������Ƿ����仯 (�Ż���ֻ�б仯ʱ�Ÿ��� UI)
        if (player.score != m_lastScore) {
            m_lastScore = player.score;

            // 4. ʹ��ָ���������
            if (scoreText) {
                // ��� text �� public �ģ�
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
