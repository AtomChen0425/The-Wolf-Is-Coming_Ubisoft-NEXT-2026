#pragma once
#include <cmath>

namespace Math3D
{
	struct Vec3
	{
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};

	inline Vec3 operator+(Vec3 a, Vec3 b) { return { a.x + b.x, a.y + b.y, a.z + b.z }; }
	inline Vec3 operator-(Vec3 a, Vec3 b) { return { a.x - b.x, a.y - b.y, a.z - b.z }; }
	inline Vec3 operator*(Vec3 a, float s) { return { a.x * s, a.y * s, a.z * s }; }

	inline float Dot(Vec3 a, Vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
	inline float LenSq(Vec3 v) { return Dot(v, v); }
	inline float Len(Vec3 v) { return std::sqrt(LenSq(v)); }

	inline Vec3 Normalize(Vec3 v)
	{
		const float l = Len(v);
		if (l <= 1e-6f) return { 0,0,0 };
		return { v.x / l, v.y / l, v.z / l };
	}
}