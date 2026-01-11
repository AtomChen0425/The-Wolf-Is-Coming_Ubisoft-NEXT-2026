#include "HadesRoomGame.h"
#include "../../ContestAPI/app.h"
#include "../../ContestAPI/AppSettings.h"
#include "../Math/Vec2.h"
#include "../Math/Rand.h"

#include <cmath>
#include <ctime>

void HadesRoomGame::Init()
{
    std::srand((unsigned)std::time(nullptr));

    // 随机生成房间
    m_map.GenerateRoom(20, 20);

    // 相机参数（你可以按美术再调）
    m_renderer.cam.tileW = 128.0f;
    m_renderer.cam.tileH = 64.0f;
    m_renderer.cam.heightPx = 48.0f;
    m_renderer.cam.origin = { APP_VIRTUAL_WIDTH * 0.5f, 120.0f };

    // 加载 sprites（PNG 带透明）
    m_player = App::CreateSprite("./data/Sprites/player.png", 1, 1);
    m_shadow = App::CreateSprite("./data/Sprites/shadow.png", 1, 1);

    // 出生点：地图中心附近找一个 floor
    m_playerPos = { m_map.w * 0.5f, 0.0f, m_map.h * 0.5f };
    m_playerVel = { 0,0,0 };
}

bool HadesRoomGame::IsWalkable(float x, float z) const
{
    // 允许在 floor 上走；墙/柱子（height>0）不可走
    int tx = (int)std::floor(x);
    int tz = (int)std::floor(z);
    if (!m_map.In(tx,tz)) return false;
    if (m_map.FloorAt(tx,tz) == 0) return false;
    if (m_map.HeightAt(tx,tz) > 0) return false;
    return true;
}

void HadesRoomGame::Update(float deltaTimeMs)
{
    const float dt = deltaTimeMs / 1000.0f;

    // 项目自带输入：左摇杆（无手柄时 WASD 会模拟）
    float inX = App::GetController().GetLeftThumbStickX();
    float inZ = -App::GetController().GetLeftThumbStickY(); // 上下反向修正

    // 归一化（防止斜向更快）
    float len = std::sqrt(inX*inX + inZ*inZ);
    if (len > 1e-3f) { inX /= len; inZ /= len; }

    const float accel = 18.0f;     // tile/s^2
    const float damping = 8.0f;

    m_playerVel.x += inX * accel * dt;
    m_playerVel.z += inZ * accel * dt;

    m_playerVel.x -= m_playerVel.x * damping * dt;
    m_playerVel.z -= m_playerVel.z * damping * dt;

    Vec3 next = m_playerPos + m_playerVel * dt;

    // 简单碰撞：分轴尝试
    if (IsWalkable(next.x, m_playerPos.z)) m_playerPos.x = next.x; else m_playerVel.x = 0;
    if (IsWalkable(m_playerPos.x, next.z)) m_playerPos.z = next.z; else m_playerVel.z = 0;

    // 动画/朝向（先留空，你以后加 8方向动画很方便）
    // m_player->Update(deltaTimeMs);

    BuildDrawList();
}

void HadesRoomGame::BuildDrawList()
{
    m_drawList.clear();

    // 1) 墙面：根据高度差生成侧面（只做最基本：有高度的tile对邻居高度差 >0 就出墙面）
    for (int z = 0; z < m_map.h; z++)
    {
        for (int x = 0; x < m_map.w; x++)
        {
            unsigned char h = m_map.HeightAt(x,z);
            if (h == 0) continue;

            // 四邻居高度
            auto neighH = [&](int nx, int nz)->unsigned char
            {
                if (!m_map.In(nx,nz)) return 0;
                return m_map.HeightAt(nx,nz);
            };

            // 每个方向如果本格更高，生成一面墙
            // 墙面底边是 tile 的对应边（世界坐标以 tile 为单位，边在 +/-0.5）
            const float fx = (float)x;
            const float fz = (float)z;

            // +X 边（右边）
            {
                unsigned char nh = neighH(x+1,z);
                if (h > nh)
                {
                    DrawItem it;
                    it.type = DrawItem::Type::WallFace;
                    it.a = { fx + 0.5f, 0.0f, fz - 0.5f };
                    it.b = { fx + 0.5f, 0.0f, fz + 0.5f };
                    it.height = float(h - nh);
                    it.sr = 0.18f; it.sg = 0.18f; it.sb = 0.22f;
                    it.worldPos = { fx + 0.5f, 0.0f, fz }; // 用底边中心当排序点
                    it.sortKey = m_renderer.cam.Project(it.worldPos).y;
                    m_drawList.push_back(it);
                }
            }
            // +Z 边（下边）
            {
                unsigned char nh = neighH(x,z+1);
                if (h > nh)
                {
                    DrawItem it;
                    it.type = DrawItem::Type::WallFace;
                    it.a = { fx - 0.5f, 0.0f, fz + 0.5f };
                    it.b = { fx + 0.5f, 0.0f, fz + 0.5f };
                    it.height = float(h - nh);
                    it.sr = 0.22f; it.sg = 0.20f; it.sb = 0.18f;
                    it.worldPos = { fx, 0.0f, fz + 0.5f };
                    it.sortKey = m_renderer.cam.Project(it.worldPos).y;
                    m_drawList.push_back(it);
                }
            }
        }
    }

    // 2) 玩家阴影（先画阴影，再画人；但二者都参与排序，阴影 sortKey 略小一点）
    {
        DrawItem sh;
        sh.type = DrawItem::Type::Shadow;
        sh.sprite = m_shadow;
        sh.spriteScale = 0.7f;
        sh.worldPos = m_playerPos;
        sh.sortKey = m_renderer.cam.Project(Vec3{m_playerPos.x, 0, m_playerPos.z}).y - 0.5f;
        sh.yOffsetPx = 0.0f;
        m_drawList.push_back(sh);
    }

    // 3) 玩家 sprite（按脚底排序）
    {
        DrawItem pl;
        pl.type = DrawItem::Type::Sprite;
        pl.sprite = m_player;
        pl.spriteScale = 1.0f;
        pl.worldPos = m_playerPos;
        pl.sortKey = m_renderer.cam.Project(Vec3{m_playerPos.x, 0, m_playerPos.z}).y;
        pl.yOffsetPx = 0.0f;
        m_drawList.push_back(pl);
    }
}

void HadesRoomGame::Render()
{
    // 先画 floor（不排序）
    for (int z = 0; z < m_map.h; z++)
    {
        for (int x = 0; x < m_map.w; x++)
        {
            if (m_map.FloorAt(x,z) == 0) continue;

            // floor 颜色可做一点随机扰动更像哈迪斯
            float base = 0.20f + 0.02f * ((x + z) % 3);
            m_renderer.DrawDiamondFloor(Vec3{(float)x, 0.0f, (float)z}, base, base, base + 0.03f);
        }
    }

    // 再画排序项：墙面、阴影、玩家
    m_renderer.RenderSorted(m_drawList);

    App::Print(20, APP_VIRTUAL_HEIGHT - 30, "Hades-like 2.5D Room (Random)  Move: LeftStick/WASD", 1,1,1);
}

void HadesRoomGame::Shutdown()
{
    delete m_player; m_player = nullptr;
    delete m_shadow; m_shadow = nullptr;
}