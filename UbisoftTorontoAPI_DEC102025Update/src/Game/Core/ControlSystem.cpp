#include "ControlSystem.h"
#include "../ContestAPI/app.h"
#include "../ContestAPI/AppSettings.h"
#include "../System/Component/Component.h"
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

        //// ���û���ƶ�������ֹͣ���� (��ѡ)
        //if (vel.vel.x == 0 && vel.vel.y == 0) {
        //    // spr.sprite->SetAnimation(-1); // �����������ͣ�ڵ�ǰ֡������Ҫ��仰
        //}
    }
}
void PlayerControlSystem25D(EntityManager& registry) {
    auto& controller = App::GetController();

    View<Velocity3D, PlayerTag, SpriteComponent> view(registry);

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

        // Y�᣺��Ծ (Space)
        // ֻ���ڵذ���(y approx 0)������
        // �����д��ʵ��������Ҫ��� pos.y == 0
        if (App::IsKeyPressed(App::KEY_SPACE)) {
            // ����ͨ����Ҫһ���������ж�
            // if (pos.y <= 0.1f) vel.vy = 300.0f; 
            vel.vy = 300.0f; // ��ʱ���ԣ���������
        }
    }
}
void ControlSystem::Update(EntityManager& registry) 
{
    //PlayerControlSystem(registry);
    PlayerControlSystem25D(registry);
}