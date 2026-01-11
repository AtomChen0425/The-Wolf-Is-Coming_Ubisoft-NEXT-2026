#pragma once
#include <vector>
#include "../Math/Rand.h"

struct TileMap
{
    int w = 0;
    int h = 0;


    std::vector<unsigned char> floor;

    std::vector<unsigned char> height;

    void Resize(int W, int H)
    {
        w = W; h = H;
        floor.assign(w * h, 0);
        height.assign(w * h, 0);
    }

    bool In(int x, int z) const { return x >= 0 && z >= 0 && x < w && z < h; }
    int  Idx(int x, int z) const { return z * w + x; }

    unsigned char& FloorAt(int x, int z) { return floor[Idx(x, z)]; }
    unsigned char& HeightAt(int x, int z) { return height[Idx(x, z)]; }
    unsigned char  FloorAt(int x, int z) const { return floor[Idx(x, z)]; }
    unsigned char  HeightAt(int x, int z) const { return height[Idx(x, z)]; }

    void GenerateRoom(int W, int H);
};