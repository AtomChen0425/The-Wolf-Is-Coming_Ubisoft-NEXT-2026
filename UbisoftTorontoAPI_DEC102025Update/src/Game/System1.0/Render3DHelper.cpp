
#include <cmath>
#include "Render3DHelper.h"
#include "../ContestAPI/app.h" 

namespace RenderIso
{
	static void Tri2D(Vec2 a, Vec2 b, Vec2 c,
		float r1, float g1, float b1,
		float r2, float g2, float b2,
		float r3, float g3, float b3,
		float z = 0.0f)
	{
		// 这里把 z 统一传 0（你也可以用 z 做额外深度效果）
		App::DrawTriangle(a.x, a.y, z, 1, b.x, b.y, z, 1, c.x, c.y, z, 1,
			r1, g1, b1, r2, g2, b2, r3, g3, b3, false);
	}

	void DrawBlock(const CameraIso& cam, Math3D::Vec3 c, float s, float h,
		float tr, float tg, float tb,
		float sr, float sg, float sb)
	{
		// 8个角点（其实只需要顶面4点 + 两个侧面）
		// 底面 y=0，顶面 y=h
		Math3D::Vec3 p000{ c.x - s, 0.0f, c.z - s };
		Math3D::Vec3 p100{ c.x + s, 0.0f, c.z - s };
		Math3D::Vec3 p010{ c.x - s, 0.0f, c.z + s };
		Math3D::Vec3 p110{ c.x + s, 0.0f, c.z + s };

		Math3D::Vec3 p001{ c.x - s, h, c.z - s };
		Math3D::Vec3 p101{ c.x + s, h, c.z - s };
		Math3D::Vec3 p011{ c.x - s, h, c.z + s };
		Math3D::Vec3 p111{ c.x + s, h, c.z + s };

		auto A = cam.Project(p001);
		auto B = cam.Project(p101);
		auto C = cam.Project(p111);
		auto D = cam.Project(p011);

		// 顶面（两三角）
		Tri2D(A, B, C, tr, tg, tb, tr, tg, tb, tr, tg, tb);
		Tri2D(A, C, D, tr, tg, tb, tr, tg, tb, tr, tg, tb);

		// 选择两个“可见侧面”：朝屏幕的两个面（根据我们的投影，通常 z+ 方向更靠上）
		// 这里固定画 “+z 面” 和 “+x 面”，简单但有效
		auto E = cam.Project(p010); // bottom -x +z
		auto F = cam.Project(p110); // bottom +x +z
		auto G = cam.Project(p111); // top +x +z
		auto H = cam.Project(p011); // top -x +z

		// +z 面
		Tri2D(E, F, G, sr, sg, sb, sr, sg, sb, sr, sg, sb);
		Tri2D(E, G, H, sr, sg, sb, sr, sg, sb, sr, sg, sb);

		auto I = cam.Project(p100); // bottom +x -z
		auto J = cam.Project(p110); // bottom +x +z
		auto K = cam.Project(p111); // top +x +z
		auto L = cam.Project(p101); // top +x -z

		// +x 面（稍微更暗一点更立体）
		Tri2D(I, J, K, sr * 0.85f, sg * 0.85f, sb * 0.85f, sr * 0.85f, sg * 0.85f, sb * 0.85f, sr * 0.85f, sg * 0.85f, sb * 0.85f);
		Tri2D(I, K, L, sr * 0.85f, sg * 0.85f, sb * 0.85f, sr * 0.85f, sg * 0.85f, sb * 0.85f, sr * 0.85f, sg * 0.85f, sb * 0.85f);
	}

	void DrawPlayer(const CameraIso& cam, Math3D::Vec3 p, float r, float ang)
	{
		// 顶面箭头（在 y = smallHeight）
		const float h = 18.0f; // 高度让它看起来像“立起来一点”
		Math3D::Vec3 f{ cos(ang), 0.0f, sin(ang) };
		Math3D::Vec3 right{ -f.z, 0.0f, f.x };

		Math3D::Vec3 tip = p + f * (r * 1.4f); tip.y = h;
		Math3D::Vec3 left = p - f * (r * 0.8f) - right * (r * 0.9f); left.y = h;
		Math3D::Vec3 rgt = p - f * (r * 0.8f) + right * (r * 0.9f); rgt.y = h;

		auto A = cam.Project(tip);
		auto B = cam.Project(rgt);
		auto C = cam.Project(left);

		Tri2D(A, B, C, 0.2f, 0.7f, 1.0f, 0.2f, 0.7f, 1.0f, 0.2f, 0.7f, 1.0f);

		// 简单画个侧面阴影（让它“3D”一点）：从顶面点拉到地面
		Math3D::Vec3 tip0 = tip; tip0.y = 0;
		Math3D::Vec3 rgt0 = rgt; rgt0.y = 0;

		auto D = cam.Project(tip0);
		auto E = cam.Project(rgt0);

		Tri2D(D, E, B, 0.12f, 0.35f, 0.5f, 0.12f, 0.35f, 0.5f, 0.12f, 0.35f, 0.5f);
		Tri2D(D, B, A, 0.12f, 0.35f, 0.5f, 0.12f, 0.35f, 0.5f, 0.12f, 0.35f, 0.5f);
	}
}