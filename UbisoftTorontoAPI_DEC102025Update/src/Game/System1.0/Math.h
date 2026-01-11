#pragma once
#include <string>
#include <cmath>
#include <cstdlib>
float Clamp(float v, float lo, float hi);
float Rand01();
float RandRange(float a, float b);

struct Vec2
{
    float x, y;
	
	Vec2 operator+(const Vec2& rhs) const {
		return { x + rhs.x, y + rhs.y };
	}
	Vec2 operator-(const Vec2& rhs) const {
		return { x - rhs.x, y - rhs.y };
	}

	Vec2 operator*(const Vec2& rhs) const {
		return { x * rhs.x, y * rhs.y };
	}

	Vec2 operator/(const Vec2& rhs) const {
		return { x / rhs.x, y / rhs.y };
	}

	Vec2 operator*(const float rhs) const {
		return { x * rhs, y * rhs };
	}

	Vec2 operator/(const float rhs) const {
		return { x / rhs, y / rhs };
	}

	Vec2& operator+=(const Vec2& rhs) {
		x += rhs.x;
		y += rhs.y;
		return *this;
	}

	Vec2& operator-=(const Vec2& rhs) {
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}

	Vec2& operator*=(const Vec2& rhs) {
		x *= rhs.x;
		y *= rhs.y;
		return *this;
	}

	Vec2& operator/=(const float rhs) {
		x /= rhs;
		y /= rhs;
		return *this;
	}

	bool operator==(const Vec2& rhs) {
		return (x == rhs.x) && (y == rhs.y);
	}

	bool operator!=(const Vec2& rhs) {
		return (x != rhs.x) || (y != rhs.y);
	}

	
};
float Dot(Vec2 a, Vec2 b);
float LenSq(Vec2 v);
float Len(Vec2 v);
Vec2 Normalize(Vec2 v);