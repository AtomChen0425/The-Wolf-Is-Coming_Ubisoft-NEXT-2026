#include <cmath>
#include "RenderHelper.h"
#include "../../ContestAPI/app.h"
#include "../System/Math/Math.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

bool Project(float wx, float wy, float wz, const Camera3D& cam, float& outX, float& outY) {

    // Calculate relative position to camera
    float rx = wx - cam.x;
    float ry = wy - cam.y;
    float rz = wz - cam.z;

    // First rotate around Y axis (yaw)
    float cosYaw = std::cos(cam.yawAngle);
    float sinYaw = std::sin(cam.yawAngle);

    float rotatedX = rx * cosYaw + rz * sinYaw;
    float rotatedZ = -rx * sinYaw + rz * cosYaw;
    
    // Then apply pitch rotation (rotate around X axis in camera space)
    float cosPitch = std::cos(cam.pitchAngle);
    float sinPitch = std::sin(cam.pitchAngle);
    
    float pitchedY = ry * cosPitch - rotatedZ * sinPitch;
    float pitchedZ = ry * sinPitch + rotatedZ * cosPitch;

    // Use pitch-adjusted coordinates for projection
    if (pitchedZ <= 1.0f) return false;

    float fov = 600.0f;
    float centerX = 1024.0f / 2.0f;
    float centerY = 768.0f / 2.0f;

    outX = rotatedX * (fov / pitchedZ) + centerX;
    outY = pitchedY * (fov / pitchedZ) + centerY;

    return true;
}
static bool ProjectToScreenWithDepth(float wx, float wy, float wz, const Camera3D& cam,
    float& outX, float& outY, float& outZ, float& outW)
{
    // Camera-relative
    const float rx = wx - cam.x;
    const float ry = wy - cam.y;
    const float rz = wz - cam.z;

    // First rotate around Y axis (yaw)
    const float cosYaw = std::cos(cam.yawAngle);
    const float sinYaw = std::sin(cam.yawAngle);

    const float rotatedX = rx * cosYaw + rz * sinYaw;
    const float rotatedZ = -rx * sinYaw + rz * cosYaw;
    
    // Then apply pitch rotation (rotate around X axis in camera space)
    const float cosPitch = std::cos(cam.pitchAngle);
    const float sinPitch = std::sin(cam.pitchAngle);
    
    const float pitchedY = ry * cosPitch - rotatedZ * sinPitch;
    const float pitchedZ = ry * sinPitch + rotatedZ * cosPitch;

    // Near plane reject
    const float nearZ = 1.0f;
    if (pitchedZ <= nearZ) return false;

    // Match your existing "pixel space" projection
    const float fov = 600.0f;
    const float centerX = 1024.0f / 2.0f;
    const float centerY = 768.0f / 2.0f;

    outX = rotatedX * (fov / pitchedZ) + centerX;
    outY = pitchedY * (fov / pitchedZ) + centerY;

    // Depth: map pitchedZ into 0..1 for GL_LESS depth test.
    // Smaller z => closer => smaller outZ (passes against larger values).
    const float farZ = 3000.0f; // tune to your world scale
    float zn = (pitchedZ - nearZ) / (farZ - nearZ);
    if (zn < 0.0f) zn = 0.0f;
    if (zn > 1.0f) zn = 1.0f;

    outZ = zn;
    outW = 1.0f;
    return true;
}

void RenderHelper::DrawQuad(Vec2 c, float half, float r, float g, float b)
{
    App::DrawTriangle(c.x - half, c.y - half, 0, 1, c.x + half, c.y - half, 0, 1, c.x + half, c.y + half, 0, 1,
        r, g, b, r, g, b, r, g, b, false);
    App::DrawTriangle(c.x - half, c.y - half, 0, 1, c.x + half, c.y + half, 0, 1, c.x - half, c.y + half, 0, 1,
        r, g, b, r, g, b, r, g, b, false);
}

void RenderHelper::DrawPlayer(Vec2 p, float angleRad)
{
    const float s = 18.0f;
    Vec2 f = { std::cos(angleRad), std::sin(angleRad) };
    Vec2 r = { -f.y, f.x }; // right

    Vec2 p1 = p + f * (s * 1.2f);
    Vec2 p2 = p - f * (s * 0.8f) + r * (s * 0.8f);
    Vec2 p3 = p - f * (s * 0.8f) - r * (s * 0.8f);

    App::DrawTriangle(p1.x, p1.y, 0, 1,
        p2.x, p2.y, 0, 1,
        p3.x, p3.y, 0, 1,
        0.2f, 0.7f, 1.0f,
        0.2f, 0.7f, 1.0f,
        0.2f, 0.7f, 1.0f,
        false);
}

