#pragma once
#include <string>
struct Vec2
{
	float x, y;
	Vec2()
		: x(0.0f), y(0.0f)
	{
	}

	Vec2(float x, float y)
		: x(x), y(y)
	{
	}

	Vec2 operator+(Vec2 const& v)
	{
		return Vec2(
			x + v.x,
			y + v.y);
	}

	Vec2 operator+=(Vec2 const& v)
	{
		x += v.x;
		y += v.y;

		return *this;
	}

	Vec2 operator-(Vec2 const& v)
	{
		return Vec2(
			x - v.x,
			y - v.y);
	}

	Vec2 operator-=(Vec2 const& v)
	{
		x -= v.x;
		y -= v.y;

		return *this;
	}

	Vec2 operator*(const Vec2& v) {
		return { x * v.x, y * v.y };
	}

	Vec2 operator*(const float v) const {
		return { x * v, y * v };
	}

	Vec2 operator*=(const Vec2& v) {
		x *= v.x;
		y *= v.y;
		return *this;
	}

	Vec2 operator/(const Vec2& v) {
		return { x / v.x, y / v.y };
	}

	Vec2 operator/=(const Vec2& v) {
		x /= v.x;
		y /= v.y;
		return *this;
	}

	bool operator==(const Vec2& v) const {
		return x == v.x && y == v.y;
	}

	bool operator!=(const Vec2& v) const {
		return !(*this == v);
	}

	std::string ToString() const {
		return "Vec2(" + std::to_string(x) + ", " + std::to_string(y) + ")";
	}
};
float Clamp(float v, float lo, float hi);
float Rand01();
float RandRange(float a, float b);
float Dot(Vec2 a, Vec2 b);
float LenSq(Vec2 v);
float Len(Vec2 v);
Vec2 Normalize(Vec2 v);