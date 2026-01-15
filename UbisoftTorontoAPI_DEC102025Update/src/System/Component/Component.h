#pragma once
#include "../../ContestAPI/SimpleSprite.h"
#include "../Math/Vec2.h"
#include "../Math/Vec3.h"
#include <vector>

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
struct PlayerTag {
    bool isOnGround;
    float rotationY;
	int score;
};
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
    Vec3 vel;
};

// Shadow component (for 2.5D rendering)
struct Shadow {
    float radius;
};

// 3D Transform component
struct Transform3D {
    Vec3 pos;
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

// Solid block tag - for blocks that can be placed on floors (like tall blocks)
struct SolidBlockTag {};

// Block types for map generation
enum class BlockType {
    Empty = 0,      // No block
    Floor = 1,      // Normal floor block
    Wall = 2,       // Wall that blocks the player
    TallBlock = 3,  // Tall obstacle block
    ScorePoint = 4  // Collectible score point
};

// Map template for procedural generation
struct MapTemplate {
    int width;      // Width in blocks
    int depth;      // Depth in blocks
    std::vector<BlockType> blocks; // Block data (row-major: z * width + x)
    
    MapTemplate() : width(0), depth(0) {}
    
    MapTemplate(int w, int d) : width(w), depth(d) {
        blocks.resize(w * d, BlockType::Empty);
    }
    
    BlockType getBlock(int x, int z) const {
        if (!isValidPosition(x, z)) return BlockType::Empty;
        return blocks[z * width + x];
    }
    
    void setBlock(int x, int z, BlockType type) {
        if (!isValidPosition(x, z)) return;
        blocks[z * width + x] = type;
    }
    
private:
    bool isValidPosition(int x, int z) const {
        return x >= 0 && x < width && z >= 0 && z < depth;
    }
};

// Score point component for collectibles
struct ScorePointTag {
    int points;
    bool collected;
};

// Enemy movement component - makes enemy move toward player
struct EnemyMoveToPlayer {
    float speed;              // Movement speed
    float detectionRange;     // Range within which enemy detects and moves toward player
    bool isActive;            // Whether this component is currently active
};

// Enemy shooting component - makes enemy shoot projectiles at player
struct EnemyShootAtPlayer {
    float shootCooldown;      // Time between shots in milliseconds
    float timeSinceLastShot;  // Time elapsed since last shot
    float detectionRange;     // Range within which enemy can shoot at player
    float projectileSpeed;    // Speed of the projectiles
    bool isActive;            // Whether this component is currently active
};

// Bullet/Projectile component
struct Bullet {
    Vec3 direction;           // Direction vector (normalized)
    float speed;              // Movement speed
    float lifetime;           // How long the bullet lives (milliseconds)
    float damage;             // Damage dealt on hit
    bool isPlayerBullet;      // true if fired by player, false if fired by enemy
};