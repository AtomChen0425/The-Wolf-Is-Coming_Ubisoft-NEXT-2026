#include "RenderSystem.h"
#include "../System/Render/RenderHelper.h"
#include "../System/Component/Component.h"
#include "../ContestAPI/app.h"
#include <cstdio>
#include <algorithm>
extern RenderHelper* gRenderHelper;

// ========================================
// Camera Projection Functions
// ========================================
// Separates camera logic from rendering logic
// Projects world coordinates to screen coordinates based on camera position

// Project a 3D world position to 2D screen coordinates
Vec2 ProjectWorldToScreen(const Position3D& worldPos, const Camera25D& camera) {
    // Screen position = World position - Camera position
    float screenX = worldPos.x - camera.x;
    float screenY = worldPos.z - camera.y;  // z maps to screen y
    
    // Apply height offset (y-axis is vertical/jump)
    float finalScreenY = screenY - worldPos.y;
    
    return Vec2{ screenX, finalScreenY };
}

// Project a 3D world position to 2D screen coordinates (ground level only, no height offset)
Vec2 ProjectWorldToScreenGround(const Position3D& worldPos, const Camera25D& camera) {
    float screenX = worldPos.x - camera.x;
    float screenY = worldPos.z - camera.y;
    
    return Vec2{ screenX, screenY };
}

// Check if a screen position is visible (within camera bounds with margin)
bool IsVisible(const Vec2& screenPos, const Camera25D& camera, float margin = 100.0f) {
    return screenPos.x >= -margin && screenPos.x <= camera.width + margin &&
           screenPos.y >= -margin && screenPos.y <= camera.height + margin;
}
void DrawSprite(EntityManager& registry) {
    View<Position, SpriteComponent> view(registry);
    for (EntityID id : view) {
        auto& pos = view.get<Position>(id);
        auto& spr = view.get<SpriteComponent>(id);

        if (spr.sprite) {
            spr.sprite->SetPosition(pos.pos.x, pos.pos.y);

            spr.sprite->Draw();
        }
    }
}
void DrawEnemies(EntityManager& registry) {
    // Draw enemies with their type-specific colors
    View<EnemyTag, Position, EnemyTypeComponent, RigidBody> enemyView(registry);
    for (EntityID id : enemyView) {
        auto& enemypos = enemyView.get<Position>(id);
        auto& enemyType = enemyView.get<EnemyTypeComponent>(id);
        auto& rb = enemyView.get<RigidBody>(id);
        
        gRenderHelper->DrawQuad(enemypos.pos, rb.radius, 
                               enemyType.color.x, 
                               enemyType.color.y, 
                               enemyType.color.z);
    }
}

void DrawBullets(EntityManager& registry) {
    View<BulletTag, Position, RigidBody> bulletView(registry);
    for (EntityID id : bulletView) {
        auto& pos = bulletView.get<Position>(id);
        auto& rb = bulletView.get<RigidBody>(id);
        
        // Draw bullets as yellow squares
        gRenderHelper->DrawQuad(pos.pos, rb.radius, 1.0f, 1.0f, 0.3f);
    }
}
void UpdateSpriteAnimation(EntityManager& registry, const float dt) {
    // ���������� SpriteComponent �� Position ��ʵ��
    View<SpriteComponent> view(registry);
    for (EntityID id : view) {
        auto& spr = view.get<SpriteComponent>(id);
        if (spr.sprite) {
            spr.sprite->Update(dt);
        }
    }
}

