#include "GenerateSystem.h"
#include "../../System/Component/Component.h"
#include "../../ContestAPI/app.h"
void GenerateSystem::CreatePlayer(EntityManager& registry)
{
    Entity entity = registry.createEntity();

    // A. ��������
    registry.addComponent(entity, Position{ Vec2{ 400.0f, 400.0f } }); // ��ʼλ��
    registry.addComponent(entity, Position3D{ 400.0f, 400.0f,0.0f });
	registry.addComponent(entity, Velocity3D{ 0.0f, 0.0f, 0.0f });
    registry.addComponent(entity, Velocity{ Vec2{ 0.0f, 0.0f } });     // ��ʼ�ٶ�
    registry.addComponent(entity, PlayerTag{});                // ���Ϊ���
	registry.addComponent(entity, RigidBody{ 20.0f, 10.0f, Vec2{0.0f,0.0f} }); // �������
	registry.addComponent(entity, Health{ 100, 100 });          // ����ֵ���

    // B. ���� Sprite (�������ʾ������)
    CSimpleSprite* pSprite = App::CreateSprite("data/TestData/Test.bmp", 8, 4);

    // ���ö�������
    const float speed = 1.0f / 15.0f;
    pSprite->CreateAnimation(ANIM_BACKWARDS, speed, { 0,1,2,3,4,5,6,7 });
    pSprite->CreateAnimation(ANIM_LEFT, speed, { 8,9,10,11,12,13,14,15 });
    pSprite->CreateAnimation(ANIM_RIGHT, speed, { 16,17,18,19,20,21,22,23 });
    pSprite->CreateAnimation(ANIM_FORWARDS, speed, { 24,25,26,27,28,29,30,31 });
    pSprite->SetScale(1.0f);

    // C. �������
    // ��ʼ������Ϊ -1 ��Ĭ�Ϸ���
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
        registry.addComponent(enemy, RigidBody{ 20.0f, 20.0f, Vec2{ 0.0f, 0.0f } }); // �������
        registry.addComponent(enemy, Health{ 100, 100 });
    }
}

void GenerateSystem::MapGenerationSystem(EntityManager& registry, float playerZ, float& nextSpawnZ) {
    const float blockSize = 20.0f;       // Length of each road block
    const float renderDistance = 1000.0f; // How far ahead to render
    const float deleteDistance = 200.0f;  // How far behind to delete
    const int roadWidth = 5;             // Number of blocks wide (5 blocks = 100 units)
    
    // --- 1. Spawn road blocks ahead of player ---
    while (nextSpawnZ < playerZ + renderDistance) {
        for (int i = 0; i < roadWidth; i++) {
            Entity block = registry.createEntity();

            // Create road blocks in a line
            // Each block is 20 units wide, centered at x=0
            float blockX = -40.0f + i * 20.0f; // Position from -40 to +40
            
            // Alternate colors for visual depth
            float r, g, b;
            if (int(nextSpawnZ / blockSize) % 2 == 0) {
                r = 0.2f; g = 0.6f; b = 0.2f; // Dark green
            } else {
                r = 0.3f; g = 0.8f; b = 0.3f; // Light green
            }

            registry.addComponent(block, Transform3D{
                blockX,         // x position
                -10.0f,         // y position (below ground level)
                nextSpawnZ,     // z position (depth)
                20.0f,          // width
                10.0f,          // height
                blockSize,      // depth
                r, g, b         // color
            });
            registry.addComponent(block, MapBlockTag{});
        }
        
        nextSpawnZ += blockSize; // Move spawn position forward
    }

    // --- 2. Despawn blocks behind player ---
    View<MapBlockTag, Transform3D> view(registry);
    std::vector<EntityID> toDestroy;

    for (EntityID id : view) {
        auto& t = view.get<Transform3D>(id);
        // If block is too far behind player, mark for deletion
        if (t.z < playerZ - deleteDistance) {
            toDestroy.push_back(id);
        }
    }
    
    // Destroy marked entities
    for (EntityID id : toDestroy) {
        registry.destroyEntity(Entity{ id, registry.getEntityVersion(id) });
    }
}
void GenerateSystem::CreatePlayer3D(EntityManager& registry) {
    Entity entity = registry.createEntity();
    // Create player as a small cube, starting at center of road on the ground
    registry.addComponent(entity, Transform3D{ 
        0.0f,    // x: center of road
        0.0f,    // y: on the ground
        50.0f,   // z: slightly ahead
        20.0f,   // width
        20.0f,   // height
        20.0f,   // depth
        255.0f,  // r: red
        0.0f,    // g
        0.0f     // b
    });
    registry.addComponent(entity, Velocity3D{ 0.0f, 0.0f, 0.0f });
    registry.addComponent(entity, PlayerTag{});
}