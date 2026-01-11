#pragma once
#include <cmath>

struct Vec3
{
    float x = 0.0f, y = 0.0f, z = 0.0f;
};

inline Vec3 operator+(Vec3 a, Vec3 b) { return {a.x + b.x, a.y + b.y, a.z + b.z}; }
inline Vec3 operator-(Vec3 a, Vec3 b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
inline Vec3 operator*(Vec3 a, float s) { return {a.x * s, a.y * s, a.z * s}; }

inline float LenSqXZ(Vec3 v) { return v.x*v.x + v.z*v.z; }