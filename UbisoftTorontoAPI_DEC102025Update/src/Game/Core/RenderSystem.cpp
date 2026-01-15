#include "RenderSystem.h"
#include "../../System/Render/RenderHelper.h"
#include "../../System/Component/Component.h"
#include "../../ContestAPI/app.h"
#include <cmath>
RenderHelper* gRenderHelper;

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
    View<EnemyTag, Position> enemyView(registry);
    for (EntityID id : enemyView) {
        auto& enemypos = enemyView.get<Position>(id);
        gRenderHelper->DrawQuad(enemypos.pos, 20.0f, 1.0f, 0.0f, 0.0f);
    }
}
void UpdateSpriteAnimation(EntityManager& registry, const float dt) {
    View<SpriteComponent> view(registry);
    for (EntityID id : view) {
        auto& spr = view.get<SpriteComponent>(id);
        if (spr.sprite) {
            spr.sprite->Update(dt);
        }
    }
}

void RenderSystem25D(EntityManager& registry, Camera25D& camera) {
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

        float screenGroundX = pos.x - camera.x;
        float screenGroundY = pos.z - camera.y;

        if (screenGroundX < -100 || screenGroundX > camera.width + 100 ||
            screenGroundY < -100 || screenGroundY > camera.height + 100) {
            continue;
        }

        float shadowSize = 20.0f;
		gRenderHelper->DrawShadow(screenGroundX, screenGroundY, shadowSize);

        float spriteScreenY = screenGroundY - pos.y;

        spr.sprite->SetPosition(screenGroundX, spriteScreenY);
        spr.sprite->Draw();
		App::Print(screenGroundX, spriteScreenY - 30.0f, ("X:"+ std::to_string((int)pos.x) + "Y:" + std::to_string((int)pos.z)).c_str(), 1.0f, 1.0f, 0.0f);
		
    }
}

// Helper function to calculate squared distance between two 3D points
inline float DistanceSq(float x1, float y1, float z1, float x2, float y2, float z2) {
    float dx = x1 - x2;
    float dy = y1 - y2;
    float dz = z1 - z2;
    return dx * dx + dy * dy + dz * dz;
}
// Calculate the minimum depth (closest point) of a cube in camera space
// This is used for proper z-sorting to avoid occlusion issues
static float MaxDepthInCameraSpace(const Transform3D& t, const Camera3D& camera) {
    const float halfW = t.width * 0.5f;
    const float halfH = t.height * 0.5f;
    const float halfD = t.depth * 0.5f;

    // 8 corners (world space)
    const Vec3 corners[8] = {
        Vec3(t.pos.x - halfW, t.pos.y - halfH, t.pos.z - halfD),
        Vec3(t.pos.x + halfW, t.pos.y - halfH, t.pos.z - halfD),
        Vec3(t.pos.x + halfW, t.pos.y + halfH, t.pos.z - halfD),
        Vec3(t.pos.x - halfW, t.pos.y + halfH, t.pos.z - halfD),
        Vec3(t.pos.x - halfW, t.pos.y - halfH, t.pos.z + halfD),
        Vec3(t.pos.x + halfW, t.pos.y - halfH, t.pos.z + halfD),
        Vec3(t.pos.x + halfW, t.pos.y + halfH, t.pos.z + halfD),
        Vec3(t.pos.x - halfW, t.pos.y + halfH, t.pos.z + halfD)
    };

    const float cosAngle = std::cos(camera.rotationAngle);
    const float sinAngle = std::sin(camera.rotationAngle);

    float maxZ = -1e30f;

    for (int i = 0; i < 8; ++i) {
        // to camera-relative
        const float rx = corners[i].x - camera.x;
        const float rz = corners[i].z - camera.z +camera.followOffsetZ;

        // rotate around Y (same math as Project)
        const float rotatedZ = -rx * sinAngle + rz * cosAngle;

        if (rotatedZ > maxZ) maxZ = rotatedZ;
    }

    return maxZ;
}
void RenderRoad3D(EntityManager& registry, Camera3D& camera) {
    // Only render map blocks, not the player
    View<Transform3D> view(registry);
    std::vector<EntityID> sortedEntities;
    for (EntityID id : view) {
        sortedEntities.push_back(id);
    }

    // Sort by minimum depth in camera space for proper rendering order
    // Painter's algorithm: render far to near (larger depth values first)
    std::sort(sortedEntities.begin(), sortedEntities.end(), [&](EntityID a, EntityID b) {
        auto& ta = view.get<Transform3D>(a);
        auto& tb = view.get<Transform3D>(b);

        float distA = MaxDepthInCameraSpace(ta, camera);
        float distB = MaxDepthInCameraSpace(tb, camera);
        
        // If blocks are at the same position, render taller blocks first
        if (ta.pos.x == tb.pos.x && ta.pos.z == tb.pos.z) { 
            return ta.pos.y > tb.pos.y; 
        }
        
        // Render far to near: larger depth first
        return distA < distB;
    });
    
    for (EntityID id : sortedEntities) {
        auto& t = view.get<Transform3D>(id);
        //RenderCube(t, camera);
        gRenderHelper->RenderCube_inNDC(t, camera);
    }
}

void RenderPlayer3D(EntityManager& registry, Camera3D& camera) {
    View<Transform3D, PlayerTag> view(registry);

    for (EntityID id : view) {
        auto& t = view.get<Transform3D>(id);
        //RenderPlayerCube(t, camera);
        
        // Display player position info
        App::Print(50, 50, 
            ("Player Pos - X:" + std::to_string((int)t.pos.x) + 
             " Y:" + std::to_string((int)t.pos.y) + 
             " Z:" + std::to_string((int)t.pos.z)).c_str(), 
            1.0f, 1.0f, 1.0f);
        
        // Display controls info
        App::Print(50, 80, "Controls: WASD to move, SPACE to jump, Arrow keys to rotate", 1.0f, 1.0f, 0.0f);
    }
}

void RenderBullets3D(EntityManager& registry, Camera3D& camera) {
    View<Transform3D, Bullet> bulletView(registry);
    
    for (EntityID id : bulletView) {
        auto& t = bulletView.get<Transform3D>(id);
        // Render bullets as small cubes
        gRenderHelper->RenderCube(t, camera);
    }
}
void RenderSystem::Render(EntityManager& registry) {
    DrawSprite(registry);
    DrawEnemies(registry);
}
void RenderSystem::Render(EntityManager& registry, Camera25D& camera) {
    RenderSystem25D(registry, camera);
}
void RenderSystem::Render(EntityManager& registry, Camera3D& camera) {
    
    
    
    RenderRoad3D(registry, camera);
    RenderBullets3D(registry, camera);
    RenderPlayer3D(registry, camera);
    
}
void RenderSystem::Update(EntityManager& registry, const float dt) {
	UpdateSpriteAnimation(registry, dt);
}