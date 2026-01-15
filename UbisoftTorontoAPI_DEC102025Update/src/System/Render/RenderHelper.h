#pragma once
#include "../Math/Vec2.h"

class RenderHelper
{
public:
	void DrawQuad(Vec2 c, float half, float r, float g, float b);
	void DrawPlayer(Vec2 p, float angleRad);
	void DrawShadow(float x, float y, float ShadowSize);
	void DrawSphere(float centerX, float centerY, float centerZ, float radius, float r, float g, float b, int segments = 16);
};
