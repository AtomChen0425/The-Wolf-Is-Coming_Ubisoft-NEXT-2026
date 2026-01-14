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

// Helper function to render a 3D cube with all 6 faces
void RenderCube(const Transform3D& t, const Camera3D& camera) {
    float halfW = t.width / 2;
    float halfH = t.height / 2;
    float halfD = t.depth / 2;
    
    // Define 8 corners of the cube
    Vec3 corners[8] = {
        Vec3(t.pos.x - halfW, t.pos.y - halfH, t.pos.z - halfD), // 0: back-bottom-left
        Vec3(t.pos.x + halfW, t.pos.y - halfH, t.pos.z - halfD), // 1: back-bottom-right
        Vec3(t.pos.x + halfW, t.pos.y + halfH, t.pos.z - halfD), // 2: back-top-right
        Vec3(t.pos.x - halfW, t.pos.y + halfH, t.pos.z - halfD), // 3: back-top-left
        Vec3(t.pos.x - halfW, t.pos.y - halfH, t.pos.z + halfD), // 4: front-bottom-left
        Vec3(t.pos.x + halfW, t.pos.y - halfH, t.pos.z + halfD), // 5: front-bottom-right
        Vec3(t.pos.x + halfW, t.pos.y + halfH, t.pos.z + halfD), // 6: front-top-right
        Vec3(t.pos.x - halfW, t.pos.y + halfH, t.pos.z + halfD)  // 7: front-top-left
    };
    
    // Project all 8 corners
    float sx[8], sy[8];
    bool visible[8];
    for (int i = 0; i < 8; i++) {
        visible[i] = Project(corners[i].x, corners[i].y, corners[i].z, camera, sx[i], sy[i]);
    }
    
    // Define the 6 faces (each face has 4 vertices, split into 2 triangles)
    // Order: back, front, left, right, bottom, top
    int faces[6][4] = {
        {0, 1, 2, 3}, // back face
        {5, 4, 7, 6}, // front face
        {4, 0, 3, 7}, // left face
        {1, 5, 6, 2}, // right face
        {4, 5, 1, 0}, // bottom face
        {3, 2, 6, 7}  // top face
    };
    
    // Face normals (for back-face culling)
    Vec3 faceNormals[6] = {
        Vec3(0, 0, -1),  // back
        Vec3(0, 0, 1),   // front
        Vec3(-1, 0, 0),  // left
        Vec3(1, 0, 0),   // right
        Vec3(0, -1, 0),  // bottom
        Vec3(0, 1, 0)    // top
    };
    
    // Colors for each face (darker on some faces for better depth perception)
    float faceColors[6][3] = {
        {t.r * 0.6f, t.g * 0.6f, t.b * 0.6f}, // back - darker
        {t.r, t.g, t.b},                        // front - full color
        {t.r * 0.7f, t.g * 0.7f, t.b * 0.7f}, // left - medium
        {t.r * 0.8f, t.g * 0.8f, t.b * 0.8f}, // right - medium-light
        {t.r * 0.5f, t.g * 0.5f, t.b * 0.5f}, // bottom - darkest
        {t.r * 0.9f, t.g * 0.9f, t.b * 0.9f}  // top - light
    };
    
    // Render each face
    for (int f = 0; f < 6; f++) {
        // Check if all vertices of this face are visible
        int v0 = faces[f][0], v1 = faces[f][1], v2 = faces[f][2], v3 = faces[f][3];
        if (!visible[v0] && !visible[v1] && !visible[v2] && !visible[v3]) {
            continue; // Skip this face if all vertices are behind camera
        }
        
        // Back-face culling: check if face is facing away from camera
        Vec3 faceCenter = (corners[v0] + corners[v1] + corners[v2] + corners[v3]) * (1.0f / 4.0f);
        Vec3 toCamera = Vec3(camera.x - faceCenter.x, camera.y - faceCenter.y, camera.z - faceCenter.z);
        float dot = toCamera.x * faceNormals[f].x + toCamera.y * faceNormals[f].y + toCamera.z * faceNormals[f].z;
        
        if (dot <= 0) continue; // Face is facing away from camera
        
        float r = faceColors[f][0];
        float g = faceColors[f][1];
        float b = faceColors[f][2];
        
        // Draw first triangle (v0, v1, v2)
        if (visible[v0] || visible[v1] || visible[v2]) {
            App::DrawTriangle(
                sx[v0], sy[v0], 0, 1.0f,
                sx[v1], sy[v1], 0, 1.0f,
                sx[v2], sy[v2], 0, 1.0f,
                r, g, b,
                r, g, b,
                r, g, b,
                false
            );
        }
        
        // Draw second triangle (v0, v2, v3)
        if (visible[v0] || visible[v2] || visible[v3]) {
            App::DrawTriangle(
                sx[v0], sy[v0], 0, 1.0f,
                sx[v2], sy[v2], 0, 1.0f,
                sx[v3], sy[v3], 0, 1.0f,
                r, g, b,
                r, g, b,
                r, g, b,
                false
            );
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

// Helper function to calculate squared distance between two 3D points
inline float DistanceSq(float x1, float y1, float z1, float x2, float y2, float z2) {
    float dx = x1 - x2;
    float dy = y1 - y2;
    float dz = z1 - z2;
    return dx * dx + dy * dy + dz * dz;
}

void RenderRoad3D(EntityManager& registry, Camera3D& camera) {
    View<Transform3D, MapBlockTag> view(registry);
    std::vector<EntityID> sortedEntities;
    for (EntityID id : view) {
        sortedEntities.push_back(id);
    }

    // Sort by distance from camera for proper rendering order
    std::sort(sortedEntities.begin(), sortedEntities.end(), [&](EntityID a, EntityID b) {
        auto& ta = view.get<Transform3D>(a);
        auto& tb = view.get<Transform3D>(b);
        float distA = DistanceSq(ta.pos.x, ta.pos.y, ta.pos.z, camera.x, camera.y, camera.z);
        float distB = DistanceSq(tb.pos.x, tb.pos.y, tb.pos.z, camera.x, camera.y, camera.z);
        return distA > distB; // Render far to near
    });
    
    for (EntityID id : sortedEntities) {
        auto& t = view.get<Transform3D>(id);
        RenderCube(t, camera);
    }
}

void RenderPlayer3D(EntityManager& registry, Camera3D& camera) {
    View<Transform3D, PlayerTag> view(registry);

    for (EntityID id : view) {
        auto& t = view.get<Transform3D>(id);
        RenderCube(t, camera);
        
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