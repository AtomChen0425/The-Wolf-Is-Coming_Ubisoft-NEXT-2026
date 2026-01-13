#include "GenerateSystem.h"
#include "../System/Component/Component.h"
#include "../ContestAPI/app.h"
void GenerateSystem::CreatePlayer(EntityManager& registry)
{
    Entity entity = registry.createEntity();

    // A. 基础数据
    registry.addComponent(entity, Position{ Vec2{ 400.0f, 400.0f } }); // 初始位置
    registry.addComponent(entity, Position3D{ 400.0f, 400.0f,0.0f });
	registry.addComponent(entity, Velocity3D{ 0.0f, 0.0f, 0.0f });
    registry.addComponent(entity, Velocity{ Vec2{ 0.0f, 0.0f } });     // 初始速度
    registry.addComponent(entity, PlayerTag{});                // 标记为玩家
	registry.addComponent(entity, RigidBody{ 20.0f, 10.0f, Vec2{0.0f,0.0f} }); // 刚体组件
	registry.addComponent(entity, Health{ 100, 100 });          // 生命值组件

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
void GenerateSystem::SpawnEnemy(EntityManager& registry) {
    Entity enemy = registry.createEntity();
    View<PlayerTag> view(registry);
    for (EntityID id : view) {
        auto& gPlayerPosComponent = view.get<Position>(id);
        Vec2 gPlayerPos = gPlayerPosComponent.pos;
        float side = Rand01();
        Vec2 pos;
        if (side < 0.25f) { pos = { 0.0f, RandRange(0, APP_VIRTUAL_HEIGHT) }; }
        else if (side < 0.5f) { pos = { APP_VIRTUAL_WIDTH, RandRange(0, APP_VIRTUAL_HEIGHT) }; }
        else if (side < 0.75f) { pos = { RandRange(0, APP_VIRTUAL_WIDTH), 0.0f }; }
        else { pos = { RandRange(0, APP_VIRTUAL_WIDTH), APP_VIRTUAL_HEIGHT }; }

        if (LenSq(pos - gPlayerPos) < 200.0f * 200.0f)
            pos = pos + Normalize(pos - gPlayerPos) * 220.0f;
        registry.addComponent(enemy, Position{ pos });
        registry.addComponent(enemy, Velocity{ Vec2{ 0.0f, 0.0f } });
        registry.addComponent(enemy, EnemyTag{});
        registry.addComponent(enemy, RigidBody{ 20.0f, 20.0f, Vec2{ 0.0f, 0.0f } }); // 刚体组件
        registry.addComponent(enemy, Health{ 100, 100 });
    }
}