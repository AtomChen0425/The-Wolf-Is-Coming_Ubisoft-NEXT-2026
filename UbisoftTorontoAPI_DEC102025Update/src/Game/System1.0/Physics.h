#pragma once
#include "Math.h"

bool CircleHit(Vec2 a, float ra, Vec2 b, float rb)
{
	const float rr = ra + rb;
	return LenSq(a - b) <= rr * rr;
}