#include "RenderSystem.h"
#include "../../System/Render/RenderHelper.h"
#include "../../System/Component/Component.h"
#include "../../ContestAPI/app.h"
#include <cmath>

static RenderHelper renderHelper;
RenderHelper* gRenderHelper = &renderHelper;
// -----------------------------------------------------------------------------
// Test RenderCube: uses App::DrawTriangle directly with NDC coords + depth test.
// This does NOT use Project(). It builds clip-space-ish coords manually so that
// depth testing can work. Intended for quick testing.
// Assumptions:
// - Screen is using NDC (-1..1). (APP_USE_VIRTUAL_RES should be OFF for this test)
// - Camera rotates around Y only (camera.rotationAngle)
// -----------------------------------------------------------------------------
static bool ProjectToScreenWithDepth(float wx, float wy, float wz, const Camera3D& cam,
    float& outX, float& outY, float& outZ, float& outW)
{
    // Camera-relative
    const float rx = wx - cam.x;
    const float ry = wy - cam.y;
    const float rz = wz - cam.z;

    // Rotate around Y (same as Project())
    const float cosAngle = std::cos(cam.rotationAngle);
    const float sinAngle = std::sin(cam.rotationAngle);

    const float rotatedX = rx * cosAngle + rz * sinAngle;
    const float rotatedZ = -rx * sinAngle + rz * cosAngle;

    // Near plane reject
    const float nearZ = 1.0f;
    if (rotatedZ <= nearZ) return false;

    // Match your existing "pixel space" projection
    const float fov = 600.0f;
    const float centerX = 1024.0f / 2.0f;
    const float centerY = 768.0f / 2.0f;

    outX = rotatedX * (fov / rotatedZ) + centerX;
    outY = ry * (fov / rotatedZ) + centerY;

    // Depth: map rotatedZ into 0..1 for GL_LESS depth test.
    // Smaller z => closer => smaller outZ (passes against larger values).
    const float farZ = 3000.0f; // tune to your world scale
    float zn = (rotatedZ - nearZ) / (farZ - nearZ);
    if (zn < 0.0f) zn = 0.0f;
    if (zn > 1.0f) zn = 1.0f;

    outZ = zn;
    outW = 1.0f;
    return true;
}
// Cheap face normal via cross product (no dependency on new math types)
static Vec3 Cross3(const Vec3& a, const Vec3& b)
{
    return Vec3(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x);
}

