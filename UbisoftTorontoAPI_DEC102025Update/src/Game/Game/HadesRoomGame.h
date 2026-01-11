#pragma once
#include <vector>

#include "../../ContestAPI/SimpleSprite.h"
#include "../Math/Vec3.h"
#include "../World/TileMap.h"
#include "../Render/IsoRenderer.h"

class HadesRoomGame
{
public:
    void Init();
    void Update(float deltaTimeMs);
    void Render();
    void Shutdown();

private:
    TileMap     m_map;
    IsoRenderer m_renderer;

    // 玩家世界坐标（tile单位），在 XZ 平面移动
    Vec3 m_playerPos { 0,0,0 };
    Vec3 m_playerVel { 0,0,0 };

    CSimpleSprite* m_player = nullptr;
    CSimpleSprite* m_shadow = nullptr;

    std::vector<DrawItem> m_drawList;

    void BuildDrawList();
    bool IsWalkable(float x, float z) const;
};