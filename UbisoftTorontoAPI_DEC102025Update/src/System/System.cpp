#include "System.h"
#include <algorithm>
#include <cmath>

Collision* gCollision;
RenderHelper* gRenderHelper;
void GameSystem::ResetGame()
{
    gPlayerPos = { APP_VIRTUAL_WIDTH * 0.5f, APP_VIRTUAL_HEIGHT * 0.5f };
    gPlayerVel = { 0, 0 };
    gPlayerAngle = 0.0f;
    gBullets.clear();
    gEnemies.clear();
    gShootCooldownMs = 0.0f;
    gSpawnTimerMs = 0.0f;
    gScore = 0;
    gGameOver = false;
}

void GameSystem::SpawnEnemy()
{
    Enemy e;
    float side = Rand01();
    if (side < 0.25f) { e.pos = { 0.0f, RandRange(0, APP_VIRTUAL_HEIGHT) }; }
    else if (side < 0.5f) { e.pos = { APP_VIRTUAL_WIDTH, RandRange(0, APP_VIRTUAL_HEIGHT) }; }
    else if (side < 0.75f) { e.pos = { RandRange(0, APP_VIRTUAL_WIDTH), 0.0f }; }
    else { e.pos = { RandRange(0, APP_VIRTUAL_WIDTH), APP_VIRTUAL_HEIGHT }; }

    if (LenSq(e.pos - gPlayerPos) < 200.0f * 200.0f)
        e.pos = e.pos + Normalize(e.pos - gPlayerPos) * 220.0f;

    gEnemies.push_back(e);
}
void GameSystem::Update(const float deltaTimeMs)
{
    const float dt = deltaTimeMs; 
    const float dtSec = deltaTimeMs / 1000.0f;

    if (App::IsKeyPressed(App::KEY_R))
    {
        ResetGame();
        return;
    }

    if (gGameOver)
        return;


    Vec2 move;
    move.x = App::GetController().GetLeftThumbStickX();
    move.y = App::GetController().GetLeftThumbStickY();

    const float accel = 1400.0f;     // px/s^2
    const float damping = 6.5f;      // velocity damping

    gPlayerVel.x += move.x * accel * dtSec;
    gPlayerVel.y += move.y * accel * dtSec;

    gPlayerVel.x -= gPlayerVel.x * damping * dtSec;
    gPlayerVel.y -= gPlayerVel.y * damping * dtSec;

    gPlayerPos = gPlayerPos + gPlayerVel * dtSec;

    gPlayerPos.x = Clamp(gPlayerPos.x, gPlayerRadius, APP_VIRTUAL_WIDTH - gPlayerRadius);
    gPlayerPos.y = Clamp(gPlayerPos.y, gPlayerRadius, APP_VIRTUAL_HEIGHT - gPlayerRadius);

    if (LenSq(move) > 0.15f * 0.15f)
        gPlayerAngle = std::atan2(move.y, move.x);

    // ----- Shooting (Space) -----
    gShootCooldownMs = std::fmax(0.0f, gShootCooldownMs - dt);

    const bool shoot = App::IsKeyPressed(App::KEY_SPACE); 
    if (shoot && gShootCooldownMs <= 0.0f)
    {
        Vec2 dir = { std::cos(gPlayerAngle), std::sin(gPlayerAngle) };
        Bullet b;
        b.pos = gPlayerPos + dir * (gPlayerRadius + 6.0f);
        b.vel = dir * 720.0f; // px/s
        gBullets.push_back(b);
        gShootCooldownMs = 120.0f;
    }

    // ----- Spawn enemies -----
    gSpawnTimerMs += dt;
    float spawnInterval = std::fmax(280.0f, 1200.0f - gScore * 20.0f);
    if (gSpawnTimerMs >= spawnInterval)
    {
        gSpawnTimerMs = 0.0f;
        SpawnEnemy();
    }

    // ----- Update bullets -----
    for (auto& b : gBullets)
    {
        b.pos = b.pos + b.vel * dtSec;
        b.lifeMs -= dt;
    }
    gBullets.erase(std::remove_if(gBullets.begin(), gBullets.end(),
        [](const Bullet& b)
        {
            if (b.lifeMs <= 0.0f) return true;
            if (b.pos.x < -50 || b.pos.x > APP_VIRTUAL_WIDTH + 50) return true;
            if (b.pos.y < -50 || b.pos.y > APP_VIRTUAL_HEIGHT + 50) return true;
            return false;
        }),
        gBullets.end());

    // ----- Move enemies towards player -----
    const float enemySpeed = 130.0f + gScore * 2.0f; // px/s
    for (auto& e : gEnemies)
    {
        Vec2 dir = Normalize(gPlayerPos - e.pos);
        e.pos = e.pos + dir * (enemySpeed * dtSec);
    }

    // ----- Bullet vs Enemy -----
    for (int bi = (int)gBullets.size() - 1; bi >= 0; --bi)
    {
        bool bulletRemoved = false;
        for (int ei = (int)gEnemies.size() - 1; ei >= 0; --ei)
        {
            float enemyHitR = gEnemies[ei].half; // 
            if (gCollision->Circle(gBullets[bi].pos, gBullets[bi].radius, gEnemies[ei].pos, enemyHitR))
            {
                // remove enemy
                gEnemies.erase(gEnemies.begin() + ei);
                // remove bullet
                gBullets.erase(gBullets.begin() + bi);
                bulletRemoved = true;
                gScore += 1;
                break;
            }
        }
        if (bulletRemoved) continue;
    }

    // ----- Player vs Enemy -----
    for (const auto& e : gEnemies)
    {
        float enemyHitR = e.half;
        if (gCollision->Circle(gPlayerPos, gPlayerRadius, e.pos, enemyHitR))
        {
            gGameOver = true;
            break;
        }
    }
}
void GameSystem::Render()
{
    // Player
    gRenderHelper->DrawPlayer(gPlayerPos, gPlayerAngle);

    // Bullets
    for (const auto& b : gBullets)
        gRenderHelper->DrawQuad(b.pos, b.radius, 1.0f, 1.0f, 0.3f);

    // Enemies
    for (const auto& e : gEnemies)
        gRenderHelper->DrawQuad(e.pos, e.half, 1.0f, 0.2f, 0.2f);

    // UI
    char buf[128];
    std::snprintf(buf, sizeof(buf), "Score: %d   Enemies: %d   Bullets: %d", gScore, (int)gEnemies.size(), (int)gBullets.size());
    App::Print(20, APP_VIRTUAL_HEIGHT - 30, buf, 1, 1, 1);

    App::Print(20, 20, "Move: LeftStick (WASD emulation)  Shoot: Space  Restart: R", 0.8f, 0.8f, 0.8f);

    if (gGameOver)
        App::Print(APP_VIRTUAL_WIDTH * 0.36f, APP_VIRTUAL_HEIGHT * 0.5f, "GAME OVER - Press R", 1.0f, 0.4f, 0.4f);
}
void GameSystem::Shutdown()
{
    gBullets.clear();
    gEnemies.clear();
}