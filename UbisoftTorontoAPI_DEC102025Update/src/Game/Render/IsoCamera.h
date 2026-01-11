#pragma once
#include "../Math/Vec2.h"
#include "../Math/Vec3.h"

struct IsoCamera
{
    // tile 像素尺寸（菱形宽高）
    float tileW = 128.0f;
    float tileH = 64.0f;

    // 每 1 高度对应的像素抬升
    float heightPx = 48.0f;

    // 屏幕原点偏移（把地图放到屏幕中间）
    Vec2 origin { 512.0f, 140.0f };

    // 将世界点投影到屏幕（世界单位=tile单位；x,z 是地面坐标；y是高度）
    Vec2 Project(const Vec3& p) const
    {
        const float halfW = tileW * 0.5f;
        const float halfH = tileH * 0.5f;

        Vec2 s;
        s.x = origin.x + (p.x - p.z) * halfW;
        s.y = origin.y + (p.x + p.z) * halfH - p.y * heightPx;
        return s;
    }
};