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

// 世界坐标 (逻辑坐标)
struct Position3D {
    float x; // 左右移动
    float z; // 前后移动 (屏幕上的上下，代表地板深度)
    float y; // 上下移动 (跳跃高度，0代表在地面)
};

// 速度
struct Velocity3D {
    float vx, vz, vy;
};

// 阴影组件 (2.5D 的灵魂)
struct Shadow {
    float radius;
};

struct Transform3D {
    float x, y, z;
    float width, height, depth; // 物体的大小
    float r, g, b; // 颜色
};

struct MapBlockTag {};