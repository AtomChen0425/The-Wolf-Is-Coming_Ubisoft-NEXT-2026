#pragma once
#include "../System/Math/Vec2.h"
struct Bullet
{
		Vec2 pos;
		Vec2 vel;
		float radius = 4.0f;
		float lifeMs = 1200.0f;
};