#pragma once
#include "Math.h"
#include "Math3D.h"
#include "CameraIso.h"

namespace RenderIso
{
	// 画一个“立方体柱子”：底面在 y=0，上表面在 y=height
	// size 表示在 XZ 平面上的半边长
	void DrawBlock(const CameraIso& cam, Math3D::Vec3 centerXZ, float size, float height,
		float topR, float topG, float topB,
		float sideR, float sideG, float sideB);

	// 画玩家：一个在地面的“箭头”顶面 + 简单侧面
	void DrawPlayer(const CameraIso& cam, Math3D::Vec3 posXZ, float radius, float angleRad);

}