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

// 1. 位置组件
struct Position {
    Vec2 pos;
};

// 2. 速度组件
struct Velocity {
    Vec2 vel;
};

// 3. 精灵组件 (持有 API 的对象)
struct SpriteComponent {
    CSimpleSprite* sprite;
    int currentAnimID; // 记录当前播放的动画，防止每帧重复 SetAnimation
};

// 4. 玩家标签
struct PlayerTag {};
struct EnemyTag {};

// 5. 刚体组件
struct RigidBody {
    float mass;
	float radius;
    Vec2 force;
};
// 6. 生命值组件
struct Health {
    int currentHealth;
    int maxHealth;
};
