#include "TileMap.h"
#include <algorithm>

void TileMap::GenerateRoom(int W, int H)
{
    Resize(W, H);

    // 随机房间矩形
    int roomW = RandInt(10, std::min(16, W-2));
    int roomH = RandInt(10, std::min(16, H-2));
    int rx = RandInt(1, W - roomW - 1);
    int rz = RandInt(1, H - roomH - 1);

    // 填地板
    for (int z = rz; z < rz + roomH; z++)
        for (int x = rx; x < rx + roomW; x++)
            FloorAt(x,z) = 1;

    // 外墙：用高度=2 的柱条（放在房间边界上）
    auto setWall = [&](int x, int z)
    {
        if (!In(x,z)) return;
        FloorAt(x,z) = 1;
        HeightAt(x,z) = 2;
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

    // 随机内柱（高度=2）
    int pillarCount = RandInt(4, 9);
    for (int i = 0; i < pillarCount; i++)
    {
        int px = RandInt(rx + 2, rx + roomW - 3);
        int pz = RandInt(rz + 2, rz + roomH - 3);

        // 避免把出生点堵死（粗略）
        if (std::abs(px - (rx + roomW/2)) + std::abs(pz - (rz + roomH/2)) < 4)
            continue;

        HeightAt(px,pz) = 2;
    }
}