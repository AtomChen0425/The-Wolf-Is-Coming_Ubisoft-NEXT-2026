#include "RenderSystem.h"
#include "../../System/Render/RenderHelper.h"
#include "../../System/Component/Component.h"
#include "../../ContestAPI/app.h"
#include <cmath>

static RenderHelper renderHelper;
RenderHelper* gRenderHelper = &renderHelper;
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
    // ���������� SpriteComponent �� Position ��ʵ��
    View<SpriteComponent> view(registry);
    for (EntityID id : view) {
        auto& spr = view.get<SpriteComponent>(id);
        if (spr.sprite) {
            spr.sprite->Update(dt);
        }
    }
}
bool Project(float wx, float wy, float wz, const Camera3D& cam, float& outX, float& outY) {
    
    // Calculate relative position to camera
    float rx = wx - cam.x;
    float ry = wy - cam.y;
    float rz = wz - cam.z;

    // Rotate the view coordinates by the camera's rotation angle (around Y axis)
    float cosAngle = std::cos(cam.rotationAngle);
    float sinAngle = std::sin(cam.rotationAngle);
    
    float rotatedX = rx * cosAngle + rz * sinAngle;
    float rotatedZ = -rx * sinAngle + rz * cosAngle;
    
    // Use rotated coordinates for projection
    if (rotatedZ <= 1.0f) return false;

    float fov = 600.0f;
    float centerX = 1024.0f / 2.0f;
    float centerY = 768.0f / 2.0f;

    outX = rotatedX * (fov / rotatedZ) + centerX;
    outY = ry * (fov / rotatedZ) + centerY;

    return true;
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

void RenderRoad3D(EntityManager& registry, Camera3D& camera) {
    View<Transform3D, MapBlockTag> view(registry);
    std::vector<EntityID> sortedEntities;
    for (EntityID id : view) {
        sortedEntities.push_back(id);
    }

    std::sort(sortedEntities.begin(), sortedEntities.end(), [&](EntityID a, EntityID b) {
        return view.get<Transform3D>(a).pos.z < view.get<Transform3D>(b).pos.z;
        });
    for (EntityID id : sortedEntities) {
        auto& t = view.get<Transform3D>(id);

        // Calculate the 4 corners of the top face of the road block
        float halfW = t.width / 2; 
        float halfD = t.depth / 2; 
        float topY = t.pos.y + t.height / 2;

        // Define 4 corners in world space
        float x1 = t.pos.x - halfW; float z1 = t.pos.z - halfD; // Back-left
        float x2 = t.pos.x + halfW; float z2 = t.pos.z - halfD; // Back-right
        float x3 = t.pos.x + halfW; float z3 = t.pos.z + halfD; // Front-right
        float x4 = t.pos.x - halfW; float z4 = t.pos.z + halfD; // Front-left

        float sx1, sy1, sx2, sy2, sx3, sy3, sx4, sy4;

        // Project 4 corners to screen space
        if (!Project(x1, topY, z1, camera, sx1, sy1)) continue;
        if (!Project(x2, topY, z2, camera, sx2, sy2)) continue;
        if (!Project(x3, topY, z3, camera, sx3, sy3)) continue;
        if (!Project(x4, topY, z4, camera, sx4, sy4)) continue;

        // Draw the road block as a quad (simplified rendering)
        /*gRenderHelper->DrawQuad(
            Vec2((sx1 + sx2 + sx3 + sx4) / 4, (sy1 + sy2 + sy3 + sy4) / 4),
            t.width / 2, 
            t.r, t.g, t.b
        );*/
        App::DrawTriangle(
            sx1, sy1, 0, 1.0f,
            sx2, sy2, 0, 1.0f,
            sx3, sy3, 0, 1.0f,
            t.r, t.g, t.b,  // 顶点1颜色
            t.r, t.g, t.b,  // 顶点2颜色
            t.r, t.g, t.b,  // 顶点3颜色
            false           // 是否线框
        );

        // 绘制第二个三角形 (左后 -> 右前 -> 左前)
        App::DrawTriangle(
            sx1, sy1, 0, 1.0f,
            sx3, sy3, 0, 1.0f,
            sx4, sy4, 0, 1.0f,
            t.r, t.g, t.b,
            t.r, t.g, t.b,
            t.r, t.g, t.b,
            false
        );
    }
}

void RenderPlayer3D(EntityManager& registry, Camera3D& camera) {
    View<Transform3D, PlayerTag> view(registry);

    for (EntityID id : view) {
        auto& t = view.get<Transform3D>(id);

        // Calculate the 4 corners of the top face of the player cube
        float halfW = t.width / 2; 
        float halfD = t.depth / 2; 
        float topY = t.pos.y + t.height;

        // Define 4 corners in world space
        float x1 = t.pos.x - halfW; float z1 = t.pos.z - halfD; // Back-left
        float x2 = t.pos.x + halfW; float z2 = t.pos.z - halfD; // Back-right
        float x3 = t.pos.x + halfW; float z3 = t.pos.z + halfD; // Front-right
        float x4 = t.pos.x - halfW; float z4 = t.pos.z + halfD; // Front-left

        float sx1, sy1, sx2, sy2, sx3, sy3, sx4, sy4;

        // Project 4 corners to screen space
        if (!Project(x1, topY, z1, camera, sx1, sy1)) continue;
        if (!Project(x2, topY, z2, camera, sx2, sy2)) continue;
        if (!Project(x3, topY, z3, camera, sx3, sy3)) continue;
        if (!Project(x4, topY, z4, camera, sx4, sy4)) continue;

        // Draw the player as a quad
        /*gRenderHelper->DrawQuad(
            Vec2((sx1 + sx2 + sx3 + sx4) / 4, (sy1 + sy2 + sy3 + sy4) / 4), 
            t.width / 2, 
            t.r, t.g, t.b
        );*/
        App::DrawTriangle(
            sx1, sy1, -1, 1.0f,
            sx2, sy2, -1, 1.0f,
            sx3, sy3, -1, 1.0f,
            t.r, t.g, t.b,  // 顶点1颜色
            t.r, t.g, t.b,  // 顶点2颜色
            t.r, t.g, t.b,  // 顶点3颜色
            false           // 是否线框
        );

        // 绘制第二个三角形 (左后 -> 右前 -> 左前)
        App::DrawTriangle(
            sx1, sy1, -1, 1.0f,
            sx3, sy3, -1, 1.0f,
            sx4, sy4, -1, 1.0f,
            t.r, t.g, t.b,
            t.r, t.g, t.b,
            t.r, t.g, t.b,
            false
        );
        
        // Display player position info
        App::Print(50, 50, 
            ("Player Pos - X:" + std::to_string((int)t.pos.x) + 
             " Y:" + std::to_string((int)t.pos.y) + 
             " Z:" + std::to_string((int)t.pos.z)).c_str(), 
            1.0f, 1.0f, 1.0f);
        
        // Display controls info
        App::Print(50, 80, "Controls: WASD to move, SPACE to jump", 1.0f, 1.0f, 0.0f);
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
    RenderPlayer3D(registry, camera);
    
}
void RenderSystem::Update(EntityManager& registry, const float dt) {
	UpdateSpriteAnimation(registry, dt);
}