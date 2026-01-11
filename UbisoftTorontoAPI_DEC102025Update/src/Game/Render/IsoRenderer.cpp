#include "IsoRenderer.h"
#include "../../ContestAPI/app.h"
#include "../../ContestAPI/SimpleSprite.h"

#include <algorithm>

void IsoRenderer::DrawDiamondFloor(const Vec3& tilePos, float r, float g, float b) const
{
    // 一个 tile 的菱形：中心在 (x,z) 的地面点，四个角分别偏移 0.5 格
    Vec3 c = tilePos; c.y = 0.0f;

    Vec3 top    { c.x + 0.0f, 0.0f, c.z - 0.5f };
    Vec3 right  { c.x + 0.5f, 0.0f, c.z + 0.0f };
    Vec3 bottom { c.x + 0.0f, 0.0f, c.z + 0.5f };
    Vec3 left   { c.x - 0.5f, 0.0f, c.z + 0.0f };

    Vec2 T = cam.Project(top);
    Vec2 R = cam.Project(right);
    Vec2 B = cam.Project(bottom);
    Vec2 L = cam.Project(left);

    // 两三角填充菱形
    App::DrawTriangle(T.x, T.y, 0, 1, R.x, R.y, 0, 1, B.x, B.y, 0, 1, r,g,b, r,g,b, r,g,b, false);
    App::DrawTriangle(T.x, T.y, 0, 1, B.x, B.y, 0, 1, L.x, L.y, 0, 1, r,g,b, r,g,b, r,g,b, false);

    // 轻微描边（可选，像哈迪斯那种干净边）
    App::DrawLine(T.x, T.y, R.x, R.y, r*0.6f, g*0.6f, b*0.6f);
    App::DrawLine(R.x, R.y, B.x, B.y, r*0.6f, g*0.6f, b*0.6f);
    App::DrawLine(B.x, B.y, L.x, L.y, r*0.6f, g*0.6f, b*0.6f);
    App::DrawLine(L.x, L.y, T.x, T.y, r*0.6f, g*0.6f, b*0.6f);
}

void IsoRenderer::DrawWallFace(const DrawItem& it) const
{
    // it.a / it.b 是地面的一条边（y=0），height 是墙高（tile高度单位）
    Vec3 a0 = it.a; a0.y = 0.0f;
    Vec3 b0 = it.b; b0.y = 0.0f;
    Vec3 a1 = it.a; a1.y = it.height;
    Vec3 b1 = it.b; b1.y = it.height;

    Vec2 A0 = cam.Project(a0);
    Vec2 B0 = cam.Project(b0);
    Vec2 A1 = cam.Project(a1);
    Vec2 B1 = cam.Project(b1);

    // 用两个三角形画侧面矩形（侧面更暗）
    App::DrawTriangle(A0.x, A0.y, 0, 1, B0.x, B0.y, 0, 1, B1.x, B1.y, 0, 1,
                      it.sr,it.sg,it.sb, it.sr,it.sg,it.sb, it.sr,it.sg,it.sb, false);
    App::DrawTriangle(A0.x, A0.y, 0, 1, B1.x, B1.y, 0, 1, A1.x, A1.y, 0, 1,
                      it.sr,it.sg,it.sb, it.sr,it.sg,it.sb, it.sr,it.sg,it.sb, false);

    // 轮廓
    App::DrawLine(A0.x, A0.y, B0.x, B0.y, it.sr*0.7f, it.sg*0.7f, it.sb*0.7f);
    App::DrawLine(A0.x, A0.y, A1.x, A1.y, it.sr*0.7f, it.sg*0.7f, it.sb*0.7f);
    App::DrawLine(B0.x, B0.y, B1.x, B1.y, it.sr*0.7f, it.sg*0.7f, it.sb*0.7f);
    App::DrawLine(A1.x, A1.y, B1.x, B1.y, it.sr*0.7f, it.sg*0.7f, it.sb*0.7f);
}

void IsoRenderer::DrawSpriteBillboard(const DrawItem& it) const
{
    if (!it.sprite) return;

    // 世界脚点 -> 屏幕脚点
    Vec3 foot = it.worldPos; foot.y = 0.0f;
    Vec2 s = cam.Project(foot);

    it.sprite->SetScale(it.spriteScale);

    // CSimpleSprite 的锚点在中心，所以我们把 sprite 往上抬半个高度（再加自定义偏移）
    const float halfH = it.sprite->GetHeight() * 0.5f * it.spriteScale;
    it.sprite->SetPosition(s.x, s.y + halfH + it.yOffsetPx);
    it.sprite->Draw();
}

void IsoRenderer::RenderSorted(std::vector<DrawItem>& items) const
{
    std::sort(items.begin(), items.end(), [](const DrawItem& a, const DrawItem& b)
    {
        return a.sortKey < b.sortKey;
    });

    for (const auto& it : items)
    {
        switch (it.type)
        {
        case DrawItem::Type::WallFace:
            DrawWallFace(it);
            break;
        case DrawItem::Type::Shadow:
        case DrawItem::Type::Sprite:
            DrawSpriteBillboard(it);
            break;
        }
    }
}