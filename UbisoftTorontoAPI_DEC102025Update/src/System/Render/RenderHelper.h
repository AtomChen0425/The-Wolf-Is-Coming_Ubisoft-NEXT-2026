#pragma once
#include "../System/Math/Vec2.h"

class RenderHelper
{
	public:
		void DrawQuad(Vec2 c, float half, float r, float g, float b);
        void DrawPlayer(Vec2 p, float angleRad);
};
