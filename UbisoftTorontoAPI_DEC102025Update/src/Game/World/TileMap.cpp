#include "TileMap.h"
#include <algorithm>

void TileMap::GenerateRoom(int W, int H)
{
    Resize(W, H);

    int roomW = RandInt(10, std::min(16, W - 2));
    int roomH = RandInt(10, std::min(16, H - 2));
    int rx = RandInt(1, W - roomW - 1);
    int rz = RandInt(1, H - roomH - 1);

    for (int z = rz; z < rz + roomH; z++)
        for (int x = rx; x < rx + roomW; x++)
            FloorAt(x, z) = 1;

    auto setWall = [&](int x, int z)
        {
            if (!In(x, z)) return;
            FloorAt(x, z) = 1;
            HeightAt(x, z) = 2;
        };

    for (int x = rx; x < rx + roomW; x++)
    {
        setWall(x, rz);
        setWall(x, rz + roomH - 1);
    }
    for (int z = rz; z < rz + roomH; z++)
    {
        setWall(rx, z);
        setWall(rx + roomW - 1, z);
    }


    int pillarCount = RandInt(4, 9);
    for (int i = 0; i < pillarCount; i++)
    {
        int px = RandInt(rx + 2, rx + roomW - 3);
        int pz = RandInt(rz + 2, rz + roomH - 3);

        if (std::abs(px - (rx + roomW / 2)) + std::abs(pz - (rz + roomH / 2)) < 4)
            continue;

        HeightAt(px, pz) = 2;
    }
}