void RenderSystem25D(EntityManager& registry, Camera25D& camera) {
    // ========================================
    // Render Enemies
    // ========================================
    View<EnemyTag, Position3D, EnemyTypeComponent, RigidBody> enemyView(registry);
    for (EntityID id : enemyView) {
        auto& worldPos = enemyView.get<Position3D>(id);
        auto& enemyType = enemyView.get<EnemyTypeComponent>(id);
        auto& rb = enemyView.get<RigidBody>(id);
        
        // Project world position to screen using camera
        Vec2 screenPosGround = ProjectWorldToScreenGround(worldPos, camera);
        
        // Cull if not visible
        if (!IsVisible(screenPosGround, camera)) {
            continue;
        }
        
        // Draw shadow at ground level
        gRenderHelper->DrawShadow(screenPosGround.x, screenPosGround.y, rb.radius * 0.8f);
        
        // Draw enemy with height offset
        Vec2 screenPos = ProjectWorldToScreen(worldPos, camera);
        gRenderHelper->DrawQuad(screenPos, rb.radius, 
                               enemyType.color.x, 
                               enemyType.color.y, 
                               enemyType.color.z);
    }
    
    // ========================================
    // Render Bullets
    // ========================================
    View<BulletTag, Position3D, RigidBody> bulletView(registry);
    for (EntityID id : bulletView) {
        auto& worldPos = bulletView.get<Position3D>(id);
        auto& rb = bulletView.get<RigidBody>(id);
        
        // Project world position to screen using camera
        Vec2 screenPos = ProjectWorldToScreen(worldPos, camera);
        
        // Cull if not visible
        if (!IsVisible(screenPos, camera)) {
            continue;
        }
        
        // Draw bullet
        gRenderHelper->DrawQuad(screenPos, rb.radius, 1.0f, 1.0f, 0.3f);
    }

    // ========================================
    // Render Sprites with Y-Sorting
    // ========================================
    View<Position3D, SpriteComponent> view(registry);

    // Sort entities by depth (z-coordinate) for proper layering
    std::vector<EntityID> sortedEntities;
    for (EntityID id : view) sortedEntities.push_back(id);
    std::sort(sortedEntities.begin(), sortedEntities.end(), [&](EntityID a, EntityID b) {
        return view.get<Position3D>(a).z < view.get<Position3D>(b).z;
    });

    for (EntityID id : sortedEntities) {
        auto& worldPos = view.get<Position3D>(id);
        auto& spr = view.get<SpriteComponent>(id);

        if (!spr.sprite) continue;

        // Project world position to screen using camera
        Vec2 screenPosGround = ProjectWorldToScreenGround(worldPos, camera);
        
        // Cull if not visible
        if (!IsVisible(screenPosGround, camera)) {
            continue;
        }

        // Draw shadow at ground level
        float shadowSize = 20.0f;
        gRenderHelper->DrawShadow(screenPosGround.x, screenPosGround.y, shadowSize);

        // Draw sprite with height offset
        Vec2 screenPos = ProjectWorldToScreen(worldPos, camera);
        spr.sprite->SetPosition(screenPos.x, screenPos.y);
        spr.sprite->Draw();
        
        // Debug display position
        App::Print(screenPos.x, screenPos.y - 30.0f, 
                  ("X:"+ std::to_string((int)worldPos.x) + "Y:" + std::to_string((int)worldPos.z)).c_str(), 
                  1.0f, 1.0f, 0.0f);
    }
}
void RenderSystem::Render(EntityManager& registry) {
    DrawSprite(registry);
    DrawEnemies(registry);
    DrawBullets(registry);
}
void RenderSystem::Render(EntityManager& registry, Camera25D& camera) {
    RenderSystem25D(registry, camera);
    
    // Display UI/HUD
    View<PlayerTag, Health, Experience> playerView(registry);
    for (EntityID id : playerView) {
        auto& health = playerView.get<Health>(id);
        auto& exp = playerView.get<Experience>(id);
        
        // Display health bar at top left
        char healthBuf[64];
        std::snprintf(healthBuf, sizeof(healthBuf), "HP: %d/%d", health.currentHealth, health.maxHealth);
        App::Print(20, camera.height - 30, healthBuf, 1.0f, 0.3f, 0.3f);
        
        // Display level and experience
        char expBuf[64];
        std::snprintf(expBuf, sizeof(expBuf), "Level: %d  Exp: %d/%d", exp.level, exp.currentExp, exp.expToNextLevel);
        App::Print(20, camera.height - 50, expBuf, 0.3f, 1.0f, 0.3f);
        
        break;
    }
    
    // Count enemies and bullets
    View<EnemyTag> enemyView(registry);
    View<BulletTag> bulletView(registry);
    int enemyCount = 0;
    int bulletCount = 0;
    
    for (EntityID id : enemyView) enemyCount++;
    for (EntityID id : bulletView) bulletCount++;
    
    char statsBuf[128];
    std::snprintf(statsBuf, sizeof(statsBuf), "Enemies: %d  Bullets: %d", enemyCount, bulletCount);
    App::Print(20, camera.height - 70, statsBuf, 0.8f, 0.8f, 0.8f);
    
    // Display controls at bottom
    App::Print(20, 20, "Move: LeftStick (WASD)  Jump: Space  Auto-Fire Enabled", 0.7f, 0.7f, 0.7f);
}
void RenderSystem::Update(EntityManager& registry, const float dt) {
	UpdateSpriteAnimation(registry, dt);
}