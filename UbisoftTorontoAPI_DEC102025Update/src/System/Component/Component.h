#pragma once
#include "../../ContestAPI/SimpleSprite.h"
#include "../Math/Vec2.h"
#include "../Math/Vec3.h"

enum {
    ANIM_FORWARDS,
    ANIM_BACKWARDS,
    ANIM_LEFT,
    ANIM_RIGHT,
};

// 2D Position component
struct Position {
    Vec2 pos;
};

// 2D Velocity component
struct Velocity {
    Vec2 vel;
};

// Sprite component (uses Contest API sprites)
struct SpriteComponent {
    CSimpleSprite* sprite;
    int currentAnimID; // Track current animation to avoid redundant SetAnimation calls
};

// Tag components
struct PlayerTag {};
struct EnemyTag {};

// 2D Rigid body component
struct RigidBody {
    float mass;
    float radius;
    Vec2 force;
};

// Health component
struct Health {
    int currentHealth;
    int maxHealth;
};

// 3D Position component
struct Position3D {
    float x; // Left/right movement
    float z; // Forward/backward movement (depth into screen)
    float y; // Up/down movement (jump height, 0 = on ground)
};

// 3D Velocity component
struct Velocity3D {
    float vx, vz, vy;
};

// Shadow component (for 2.5D rendering)
struct Shadow {
    float radius;
};

// 3D Transform component
struct Transform3D {
    float x, y, z;              // Position
    float width, height, depth; // Size of the object
    float r, g, b;              // Color (normalized 0.0-1.0 range)
};

// 3D Collider component for collision detection
struct Collider3D {
    float width, height, depth; // Size of the collider box
    bool isFloor;               // True if this is a floor object
    bool isWall;                // True if this is a wall object
};

// Map block tag
struct MapBlockTag {};