// Test cube renderer (12 triangles). Uses depth test inside App::DrawTriangle.
void RenderCube_TestNDC(const Transform3D& t, const Camera3D& camera)
{
    const float halfW = t.width * 0.5f;
    const float halfH = t.height * 0.5f;
    const float halfD = t.depth * 0.5f;

    // 8 corners in world space
    Vec3 corners[8] = {
        Vec3(t.pos.x - halfW, t.pos.y - halfH, t.pos.z - halfD), // 0
        Vec3(t.pos.x + halfW, t.pos.y - halfH, t.pos.z - halfD), // 1
        Vec3(t.pos.x + halfW, t.pos.y + halfH, t.pos.z - halfD), // 2
        Vec3(t.pos.x - halfW, t.pos.y + halfH, t.pos.z - halfD), // 3
        Vec3(t.pos.x - halfW, t.pos.y - halfH, t.pos.z + halfD), // 4
        Vec3(t.pos.x + halfW, t.pos.y - halfH, t.pos.z + halfD), // 5
        Vec3(t.pos.x + halfW, t.pos.y + halfH, t.pos.z + halfD), // 6
        Vec3(t.pos.x - halfW, t.pos.y + halfH, t.pos.z + halfD)  // 7
    };

    // Project to NDC + depth
    float x[8], y[8], z[8], w[8];
    bool ok[8];
    for (int i = 0; i < 8; ++i) {
        ok[i] = ProjectToScreenWithDepth(corners[i].x, corners[i].y, corners[i].z, camera,
            x[i], y[i], z[i], w[i]);
    }

    // Faces
    int faces[6][4] = {
        {0, 1, 2, 3}, // back
        {5, 4, 7, 6}, // front
        {4, 0, 3, 7}, // left
        {1, 5, 6, 2}, // right
        {4, 5, 1, 0}, // bottom
        {3, 2, 6, 7}  // top
    };

    // Slight shading per face
    float faceColors[6][3] = {
        {t.r * 0.6f, t.g * 0.6f, t.b * 0.6f},
        {t.r,        t.g,        t.b},
        {t.r * 0.7f, t.g * 0.7f, t.b * 0.7f},
        {t.r * 0.8f, t.g * 0.8f, t.b * 0.8f},
        {t.r * 0.5f, t.g * 0.5f, t.b * 0.5f},
        {t.r * 0.9f, t.g * 0.9f, t.b * 0.9f}
    };

    // Optional: basic back-face culling based on geometric normal in world space
    for (int f = 0; f < 6; ++f)
    {
        const int v0 = faces[f][0];
        const int v1 = faces[f][1];
        const int v2 = faces[f][2];
        const int v3 = faces[f][3];

        if (!ok[v0] && !ok[v1] && !ok[v2] && !ok[v3]) continue;

        // Normal from (v0->v1) x (v0->v2)
        const Vec3 e1(corners[v1].x - corners[v0].x, corners[v1].y - corners[v0].y, corners[v1].z - corners[v0].z);
        const Vec3 e2(corners[v2].x - corners[v0].x, corners[v2].y - corners[v0].y, corners[v2].z - corners[v0].z);
        const Vec3 n = Cross3(e1, e2);

        // Face center
        const Vec3 c(
            (corners[v0].x + corners[v1].x + corners[v2].x + corners[v3].x) * 0.25f,
            (corners[v0].y + corners[v1].y + corners[v2].y + corners[v3].y) * 0.25f,
            (corners[v0].z + corners[v1].z + corners[v2].z + corners[v3].z) * 0.25f);

        // Vector to camera
        const Vec3 toCam(camera.x - c.x, camera.y - c.y, camera.z - c.z);
        const float dot = toCam.x * n.x + toCam.y * n.y + toCam.z * n.z;
        if (dot >= 0.0f) continue;

        const float r = faceColors[f][0];
        const float g = faceColors[f][1];
        const float b = faceColors[f][2];

        // Tri 1: (v0, v1, v2)
        if (ok[v0] || ok[v1] || ok[v2]) {
            App::DrawTriangle(
                x[v0], y[v0], z[v0], w[v0],
                x[v1], y[v1], z[v1], w[v1],
                x[v2], y[v2], z[v2], w[v2],
                r, g, b,
                r, g, b,
                r, g, b,
                false
            );
        }

        // Tri 2: (v0, v2, v3)
        if (ok[v0] || ok[v2] || ok[v3]) {
            App::DrawTriangle(
                x[v0], y[v0], z[v0], w[v0],
                x[v2], y[v2], z[v2], w[v2],
                x[v3], y[v3], z[v3], w[v3],
                r, g, b,
                r, g, b,
                r, g, b,
                false
            );
        }
    }
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
        // Calculate average of 4 corners
        Vec3 c0 = corners[v0];
        Vec3 c1 = corners[v1];
        Vec3 c2 = corners[v2];
        Vec3 c3 = corners[v3];
        Vec3 faceCenter((c0.x + c1.x + c2.x + c3.x) * 0.25f,
                       (c0.y + c1.y + c2.y + c3.y) * 0.25f,
                       (c0.z + c1.z + c2.z + c3.z) * 0.25f);
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
void RenderPlayerCube(const Transform3D& t, const Camera3D& camera) {
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
        // Calculate average of 4 corners
        Vec3 c0 = corners[v0];
        Vec3 c1 = corners[v1];
        Vec3 c2 = corners[v2];
        Vec3 c3 = corners[v3];
        Vec3 faceCenter((c0.x + c1.x + c2.x + c3.x) * 0.25f,
            (c0.y + c1.y + c2.y + c3.y) * 0.25f,
            (c0.z + c1.z + c2.z + c3.z) * 0.25f);
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
        RenderCube_TestNDC(t, camera);
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
        RenderCube(t, camera);
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