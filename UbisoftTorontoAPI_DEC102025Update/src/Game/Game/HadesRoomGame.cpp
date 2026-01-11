#include "HadesRoomGame.h"
#include "../../ContestAPI/app.h"
#include "../../ContestAPI/AppSettings.h"
#include "../Math/Vec2.h"
#include "../Math/Rand.h"

#include <cmath>
#include <ctime>

enum
{
    ANIM_FORWARDS,
    ANIM_BACKWARDS,
    ANIM_LEFT,
    ANIM_RIGHT,
};
void HadesRoomGame::Init()
{
    std::srand((unsigned)std::time(nullptr));

    m_map.GenerateRoom(20, 20);

    m_renderer.cam.tileW = 128.0f;
    m_renderer.cam.tileH = 64.0f;
    m_renderer.cam.heightPx = 48.0f;
    m_renderer.cam.origin = { APP_VIRTUAL_WIDTH * 0.5f, 120.0f };

    m_player = App::CreateSprite("./data/TestData/Test.bmp", 8, 4);
    m_shadow = App::CreateSprite("./data/TestData/Test.bmp", 8, 4);

    const float speed = 1.0f / 15.0f;
    m_player->CreateAnimation(ANIM_BACKWARDS, speed, { 0,1,2,3,4,5,6,7 });
    m_player->CreateAnimation(ANIM_LEFT, speed, { 8,9,10,11,12,13,14,15 });
    m_player->CreateAnimation(ANIM_RIGHT, speed, { 16,17,18,19,20,21,22,23 });
    m_player->CreateAnimation(ANIM_FORWARDS, speed, { 24,25,26,27,28,29,30,31 });
    m_playerPos = { m_map.w * 0.5f, 0.0f, m_map.h * 0.5f };
    m_playerVel = { 0,0,0 };
}

bool HadesRoomGame::IsWalkable(float x, float z) const
{
    int tx = (int)std::floor(x);
    int tz = (int)std::floor(z);
    if (!m_map.In(tx, tz)) return false;
    if (m_map.FloorAt(tx, tz) == 0) return false;
    if (m_map.HeightAt(tx, tz) > 0) return false;
    return true;
}

void HadesRoomGame::Update(float deltaTimeMs)
{
    const float dt = deltaTimeMs / 1000.0f;

    float inX = App::GetController().GetLeftThumbStickX();
    float inZ = App::GetController().GetLeftThumbStickY(); 

    float len = std::sqrt(inX * inX + inZ * inZ);
    if (len > 1e-3f) { inX /= len; inZ /= len; }

    const float accel = 30.0f;     
    const float damping = 8.0f;

    m_playerVel.x += inX * accel * dt;
    m_playerVel.z += inZ * accel * dt;

    m_playerVel.x -= m_playerVel.x * damping * dt;
    m_playerVel.z -= m_playerVel.z * damping * dt;

    Vec3 next = m_playerPos + m_playerVel * dt;

    if (IsWalkable(next.x, m_playerPos.z)) m_playerPos.x = next.x; else m_playerVel.x = 0;
    if (IsWalkable(m_playerPos.x, next.z)) m_playerPos.z = next.z; else m_playerVel.z = 0;


    BuildDrawList();

    {
        const Vec2 desired{ APP_VIRTUAL_WIDTH * 0.5f, APP_VIRTUAL_HEIGHT * 0.60f }; 
        const Vec2 playerScreen = m_renderer.cam.Project(Vec3{ m_playerPos.x, 0.0f, m_playerPos.z });
        m_renderer.cam.origin = m_renderer.cam.origin + (desired - playerScreen);
    }
}

void HadesRoomGame::BuildDrawList()
{
    m_drawList.clear();

    for (int z = 0; z < m_map.h; z++)
    {
        for (int x = 0; x < m_map.w; x++)
        {
            unsigned char h = m_map.HeightAt(x, z);
            if (h == 0) continue;

            auto neighH = [&](int nx, int nz)->unsigned char
                {
                    if (!m_map.In(nx, nz)) return 0;
                    return m_map.HeightAt(nx, nz);
                };

            const float fx = (float)x;
            const float fz = (float)z;

            {
                unsigned char nh = neighH(x + 1, z);
                if (h > nh)
                {
                    DrawItem it;
                    it.type = DrawItem::Type::WallFace;
                    it.a = { fx + 0.5f, 0.0f, fz - 0.5f };
                    it.b = { fx + 0.5f, 0.0f, fz + 0.5f };
                    it.height = float(h - nh);
                    it.sr = 0.18f; it.sg = 0.18f; it.sb = 0.22f;
                    it.worldPos = { fx + 0.5f, 0.0f, fz };
                    it.sortKey = m_renderer.cam.Project(it.worldPos).y;
                    m_drawList.push_back(it);
                }
            }
            {
                unsigned char nh = neighH(x, z + 1);
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
    {
        DrawItem sh;
        sh.type = DrawItem::Type::Shadow;
        sh.sprite = m_shadow;
        sh.spriteScale = 0.7f;
        sh.worldPos = m_playerPos;
        sh.sortKey = m_renderer.cam.Project(Vec3{ m_playerPos.x, 0, m_playerPos.z }).y - 0.5f;
        sh.yOffsetPx = 0.0f;
        m_drawList.push_back(sh);
    }

    {
        DrawItem pl;
        pl.type = DrawItem::Type::Sprite;
        pl.sprite = m_player;
        pl.spriteScale = 1.0f;
        pl.worldPos = m_playerPos;
        pl.sortKey = m_renderer.cam.Project(Vec3{ m_playerPos.x, 0, m_playerPos.z }).y;
        pl.yOffsetPx = 0.0f;
        m_drawList.push_back(pl);
    }
}

void HadesRoomGame::Render()
{
    for (int z = 0; z < m_map.h; z++)
    {
        for (int x = 0; x < m_map.w; x++)
        {
            if (m_map.FloorAt(x, z) == 0) continue;

            float base = 0.20f + 0.02f * ((x + z) % 3);
            m_renderer.DrawDiamondFloor(Vec3{ (float)x, 0.0f, (float)z }, base, base, base + 0.03f);
        }
    }
    m_renderer.RenderSorted(m_drawList);

    App::Print(20, APP_VIRTUAL_HEIGHT - 30, "Hades-like 2.5D Room (Random)  Move: LeftStick/WASD", 1, 1, 1);
}

void HadesRoomGame::Shutdown()
{
    delete m_player; m_player = nullptr;
    delete m_shadow; m_shadow = nullptr;
}