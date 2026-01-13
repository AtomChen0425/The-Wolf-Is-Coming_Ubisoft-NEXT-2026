#include "ControlSystem.h"
#include "../../ContestAPI/app.h"
#include "../../ContestAPI/AppSettings.h"
#include "../../System/Component/Component.h"
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

        //// ���û���ƶ�������ֹͣ���� (��ѡ)
        //if (vel.vel.x == 0 && vel.vel.y == 0) {
        //    // spr.sprite->SetAnimation(-1); // �����������ͣ�ڵ�ǰ֡������Ҫ��仰
        //}
    }
}
void PlayerControlSystem25D(EntityManager& registry) {
    auto& controller = App::GetController();

    View<Velocity3D,PlayerTag> view(registry); // ����������д��ٶȵ�����(ʵ��Ӧ��PlayerTag)

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

void PlayerControl3D(EntityManager& registry, float dt, Camera3D& camera, float& nextSpawnZ) {
    float dtSec = dt / 1000.0f;
    float playerCurrentZ = 0.0f;
    
    // Constants for physics
    const float gravity = -980.0f; // Gravity acceleration (pixels/s^2)
    const float groundLevel = 0.0f; // Ground Y position
    const float jumpVelocity = 400.0f; // Initial jump velocity
    const float forwardSpeed = 200.0f; // Forward/backward speed
    const float strafeSpeed = 300.0f; // Left/right strafe speed
    const float roadWidth = 100.0f; // Width of the road (5 blocks * 20 units each)
    const float roadMinX = -40.0f; // Left edge of road
    const float roadMaxX = 40.0f;  // Right edge of road

    View<PlayerTag, Transform3D, Velocity3D> view(registry);
    for (EntityID id : view) {
        auto& pos = view.get<Transform3D>(id);
        auto& vel = view.get<Velocity3D>(id);

        // 1. Handle horizontal input (forward/backward and strafe)
        vel.vz = 0.0f;
        vel.vx = 0.0f;
        
        if (App::IsKeyPressed(App::KEY_A)) vel.vx = -1.0f;
        if (App::IsKeyPressed(App::KEY_D)) vel.vx = 1.0f;
        if (App::IsKeyPressed(App::KEY_W)) vel.vz = 1.0f;
        if (App::IsKeyPressed(App::KEY_S)) vel.vz = -1.0f;

        // 2. Apply gravity
        vel.vy += gravity * dtSec;

        // 3. Handle jump input (only when on ground)
        if (App::IsKeyPressed(App::KEY_SPACE) && pos.y <= groundLevel + 0.1f) {
            vel.vy = jumpVelocity;
        }

        // 4. Update position based on velocity
        pos.z += vel.vz * forwardSpeed * dtSec;
        pos.x += vel.vx * strafeSpeed * dtSec;
        pos.y += vel.vy * dtSec;

        // 5. Keep player on the road (boundary constraints)
        if (pos.x < roadMinX) pos.x = roadMinX;
        if (pos.x > roadMaxX) pos.x = roadMaxX;

        // 6. Ground collision
        if (pos.y < groundLevel) {
            pos.y = groundLevel;
            vel.vy = 0.0f;
        }

        // Record player's current Z position for map generation
        playerCurrentZ = pos.z;
    }

    // 7. Update map generation system based on player position
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