void RenderHelper::DrawShadow(float x, float y, float ShadowSize)
{
    App::DrawLine(x - ShadowSize, y, x + ShadowSize, y, 0, 0, 0);
    App::DrawLine(x, y - ShadowSize / 2, x, y + ShadowSize / 2, 0, 0, 0);
}

// Draw a 3D sphere using triangles
// This is a simple implementation using latitude/longitude segments
void RenderHelper::DrawSphere(float centerX, float centerY, float centerZ, float radius, float r, float g, float b, int segments)
{
    // Segment count constraints for performance and quality balance
    const int MIN_SEGMENTS = 4;
    const int MAX_SEGMENTS = 32;
    
    // Clamp segments to reasonable values
    if (segments < MIN_SEGMENTS) segments = MIN_SEGMENTS;
    if (segments > MAX_SEGMENTS) segments = MAX_SEGMENTS;
    
    const int latSegments = segments;
    const int lonSegments = segments * 2;
    
    // Generate sphere vertices using spherical coordinates
    // We'll render the sphere as a series of triangle strips
    for (int lat = 0; lat < latSegments; ++lat) {
        float theta1 = static_cast<float>(lat) * M_PI / latSegments;
        float theta2 = static_cast<float>(lat + 1) * M_PI / latSegments;
        
        for (int lon = 0; lon < lonSegments; ++lon) {
            float phi1 = static_cast<float>(lon) * 2.0f * M_PI / lonSegments;
            float phi2 = static_cast<float>(lon + 1) * 2.0f * M_PI / lonSegments;
            
            // Calculate the four vertices of this quad
            // v1: (lat, lon)
            float x1 = centerX + radius * std::sin(theta1) * std::cos(phi1);
            float y1 = centerY + radius * std::cos(theta1);
            float z1 = centerZ + radius * std::sin(theta1) * std::sin(phi1);
            
            // v2: (lat+1, lon)
            float x2 = centerX + radius * std::sin(theta2) * std::cos(phi1);
            float y2 = centerY + radius * std::cos(theta2);
            float z2 = centerZ + radius * std::sin(theta2) * std::sin(phi1);
            
            // v3: (lat+1, lon+1)
            float x3 = centerX + radius * std::sin(theta2) * std::cos(phi2);
            float y3 = centerY + radius * std::cos(theta2);
            float z3 = centerZ + radius * std::sin(theta2) * std::sin(phi2);
            
            // v4: (lat, lon+1)
            float x4 = centerX + radius * std::sin(theta1) * std::cos(phi2);
            float y4 = centerY + radius * std::cos(theta1);
            float z4 = centerZ + radius * std::sin(theta1) * std::sin(phi2);
            
            // Add shading based on latitude for better depth perception
            float shading1 = 0.5f + 0.5f * std::cos(theta1);
            float shading2 = 0.5f + 0.5f * std::cos(theta2);
            
            float r1 = r * shading1;
            float g1 = g * shading1;
            float b1 = b * shading1;
            
            float r2 = r * shading2;
            float g2 = g * shading2;
            float b2 = b * shading2;
            
            // Draw two triangles to form a quad
            // Triangle 1: v1, v2, v3
            App::DrawTriangle(
                x1, y1, z1, 1.0f,
                x2, y2, z2, 1.0f,
                x3, y3, z3, 1.0f,
                r1, g1, b1,
                r2, g2, b2,
                r2, g2, b2,
                false
            );
            
            // Triangle 2: v1, v3, v4
            App::DrawTriangle(
                x1, y1, z1, 1.0f,
                x3, y3, z3, 1.0f,
                x4, y4, z4, 1.0f,
                r1, g1, b1,
                r2, g2, b2,
                r1, g1, b1,
                false
            );
        }
    }
}

// Test cube renderer (12 triangles). Uses depth test inside App::DrawTriangle.
void RenderHelper::RenderCube_inNDC(const Transform3D& t, const Camera3D& camera)
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
    //float x[8], y[8], z[8], w[8];
    float x[8] = { 0 }, y[8] = { 0 }, z[8] = { 0 }, w[8] = { 0 };
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
		const float dot = Dot3(toCam, n);
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


// Helper function to render a 3D cube with all 6 faces
void RenderHelper::RenderCube(const Transform3D& t, const Camera3D& camera) {
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
void RenderHelper::RenderPlayerCube(const Transform3D& t, const Camera3D& camera) {
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
