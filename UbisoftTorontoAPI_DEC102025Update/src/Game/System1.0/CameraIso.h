#pragma once
#include "Math.h"
#include "Math3D.h"

namespace RenderIso
{
	struct CameraIso
	{
		// 屏幕中心（虚拟分辨率坐标）
		Vec2 screenCenter{ 512.0f, 384.0f };

		// 世界到屏幕的缩放
		float scale = 1.0f;

		// 斜投影参数：决定“倾角”和“压扁程度”
		float kx = 0.6f;  // z 对 x 的影响
		float kz = 1.0f;  // z 对 y 的影响
		float ky = 1.0f;  // y(高度) 对 y 的影响

		// 可选：世界原点偏移（相机跟随）
		Math3D::Vec3 focus{ 0,0,0 };

		Vec2 Project(const Math3D::Vec3& world) const
		{
			const float x = world.x - focus.x;
			const float y = world.y - focus.y;
			const float z = world.z - focus.z;

			Vec2 out;
			out.x = screenCenter.x + (x + z * kx) * scale;
			out.y = screenCenter.y + (-z * kz - y * ky) * scale;
			return out;
		}
	};
}