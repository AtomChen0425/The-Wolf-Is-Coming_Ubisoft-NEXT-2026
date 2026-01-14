#include "RenderSystem.h"
#include "../System/Render/RenderHelper.h"
#include "../System/Component/Component.h"
#include "../ContestAPI/app.h"
#include <cstdio>
#include <algorithm>
extern RenderHelper* gRenderHelper;
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
    // First, render all enemies and bullets (they don't have sprites)
    // Render enemies with their type-specific colors
    View<EnemyTag, Position3D, EnemyTypeComponent, RigidBody> enemyView(registry);
    for (EntityID id : enemyView) {
        auto& pos = enemyView.get<Position3D>(id);
        auto& enemyType = enemyView.get<EnemyTypeComponent>(id);
        auto& rb = enemyView.get<RigidBody>(id);
        
        // Convert to screen space
        float screenX = pos.x - camera.x;
        float screenY = pos.z - camera.y;
        
        // Culling
        if (screenX < -100 || screenX > camera.width + 100 ||
            screenY < -100 || screenY > camera.height + 100) {
            continue;
        }
        
        // Draw shadow
        gRenderHelper->DrawShadow(screenX, screenY, rb.radius * 0.8f);
        
        // Draw enemy square at ground position (minus Y for jump height)
        float drawY = screenY - pos.y;
        Vec2 screenPos = { screenX, drawY };
        gRenderHelper->DrawQuad(screenPos, rb.radius, 
                               enemyType.color.x, 
                               enemyType.color.y, 
                               enemyType.color.z);
    }
    
    // Render bullets
    View<BulletTag, Position3D, RigidBody> bulletView(registry);
    for (EntityID id : bulletView) {
        auto& pos = bulletView.get<Position3D>(id);
        auto& rb = bulletView.get<RigidBody>(id);
        
        float screenX = pos.x - camera.x;
        float screenY = pos.z - camera.y;
        
        if (screenX < -100 || screenX > camera.width + 100 ||
            screenY < -100 || screenY > camera.height + 100) {
            continue;
        }
        
        float drawY = screenY - pos.y;
        Vec2 screenPos = { screenX, drawY };
        gRenderHelper->DrawQuad(screenPos, rb.radius, 1.0f, 1.0f, 0.3f);
    }

    // Then render sprites (player and other entities with sprites)
    View<Position3D, SpriteComponent> view(registry);

    // �����߼� (Y-Sorting) - ��ѡ�����������
    std::vector<EntityID> sortedEntities;
    for (EntityID id : view) sortedEntities.push_back(id);
    std::sort(sortedEntities.begin(), sortedEntities.end(), [&](EntityID a, EntityID b) {
        return view.get<Position3D>(a).z < view.get<Position3D>(b).z;
        });

    for (EntityID id : sortedEntities) {
        auto& pos = view.get<Position3D>(id);
        auto& spr = view.get<SpriteComponent>(id);

        if (!spr.sprite) continue;

        // ===========================================
        // �ؼ���ʽ����ȥ Camera ƫ��
        // ===========================================

        // 1. ���㡾���桿����Ļ�ϵ�λ��
        // ��ĻX = ����X - ���X
        float screenGroundX = pos.x - camera.x;
        // ��ĻY = ����Z - ���Y
        float screenGroundY = pos.z - camera.y;

        // ��׶�޳� (Culling): �������Ļ��Ͳ���
        if (screenGroundX < -100 || screenGroundX > camera.width + 100 ||
            screenGroundY < -100 || screenGroundY > camera.height + 100) {
            continue;
        }

        // 2. ������Ӱ (��Զ���ڵ���λ��)
        float shadowSize = 20.0f;
        // ��� DrawLine ������Ҫ RGB�������ú�ɫ (0,0,0)
		gRenderHelper->DrawShadow(screenGroundX, screenGroundY, shadowSize);

        // 3. �������� (������Ծ�߶�ƫ��)
        // ֻ�������õ��� pos.y��
        float spriteScreenY = screenGroundY - pos.y;

        spr.sprite->SetPosition(screenGroundX, spriteScreenY);
        spr.sprite->Draw();
		App::Print(screenGroundX, spriteScreenY - 30.0f, ("X:"+ std::to_string((int)pos.x) + "Y:" + std::to_string((int)pos.z)).c_str(), 1.0f, 1.0f, 0.0f);
		
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