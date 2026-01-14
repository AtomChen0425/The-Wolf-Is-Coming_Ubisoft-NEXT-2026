#include "GenerateSystem.h"
#include "../System/Component/Component.h"
#include "../ContestAPI/app.h"

// ========================================
// Enemy Stats Configuration
// ========================================
// Helper function to get enemy stats based on type
// This centralizes all enemy balance parameters
static void GetEnemyStats(EnemyType type, Vec3& color, float& speed, int& health, int& scoreValue, float& radius) {
    switch (type) {
        case EnemyType::WEAK:
            color = Vec3{ 1.0f, 0.2f, 0.2f };  // Red
            speed = 180.0f;
            health = 50;
            scoreValue = 10;
            radius = 15.0f;
            break;
        case EnemyType::NORMAL:
            color = Vec3{ 0.2f, 1.0f, 0.2f };  // Green
            speed = 120.0f;
            health = 100;
            scoreValue = 20;
            radius = 20.0f;
            break;
        case EnemyType::TANK:
            color = Vec3{ 0.2f, 0.2f, 1.0f };  // Blue
            speed = 70.0f;
            health = 250;
            scoreValue = 50;
            radius = 30.0f;
            break;
        case EnemyType::ELITE:
            color = Vec3{ 0.8f, 0.2f, 0.8f };  // Purple
            speed = 130.0f;
            health = 200;
            scoreValue = 40;
            radius = 25.0f;
            break;
    }
}

// ========================================
// Player Creation
// ========================================

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
    
    // Add experience and auto-weapon
    registry.addComponent(entity, Experience{ 0, 100, 1 });
    registry.addComponent(entity, AutoWeapon{ 500.0f, 0.0f, 400.0f, 20.0f, 500.0f });

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

// ========================================
// Enemy Spawning
// ========================================
void GenerateSystem::SpawnEnemyOfType(EntityManager& registry, EnemyType type) {
    Entity enemy = registry.createEntity();
    
    // Get enemy stats based on type
    Vec3 color;
    float speed;
    int health;
    int scoreValue;
    float radius;
    GetEnemyStats(type, color, speed, health, scoreValue, radius);
    
    // Find player position for spawning enemies away from player
    View<PlayerTag, Position3D> view(registry);
    Vec2 playerPos = { 512.0f, 384.0f }; // Default center
    
    for (EntityID id : view) {
        auto& pos3d = view.get<Position3D>(id);
        playerPos = { pos3d.x, pos3d.z };
        break;
    }
    
    // Spawn enemies at screen edges (top, bottom, left, right)
    float side = Rand01();
    Vec2 pos;
    if (side < 0.25f) { pos = { 0.0f, RandRange(0, APP_VIRTUAL_HEIGHT) }; }
    else if (side < 0.5f) { pos = { APP_VIRTUAL_WIDTH, RandRange(0, APP_VIRTUAL_HEIGHT) }; }
    else if (side < 0.75f) { pos = { RandRange(0, APP_VIRTUAL_WIDTH), 0.0f }; }
    else { pos = { RandRange(0, APP_VIRTUAL_WIDTH), APP_VIRTUAL_HEIGHT }; }

    // Ensure minimum distance from player (avoid spawning on top of player)
    if (LenSq(pos - playerPos) < 200.0f * 200.0f)
        pos = pos + Normalize(pos - playerPos) * 220.0f;
    
    // Add all required components to enemy
    registry.addComponent(enemy, Position{ pos });
    registry.addComponent(enemy, Position3D{ pos.x, 0.0f, pos.y });  // pos.y is the z-coordinate (forward/back)
    registry.addComponent(enemy, Velocity{ Vec2{ 0.0f, 0.0f } });
    registry.addComponent(enemy, Velocity3D{ 0.0f, 0.0f, 0.0f });
    registry.addComponent(enemy, EnemyTag{});
    registry.addComponent(enemy, RigidBody{ 20.0f, radius, Vec2{ 0.0f, 0.0f } });
    registry.addComponent(enemy, Health{ health, health });
    registry.addComponent(enemy, EnemyTypeComponent{ type, color, speed, scoreValue });
}

void GenerateSystem::SpawnEnemy(EntityManager& registry) {
    // Randomly select enemy type with weighted probabilities
    // Spawn rates: WEAK=50%, NORMAL=30%, TANK=15%, ELITE=5%
    float roll = Rand01();
    EnemyType type;
    
    if (roll < 0.5f) {
        type = EnemyType::WEAK;        // 50% chance
    } else if (roll < 0.8f) {
        type = EnemyType::NORMAL;      // 30% chance
    } else if (roll < 0.95f) {
        type = EnemyType::TANK;        // 15% chance
    } else {
        type = EnemyType::ELITE;       // 5% chance
    }
    
    SpawnEnemyOfType(registry, type);
}

// ========================================
// Bullet Creation
// ========================================
void GenerateSystem::CreateBullet(EntityManager& registry, Vec2 position, Vec2 direction, float speed, float damage) {
    Entity bullet = registry.createEntity();
    
    // Calculate bullet velocity from direction and speed
    Vec2 velocity = Normalize(direction) * speed;
    
    // Add all bullet components
    registry.addComponent(bullet, Position{ position });
    registry.addComponent(bullet, Position3D{ position.x, 0.0f, position.y });  // position.y is z-coordinate
    registry.addComponent(bullet, Velocity{ velocity });
    registry.addComponent(bullet, BulletTag{});
    registry.addComponent(bullet, BulletComponent{ damage, 2000.0f, 2000.0f });  // 2 second lifetime
    registry.addComponent(bullet, RigidBody{ 1.0f, 4.0f, Vec2{ 0.0f, 0.0f } });  // Small radius for collision
}