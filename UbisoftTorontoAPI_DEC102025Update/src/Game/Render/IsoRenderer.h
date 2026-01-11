#pragma once
#include <vector>
#include "../Math/Vec2.h"
#include "../Math/Vec3.h"
#include "IsoCamera.h"

class CSimpleSprite;

struct DrawItem
{
    float sortKey = 0.0f; // screenY of footpoint
    enum class Type { WallFace, Sprite, Shadow } type;

    // common
    Vec3 worldPos; // for sorting / projection

    // wall face
    Vec3 a, b;      // bottom edge in world (y=0)
    float height = 0.0f;
    float r=1,g=1,bcol=1;
    float sr=1,sg=1,sb=1; // side color

    // sprite/shadow
    CSimpleSprite* sprite = nullptr;
    float spriteScale = 1.0f;
    float yOffsetPx = 0.0f; // shift sprite up/down in screen space
};

class IsoRenderer
{
public:
    IsoCamera cam;

    void DrawDiamondFloor(const Vec3& tilePos, float r, float g, float b) const;
    void DrawWallFace(const DrawItem& it) const;
    void DrawSpriteBillboard(const DrawItem& it) const;

    void RenderSorted(std::vector<DrawItem>& items) const;
};