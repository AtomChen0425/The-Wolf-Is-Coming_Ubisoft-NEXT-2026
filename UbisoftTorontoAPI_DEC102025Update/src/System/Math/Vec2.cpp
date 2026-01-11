#include "Vec2.h"
#include <cmath>

float Dot(Vec2 a, Vec2 b) { return a.x * b.x + a.y * b.y; }
float LenSq(Vec2 v) { return Dot(v, v); }
float Len(Vec2 v) { return sqrt(LenSq(v)); }

Vec2 Normalize(Vec2 v)
{
	const float l = Len(v);
	if (l <= 1e-6f) return { 0.0f, 0.0f };
	return { v.x / l, v.y / l };
}

float Clamp(float v, float lo, float hi)
{
	return (v < lo) ? lo : (v > hi) ? hi : v;
}

float Rand01()
{
	return float(rand()) / float(RAND_MAX);
}

float RandRange(float a, float b)
{
	return a + (b - a) * Rand01();
}