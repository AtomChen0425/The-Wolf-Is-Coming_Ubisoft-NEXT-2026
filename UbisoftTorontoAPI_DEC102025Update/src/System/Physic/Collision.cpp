#include "../System/Physic/Collision.h"

bool Collision::AABB(const Vec2& minA, const Vec2& maxA, const Vec2& minB, const Vec2& maxB)
{
    return (minA.x < maxB.x && maxA.x > minB.x && minA.y < maxB.y && maxA.y > minB.y);
}

bool Collision::Circle(const Vec2& centerA, float radiusA, const Vec2& centerB, float radiusB)
{
    float dx = centerA.x - centerB.x;
    float dy = centerA.y - centerB.y;
    float distanceSquared = dx * dx + dy * dy;
    float radiusSum = radiusA + radiusB;
    return distanceSquared < (radiusSum * radiusSum);
}
