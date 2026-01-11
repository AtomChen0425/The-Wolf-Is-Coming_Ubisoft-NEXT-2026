#pragma once
#include "../Math/Vec2.h"
#include "../Math/Vec3.h"

struct IsoCamera
{
    float tileW = 128.0f;
    float tileH = 64.0f;


    float heightPx = 48.0f;

    Vec2 origin{ 512.0f, 140.0f };


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