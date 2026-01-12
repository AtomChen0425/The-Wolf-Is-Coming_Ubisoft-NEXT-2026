#include "ECSSystem.h"
EngineSystem::EngineSystem()
    : registry(std::make_unique<EntityManager>())
{
}
void EngineSystem::CreatePlayer(EntityManager& registry)
{
    Entity entity = registry.createEntity();

    // A. 基础数据
    registry.addComponent(entity, Position{ Vec2{ 400.0f, 400.0f } }); // 初始位置
    registry.addComponent(entity, Velocity{ Vec2{ 0.0f, 0.0f } });     // 初始速度
    registry.addComponent(entity, PlayerTag{});                // 标记为玩家

    // B. 创建 Sprite (复制你的示例代码)
    CSimpleSprite* pSprite = App::CreateSprite("data/TestData/Test.bmp", 8, 4);

    // 设置动画配置
    const float speed = 1.0f / 15.0f;
    pSprite->CreateAnimation(ANIM_BACKWARDS, speed, { 0,1,2,3,4,5,6,7 });
    pSprite->CreateAnimation(ANIM_LEFT, speed, { 8,9,10,11,12,13,14,15 });
    pSprite->CreateAnimation(ANIM_RIGHT, speed, { 16,17,18,19,20,21,22,23 });
    pSprite->CreateAnimation(ANIM_FORWARDS, speed, { 24,25,26,27,28,29,30,31 });
    pSprite->SetScale(1.0f);

    // C. 存入组件
    // 初始动画设为 -1 或默认方向
    registry.addComponent(entity, SpriteComponent{ pSprite, 0 });
}

void EngineSystem::InputSystem(EntityManager& registry) {
    // 获取控制器
    const auto& controller = App::GetController();

    // 遍历所有玩家 (通常就 1 个)
    View<Velocity, SpriteComponent, PlayerTag> view(registry);

    for (EntityID id : view) {
        auto& vel = view.get<Velocity>(id);
        auto& spr = view.get<SpriteComponent>(id);

        // 1. 重置速度 (如果不按键就停下)
        vel.vel.x = 0;
        vel.vel.y = 0;

        float moveSpeed = 1.0f; // 定义移动速度

        // 2. 处理输入 -> 修改速度 & 切换动画
        // Right
        if (controller.GetLeftThumbStickX() > 0.5f) {
            vel.vel.x = moveSpeed;
            if (spr.currentAnimID != ANIM_RIGHT) {
                spr.sprite->SetAnimation(ANIM_RIGHT);
                spr.currentAnimID = ANIM_RIGHT;
            }
        }
        // Left
        else if (controller.GetLeftThumbStickX() < -0.5f) {
            vel.vel.x = -moveSpeed;
            if (spr.currentAnimID != ANIM_LEFT) {
                spr.sprite->SetAnimation(ANIM_LEFT);
                spr.currentAnimID = ANIM_LEFT;
            }
        }

        // Up (注意 Y 轴方向，通常屏幕向下是 +Y，但你的例子里 Up 是 y+=1 还是 y-=1？)
        // 假设你的例子里 Up 是 y+=1 (根据你的代码 y+=1.0f)
        if (controller.GetLeftThumbStickY() > 0.5f) {
            vel.vel.y = moveSpeed;
            if (spr.currentAnimID != ANIM_FORWARDS) {
                spr.sprite->SetAnimation(ANIM_FORWARDS);
                spr.currentAnimID = ANIM_FORWARDS;
            }
        }
        // Down
        else if (controller.GetLeftThumbStickY() < -0.5f) {
            vel.vel.y = -moveSpeed;
            if (spr.currentAnimID != ANIM_BACKWARDS) {
                spr.sprite->SetAnimation(ANIM_BACKWARDS);
                spr.currentAnimID = ANIM_BACKWARDS;
            }
        }

        // 如果没有移动，可以停止动画 (可选)
        if (vel.vel.x == 0 && vel.vel.y == 0) {
            // spr.sprite->SetAnimation(-1); // 如果你想让它停在当前帧，不需要这句话
        }
    }
}
void EngineSystem::MovementSystem(EntityManager& registry, float dt) {
    // 遍历所有有 Position 和 Velocity 的实体
    View<Position, Velocity> view(registry);
    for (EntityID id : view) {
        auto& pos = view.get<Position>(id);
        auto& vel = view.get<Velocity>(id);
        // 注意：如果 dt 是毫秒，可能需要调整系数
        pos.pos.x += vel.vel.x * dt;
        pos.pos.y += vel.vel.y * dt;
    }
}
void EngineSystem::RenderSystem(EntityManager& registry) {
    // 遍历所有有 SpriteComponent 和 Position 的实体
    View<Position, SpriteComponent> view(registry);
    for (EntityID id : view) {
        auto& pos = view.get<Position>(id);
        auto& spr = view.get<SpriteComponent>(id);

        if (spr.sprite) {
            // 1. 搬运工：把 ECS 的位置同步给 Sprite
            spr.sprite->SetPosition(pos.pos.x, pos.pos.y);

            // 2. 画家：画出来
            spr.sprite->Draw();
        }
    }
}
void EngineSystem::UpdateAnimation(EntityManager& registry, const float deltaTimeMs) {
    // 遍历所有有 SpriteComponent 和 Position 的实体
    View<SpriteComponent> view(registry);
    for (EntityID id : view) {
        auto& spr = view.get<SpriteComponent>(id);
        if (spr.sprite) {
            // 只更新动画帧，绝对不要在这里 Draw！
            spr.sprite->Update(deltaTimeMs);
        }
    }
}
void EngineSystem::ResetGame() {
    // 实现游戏重置逻辑，例如清空实体，重新创建玩家等
    registry = std::make_unique<EntityManager>();

    CreatePlayer(*registry);
}
void EngineSystem::Update(const float deltaTimeMs) {
    if (!registry) return;
    InputSystem(*registry);
    MovementSystem(*registry, deltaTimeMs);
    UpdateAnimation(*registry, deltaTimeMs);
}
void EngineSystem::Render() {
    if (!registry) return;
    RenderSystem(*registry);
}
void EngineSystem::Shutdown() {
    // 清理资源
    View<SpriteComponent> view(*registry);
    for (EntityID id : view) {
        auto& spr = view.get<SpriteComponent>(id);
        delete spr.sprite;
    }
    registry.reset();
}