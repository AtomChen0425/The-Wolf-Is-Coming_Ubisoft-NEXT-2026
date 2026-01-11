#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#include "../ContestAPI/app.h"
#include "../ContestAPI/AppSettings.h" // 为了用 APP_VIRTUAL_WIDTH/HEIGHT

// ----------------------------
// 小游戏：吃星星 (30秒)
// 左摇杆/WSAD 移动方块，碰到星星加分，时间到结束
// ----------------------------

static float gPlayerX = 512.0f;
static float gPlayerY = 384.0f;
static float gPlayerRadius = 18.0f;

static float gStarX = 200.0f;
static float gStarY = 200.0f;
static float gStarRadius = 12.0f;

static int   gScore = 0;
static float gTimeLeftMs = 30000.0f; // 30 秒
static bool  gGameOver = false;

// 简单随机
static float RandRange(float a, float b)
{
    return a + (b - a) * (float(rand()) / float(RAND_MAX));
}

static void RespawnStar()
{
    // 留点边距，避免刷在边上
    gStarX = RandRange(50.0f, APP_VIRTUAL_WIDTH - 50.0f);
    gStarY = RandRange(50.0f, APP_VIRTUAL_HEIGHT - 50.0f);
}

static float LengthSq(float dx, float dy) { return dx * dx + dy * dy; }

// 用两个三角形画一个“方块”
static void DrawQuad(float cx, float cy, float half, float r, float g, float b)
{
    // 三角形1：左下-右下-右上
    App::DrawTriangle(cx - half, cy - half, 0, 1,
        cx + half, cy - half, 0, 1,
        cx + half, cy + half, 0, 1,
        r, g, b, r, g, b, r, g, b, false);
    // 三角形2：左下-右上-左上
    App::DrawTriangle(cx - half, cy - half, 0, 1,
        cx + half, cy + half, 0, 1,
        cx - half, cy + half, 0, 1,
        r, g, b, r, g, b, r, g, b, false);
}

// 用三个三角形画一个“星星感”的图标（很简陋但够用）
static void DrawStar(float x, float y, float s, float r, float g, float b)
{
    App::DrawTriangle(x, y + s, 0, 1, x - s, y - s, 0, 1, x + s, y - s, 0, 1, r, g, b, r, g, b, r, g, b, false);
    App::DrawTriangle(x, y - s, 0, 1, x - s, y + s, 0, 1, x + s, y + s, 0, 1, r, g, b, r, g, b, r, g, b, false);
    App::DrawTriangle(x - s, y, 0, 1, x + s, y, 0, 1, x, y + s * 1.2f, 0, 1, r, g, b, r, g, b, r, g, b, false);
}

void Init()
{
    std::srand((unsigned)std::time(nullptr));
    gPlayerX = APP_VIRTUAL_WIDTH * 0.5f;
    gPlayerY = APP_VIRTUAL_HEIGHT * 0.5f;
    gScore = 0;
    gTimeLeftMs = 30000.0f;
    gGameOver = false;
    RespawnStar();
}

void Update(const float deltaTimeMs)
{
    if (gGameOver)
    {
        // 按 R 重开（键盘）
        if (App::IsKeyPressed(App::KEY_R))
        {
            Init();
        }
        return;
    }

    gTimeLeftMs -= deltaTimeMs;
    if (gTimeLeftMs <= 0.0f)
    {
        gTimeLeftMs = 0.0f;
        gGameOver = true;
        return;
    }

    // 移动：支持柄左摇杆，也支持 WSAD（因为手柄会在无手柄时用键盘模拟）
    float moveX = App::GetController().GetLeftThumbStickX(); // -1..1
    float moveY = App::GetController().GetLeftThumbStickY(); // -1..1

    // 速度：像素/秒 => 换算成像素/ms
    const float speedPxPerSec = 260.0f;
    const float speedPxPerMs = speedPxPerSec / 1000.0f;

    gPlayerX += moveX * speedPxPerMs * deltaTimeMs;
    gPlayerY += moveY * speedPxPerMs * deltaTimeMs;

    // 边界限制
    if (gPlayerX < gPlayerRadius) gPlayerX = gPlayerRadius;
    if (gPlayerY < gPlayerRadius) gPlayerY = gPlayerRadius;
    if (gPlayerX > APP_VIRTUAL_WIDTH - gPlayerRadius)  gPlayerX = APP_VIRTUAL_WIDTH - gPlayerRadius;
    if (gPlayerY > APP_VIRTUAL_HEIGHT - gPlayerRadius) gPlayerY = APP_VIRTUAL_HEIGHT - gPlayerRadius;

    // 碰撞：圆形近似
    float dx = gPlayerX - gStarX;
    float dy = gPlayerY - gStarY;
    float hitDist = gPlayerRadius + gStarRadius;
    if (LengthSq(dx, dy) <= hitDist * hitDist)
    {
        gScore += 1;
        RespawnStar();
    }
}

void Render()
{
    // 画玩家（蓝色方块）
    DrawQuad(gPlayerX, gPlayerY, gPlayerRadius, 0.2f, 0.6f, 1.0f);

    // 画星星（金色）
    DrawStar(gStarX, gStarY, gStarRadius, 1.0f, 0.85f, 0.1f);

    // UI
    char buf[128];
    std::snprintf(buf, sizeof(buf), "Score: %d", gScore);
    App::Print(20, APP_VIRTUAL_HEIGHT - 30, buf, 1, 1, 1);

    std::snprintf(buf, sizeof(buf), "Time: %.1f", gTimeLeftMs / 1000.0f);
    App::Print(200, APP_VIRTUAL_HEIGHT - 30, buf, 1, 1, 1);

    App::Print(20, 20, "Move: LeftStick / (keyboard emulation works). Restart: R", 0.8f, 0.8f, 0.8f);

    if (gGameOver)
    {
        App::Print(APP_VIRTUAL_WIDTH * 0.35f, APP_VIRTUAL_HEIGHT * 0.5f, "GAME OVER - Press R to Restart", 1, 0.4f, 0.4f);
    }
}

void Shutdown()
{
    // 这个版本没 new 任何资源，所以不需要释放
}