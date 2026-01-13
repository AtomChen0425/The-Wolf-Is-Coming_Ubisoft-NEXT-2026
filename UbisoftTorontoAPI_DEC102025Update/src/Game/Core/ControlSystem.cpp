#include "ControlSystem.h"
#include "../ContestAPI/app.h"
#include "../ContestAPI/AppSettings.h"
#include "../System/Component/Component.h"
#include "GenerateSystem.h"
void PlayerControlSystem(EntityManager& registry) {
    const auto& controller = App::GetController();
    View<Velocity, SpriteComponent, PlayerTag> view(registry);

    for (EntityID id : view) {
        auto& vel = view.get<Velocity>(id);
        auto& spr = view.get<SpriteComponent>(id);

        vel.vel.x = 0;
        vel.vel.y = 0;

        float moveSpeed = 1.0f;
        const float keep = 0.85f;
        Vec2 input{};
        input.x = controller.GetLeftThumbStickX();
        input.y = controller.GetLeftThumbStickY();
        Vec2 desiredMove = input * moveSpeed;
        vel.vel = vel.vel * keep + desiredMove;
        // Right
        if (input.x > 0.5f) {
            if (spr.currentAnimID != ANIM_RIGHT) { spr.sprite->SetAnimation(ANIM_RIGHT); spr.currentAnimID = ANIM_RIGHT; }
        }
        else if (input.x < -0.5f) {
            if (spr.currentAnimID != ANIM_LEFT) { spr.sprite->SetAnimation(ANIM_LEFT); spr.currentAnimID = ANIM_LEFT; }
        }

        if (input.y > 0.5f) {
            if (spr.currentAnimID != ANIM_FORWARDS) { spr.sprite->SetAnimation(ANIM_FORWARDS); spr.currentAnimID = ANIM_FORWARDS; }
        }
        else if (input.y < -0.5f) {
            if (spr.currentAnimID != ANIM_BACKWARDS) { spr.sprite->SetAnimation(ANIM_BACKWARDS); spr.currentAnimID = ANIM_BACKWARDS; }
        }

        //// 如果没有移动，可以停止动画 (可选)
        //if (vel.vel.x == 0 && vel.vel.y == 0) {
        //    // spr.sprite->SetAnimation(-1); // 如果你想让它停在当前帧，不需要这句话
        //}
    }
}
void PlayerControlSystem25D(EntityManager& registry) {
    auto& controller = App::GetController();

    View<Velocity3D,PlayerTag> view(registry); // 假设控制所有带速度的物体(实际应加PlayerTag)

    for (EntityID id : view) {
        auto& vel = view.get<Velocity3D>(id);
        auto& spr = view.get<SpriteComponent>(id);
        //float speed = 200.0f;

        float moveSpeed = 10.0f;
        const float keep = 0.85f;
        Vec2 input{};
        input.x = controller.GetLeftThumbStickX();
        input.y = controller.GetLeftThumbStickY();
        Vec2 desiredMove = input * moveSpeed;
        vel.vx = vel.vx * keep + desiredMove.x;
        vel.vz = vel.vz * keep + desiredMove.y;
        // Right
        if (input.x > 0.5f) {
            if (spr.currentAnimID != ANIM_RIGHT) { spr.sprite->SetAnimation(ANIM_RIGHT); spr.currentAnimID = ANIM_RIGHT; }
        }
        else if (input.x < -0.5f) {
            if (spr.currentAnimID != ANIM_LEFT) { spr.sprite->SetAnimation(ANIM_LEFT); spr.currentAnimID = ANIM_LEFT; }
        }

        if (input.y > 0.5f) {
            if (spr.currentAnimID != ANIM_FORWARDS) { spr.sprite->SetAnimation(ANIM_FORWARDS); spr.currentAnimID = ANIM_FORWARDS; }
        }
        else if (input.y < -0.5f) {
            if (spr.currentAnimID != ANIM_BACKWARDS) { spr.sprite->SetAnimation(ANIM_BACKWARDS); spr.currentAnimID = ANIM_BACKWARDS; }
        }

        // Y轴：跳跃 (Space)
        // 只有在地板上(y approx 0)才能跳
        // 这里简化写，实际上你需要检测 pos.y == 0
        if (App::IsKeyPressed(App::KEY_SPACE)) {
            // 这里通常需要一个防连跳判断
            // if (pos.y <= 0.1f) vel.vy = 300.0f; 
            vel.vy = 300.0f; // 临时测试，会有连跳
        }
    }
}

void PlayerControl3D(EntityManager& registry, float dt, Camera3D& camera,float& nextSpawnZ) {
    float dtSec = dt / 1000.0f;
    float playerCurrentZ = 0.0f;

    View<PlayerTag, Transform3D, Velocity3D> view(registry);
    for (EntityID id : view) {
        auto& pos = view.get<Transform3D>(id);
        auto& vel = view.get<Velocity3D>(id);

        // 1. 处理输入 (简单的自动奔跑 + 左右移动)
        vel.vz = 0.0f; // 永远向前跑！

        vel.vx = 0.0f;
        if (App::IsKeyPressed(App::KEY_A)) vel.vx = -1.0f;
        if (App::IsKeyPressed(App::KEY_D)) vel.vx = 1.0f;
        if (App::IsKeyPressed(App::KEY_W)) vel.vz = 1.0f;
        if (App::IsKeyPressed(App::KEY_S)) vel.vz = -1.0f;
        float forwardSpeed = 100.0f; // 前进速度
        float strafeSpeed = 300.0f;  // 横移速度

        // 2. 更新玩家位置
        pos.z += vel.vz * forwardSpeed * dtSec; // Z 轴一直增加
        pos.x += vel.vx * strafeSpeed * dtSec;

        // 记录玩家当前的 Z，用于地图生成
        playerCurrentZ = pos.z;

    }

    // 4. 驱动地图生成系统
    // 传入玩家最新的 Z 坐标，让地图系统决定是否要生成新路
    GenerateSystem::MapGenerationSystem(registry, playerCurrentZ, nextSpawnZ);
}
void ControlSystem::Update(EntityManager& registry) 
{
    //PlayerControlSystem(registry);
    PlayerControlSystem25D(registry);
}
void ControlSystem::Update(EntityManager& registry, float dt, Camera3D& camera, float& nextSpawnZ)
{
    PlayerControl3D(registry, dt, camera, nextSpawnZ);
}