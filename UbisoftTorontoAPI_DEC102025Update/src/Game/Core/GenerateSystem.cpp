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

void GenerateSystem::MapGenerationSystem(EntityManager& registry, float playerZ,float& nextSpawnZ) {
    float blockSize = 20.0f;      // 每个路块长度
    float renderDistance = 3000.0f; // 在玩家前方多远生成
    float deleteDistance = 500.0f;  // 在玩家后方多远销毁
    int roadWidth = 4;
    // --- 1. 生成 (Spawn) ---
    // 只要 "下一个生成点" 在 "玩家视野" 内，就一直循环生成
    while (nextSpawnZ < playerZ + renderDistance) {
        for (int i = 0; i <= roadWidth; i++) {
            Entity block = registry.createEntity();

            // 简单的颜色交替，产生斑马线效果，体现速度感
            //float color = (int(nextSpawnZ / blockSize) % 2 == 0) ? 0.7f : 0.4f;

            registry.addComponent(block, Transform3D{
                -40.0f+i*20.0f, -50.0f, nextSpawnZ,   // 位置 (Y在脚下, Z在前方)
                50.0f, 10.0f, blockSize,   // 宽, 高, 深
                0, 255, 0         // 灰色
                });
            registry.addComponent(block, MapBlockTag{}); // 打标签！
        }
        

        nextSpawnZ += blockSize; // 推进生成点
    }

    // --- 2. 销毁 (Despawn) ---
    View<MapBlockTag, Transform3D> view(registry);
    std::vector<EntityID> toDestroy;

    for (EntityID id : view) {
        auto& t = view.get<Transform3D>(id);
        // 如果路块的 Z 小于 玩家 Z 减去删除距离 (也就是说在玩家身后很远了)
        if (t.z < playerZ - deleteDistance) {
            toDestroy.push_back(id);
        }
    }
    // 统一销毁
    for (EntityID id : toDestroy) {
        registry.destroyEntity(Entity{id,registry.getEntityVersion(id)});
    }
}
void GenerateSystem::CreatePlayer3D(EntityManager& registry) {
    Entity entity = registry.createEntity();
    // 玩家是个小红块，初始在 (0,0,0)
    registry.addComponent(entity, Transform3D{ 0, 0, 0, 50, 50, 50, 255.0f, 0.0f, 0.0f });
    registry.addComponent(entity, Velocity3D{});
    registry.addComponent(entity, PlayerTag{});
}