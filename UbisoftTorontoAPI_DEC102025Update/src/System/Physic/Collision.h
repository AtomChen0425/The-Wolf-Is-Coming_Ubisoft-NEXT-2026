#pragma once
#include "../System/Math/Vec2.h"
class Collision
{
public:
    bool AABB(const Vec2& minA, const Vec2& maxA, const Vec2& minB, const Vec2& maxB);
    bool Circle(const Vec2& centerA, float radiusA, const Vec2& centerB, float radiusB);
};
