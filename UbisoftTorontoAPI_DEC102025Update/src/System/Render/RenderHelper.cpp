#include <cmath>
#include "RenderHelper.h"
#include "../../ContestAPI/app.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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
    // Clamp segments to reasonable values
    if (segments < 4) segments = 4;
    if (segments > 32) segments = 32;
    
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
