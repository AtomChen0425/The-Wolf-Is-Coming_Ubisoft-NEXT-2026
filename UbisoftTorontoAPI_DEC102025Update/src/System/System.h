#pragma once
#include "Math/Vec2.h"
#include "Component/Bullet.h"
#include "Component/Enemy.h"
#include <vector>
#include "../ContestAPI/app.h"
#include "../ContestAPI/AppSettings.h"
#include "../System/Physic/collision.h"
#include "../System/Render/RenderHelper.h"
class GameSystem
{
public:
    Vec2  gPlayerPos;
    Vec2  gPlayerVel;
    float gPlayerAngle = 0.0f;
    float gPlayerRadius = 14.0f;

    std::vector<Bullet> gBullets;
    std::vector<Enemy>  gEnemies;

    float gShootCooldownMs = 0.0f;
    float gSpawnTimerMs = 0.0f;

    int   gScore = 0;
    bool  gGameOver = false;

    void ResetGame();

    void SpawnEnemy();
	void Update(const float deltaTimeMs);
	void Render();
};