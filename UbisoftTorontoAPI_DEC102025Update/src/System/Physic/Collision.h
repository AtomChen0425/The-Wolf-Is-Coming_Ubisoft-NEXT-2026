#pragma once
#include "../System/Math/Vec2.h"
#include "../System/Math/Vec3.h"
class Collision
{
public:
    bool AABB(const Vec2& minA, const Vec2& maxA, const Vec2& minB, const Vec2& maxB);
    bool Circle(const Vec2& centerA, float radiusA, const Vec2& centerB, float radiusB);
    bool AABB3D(const Vec3& minA, const Vec3& maxA, const Vec3& minB, const Vec3& maxB);
};
