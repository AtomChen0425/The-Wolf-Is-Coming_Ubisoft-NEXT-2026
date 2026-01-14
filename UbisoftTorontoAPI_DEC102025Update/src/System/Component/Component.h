#pragma once
#include "../ContestAPI/SimpleSprite.h"
#include "../System/Math/Vec2.h"
#include "../System/Math/Vec3.h"
enum {
    ANIM_FORWARDS,
    ANIM_BACKWARDS,
    ANIM_LEFT,
    ANIM_RIGHT,
};

// 1. λ�����
struct Position {
    Vec2 pos;
};

// 2. �ٶ����
struct Velocity {
    Vec2 vel;
};

// 3. ������� (���� API �Ķ���)
struct SpriteComponent {
    CSimpleSprite* sprite;
    int currentAnimID; // ��¼��ǰ���ŵĶ�������ֹÿ֡�ظ� SetAnimation
};

// 4. ��ұ�ǩ
struct PlayerTag {};
struct EnemyTag {};

// 5. �������
struct RigidBody {
    float mass;
	float radius;
    Vec2 force;
};
// 6. ����ֵ���
struct Health {
    int currentHealth;
    int maxHealth;
};

// �������� (�߼�����)
struct Position3D {
    float x; // �����ƶ�
    float z; // ǰ���ƶ� (��Ļ�ϵ����£������ذ����)
    float y; // �����ƶ� (��Ծ�߶ȣ�0�����ڵ���)
};

// �ٶ�
struct Velocity3D {
    float vx, vz, vy;
};

// ��Ӱ��� (2.5D �����)
struct Shadow {
    float radius;
};

// ========================================
// Vampire Survivors-style Components
// ========================================

// Enemy type definitions
enum class EnemyType {
    WEAK,       // Fast, low health, red
    NORMAL,     // Medium speed, medium health, green
    TANK,       // Slow, high health, blue
    ELITE       // Medium speed, high health, purple
};

// Enemy type component
struct EnemyTypeComponent {
    EnemyType type;
    Vec3 color;      // RGB color for rendering
    float speed;     // Movement speed multiplier
    int scoreValue;  // Score gained when killed
};

// Bullet component for auto-attack system
struct BulletTag {};

struct BulletComponent {
    float damage;
    float lifetime;     // Remaining lifetime in ms
    float maxLifetime;  // Maximum lifetime in ms
};

// Experience and leveling
struct Experience {
    int currentExp;
    int expToNextLevel;
    int level;
};

// Auto-weapon system
struct AutoWeapon {
    float cooldownMs;
    float currentCooldownMs;
    float bulletSpeed;
    float damage;
    float range;
};