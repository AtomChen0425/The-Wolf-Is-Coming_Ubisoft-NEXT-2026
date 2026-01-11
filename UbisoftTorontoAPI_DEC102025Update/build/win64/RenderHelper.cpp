#include <cmath>
#include "../System/Render/RenderHelper.h"
#include "../ContestAPI/app.h"

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