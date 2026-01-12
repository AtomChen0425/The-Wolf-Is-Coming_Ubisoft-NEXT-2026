#pragma once
#include "Math/Vec2.h"
#include "../ContestAPI/app.h"
#include "../ContestAPI/AppSettings.h"
#include "../System/Physic/collision.h"
#include "../System/Render/RenderHelper.h"
#include "../System/ECS/ECS.h"
#include "../System/Component/Component.h"
class EngineSystem
{
public:
    EngineSystem();
    ~EngineSystem() = default;
    void CreatePlayer(EntityManager& registry);
	void InputSystem(EntityManager& registry);
	void MovementSystem(EntityManager& registry, const float deltaTimeMs);
    void RenderSystem(EntityManager& registry);
	void UpdateAnimation(EntityManager& registry, const float deltaTimeMs);
    void ResetGame();

    //void SpawnEnemy(EntityManager& registry);
    void Update(const float deltaTimeMs);
    void Render();
    void Shutdown();
private:
    std::unique_ptr<EntityManager> registry;
};