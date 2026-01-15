#include "GenerateSystem.h"
#include "../../System/Component/Component.h"
#include "../../ContestAPI/app.h"
void GenerateSystem::CreatePlayer(EntityManager& registry)
{
    Entity entity = registry.createEntity();


    registry.addComponent(entity, Position{ Vec2{ 400.0f, 400.0f } }); // 
    registry.addComponent(entity, Position3D{ 400.0f, 400.0f,0.0f });
	registry.addComponent(entity, Velocity3D{});
    registry.addComponent(entity, Velocity{ Vec2{ 0.0f, 0.0f } });     // 
    registry.addComponent(entity, PlayerTag{ true });                // 
	registry.addComponent(entity, RigidBody{ 20.0f, 10.0f, Vec2{0.0f,0.0f} }); // 
	registry.addComponent(entity, Health{ 100, 100 });          //

    CSimpleSprite* pSprite = App::CreateSprite("data/TestData/Test.bmp", 8, 4);

    // 
    const float speed = 1.0f / 15.0f;
    pSprite->CreateAnimation(ANIM_BACKWARDS, speed, { 0,1,2,3,4,5,6,7 });
    pSprite->CreateAnimation(ANIM_LEFT, speed, { 8,9,10,11,12,13,14,15 });
    pSprite->CreateAnimation(ANIM_RIGHT, speed, { 16,17,18,19,20,21,22,23 });
    pSprite->CreateAnimation(ANIM_FORWARDS, speed, { 24,25,26,27,28,29,30,31 });
    pSprite->SetScale(1.0f);

    // 
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
        registry.addComponent(enemy, RigidBody{ 20.0f, 20.0f, Vec2{ 0.0f, 0.0f } }); 
        registry.addComponent(enemy, Health{ 100, 100 });
    }
}

void GenerateSystem::MapGenerationSystem(EntityManager& registry, float playerZ, float& nextSpawnZ) {
    const float blockSize = 100.0f;       // Length of each road block
    const float renderDistance = 1000.0f; // How far ahead to render
    const float deleteDistance = 1000.0f;  // How far behind to delete
    
    if (nextSpawnZ < blockSize) {
        nextSpawnZ = blockSize;
    }
    
    // --- 1. Spawn road sections ahead of player using templates ---
    while (nextSpawnZ < playerZ + renderDistance) {
        // Generate a random template for this section
        MapTemplate tmpl = CreateTestTemplate();
        GenerateMapFromTemplate(registry, tmpl, nextSpawnZ);
        
        // Create side walls for this section
        const int roadWidth = 5;
        const float wallX = roadWidth / 2 * blockSize + blockSize / 2;
        
        // Left wall
        Entity leftWall = registry.createEntity();
        registry.addComponent(leftWall, Transform3D{
            Vec3{-wallX, 50.0f, nextSpawnZ},
            20.0f, 100.0f, blockSize,
            0.6f, 0.3f, 0.1f  // Brown
        });
        registry.addComponent(leftWall, MapBlockTag{});
        registry.addComponent(leftWall, Collider3D{
            20.0f, 100.0f, blockSize,
            false, true  // not floor, isWall
        });
        
        // Right wall
        Entity rightWall = registry.createEntity();
        registry.addComponent(rightWall, Transform3D{
            Vec3{wallX, 50.0f, nextSpawnZ},
            20.0f, 100.0f, blockSize,
            0.6f, 0.3f, 0.1f  // Brown
        });
        registry.addComponent(rightWall, MapBlockTag{});
        registry.addComponent(rightWall, Collider3D{
            20.0f, 100.0f, blockSize,
            false, true  // not floor, isWall
        });
        
        nextSpawnZ += blockSize;
    }

    // --- 2. Despawn blocks behind player ---
    View<MapBlockTag, Transform3D> view(registry);
    std::vector<EntityID> toDestroy;

    for (EntityID id : view) {
        auto& t = view.get<Transform3D>(id);
        if (t.pos.z < playerZ - deleteDistance) {
            toDestroy.push_back(id);
        }
    }
    
    // Also despawn collected score points
    View<ScorePointTag, Transform3D> scoreView(registry);
    for (EntityID id : scoreView) {
        auto& scoreTag = scoreView.get<ScorePointTag>(id);
        auto& t = scoreView.get<Transform3D>(id);
        if (t.pos.z < playerZ - deleteDistance || scoreTag.collected) {
            toDestroy.push_back(id);
        }
    }
    
    for (EntityID id : toDestroy) {
        registry.destroyEntity(id);
    }
}
void GenerateSystem::CreatePlayer3D(EntityManager& registry) {
    Entity entity = registry.createEntity();
    // Create player as a small cube, starting at center of road on the ground
    // Floor blocks are at Y=-10 with height=10, so top is at Y=-5
    // Player with height=30 should be at Y=-5+15=10 to stand on the ground
    registry.addComponent(entity, Transform3D{ 
        Vec3{
            0.0f,    // x: center of road
            10.0f,   // y: on the ground (floor top at -5, player height/2 = 15, so 10)
            50.0f,   // z: slightly ahead
        },
        20.0f,   // width
        30.0f,   // height
        20.0f,   // depth
        1.0f,    // r: red
        0.0f,    // g
        0.0f,    // b
    });
    registry.addComponent(entity, Velocity3D{});
    registry.addComponent(entity, PlayerTag{true, 0, 0, 0});
}

// Create a default simple template
MapTemplate GenerateSystem::CreateDefaultTemplate() {
    const int roadWidth = 5;
    MapTemplate tmpl(roadWidth, 1); // 5 wide, 1 deep
    
    // Fill with floor blocks
    for (int x = 0; x < roadWidth; x++) {
        tmpl.setBlock(x, 0, BlockType::Floor);
    }
    
    return tmpl;
}

// Create a more interesting test template with obstacles
MapTemplate GenerateSystem::CreateTestTemplate() {
    const int roadWidth = 5;
    MapTemplate tmpl(roadWidth, 1);
    
    // Randomly decide what to put in this row
    float chance = Rand01();
    
    if (chance < 0.6f) {
        // 60% chance: Normal floor
        for (int x = 0; x < roadWidth; x++) {
            tmpl.setBlock(x, 0, BlockType::Floor);
        }
    } else if (chance < 0.75f) {
        // 15% chance: Floor with a tall block obstacle in the middle
        for (int x = 0; x < roadWidth; x++) {
            tmpl.setBlock(x, 0, BlockType::Floor);
        }
        int obstacleX = (int)(Rand01() * roadWidth);
        tmpl.setBlock(obstacleX, 0, BlockType::TallBlock);
    } else if (chance < 0.90f) {
        // 15% chance: Floor with score points
        for (int x = 0; x < roadWidth; x++) {
            tmpl.setBlock(x, 0, BlockType::Floor);
        }
        int scoreX = (int)(Rand01() * roadWidth);
        tmpl.setBlock(scoreX, 0, BlockType::ScorePoint);
    } else {
        // 10% chance: Gap with some floor blocks missing
        for (int x = 0; x < roadWidth; x++) {
            if (Rand01() > 0.3f) {
                tmpl.setBlock(x, 0, BlockType::Floor);
            }
        }
    }
    
    return tmpl;
}

// Generate map blocks from a template at a specific Z position
void GenerateSystem::GenerateMapFromTemplate(EntityManager& registry, const MapTemplate& mapTemplate, float startZ) {
    const float blockSize = 100.0f;
    const float floorHeight = 10.0f;
    const float tallBlockHeight = 60.0f;
    const float scorePointHeight = 30.0f;
    const int roadWidth = mapTemplate.width;
    
    for (int z = 0; z < mapTemplate.depth; z++) {
        for (int x = 0; x < mapTemplate.width; x++) {
            BlockType type = mapTemplate.getBlock(x, z);
            
            if (type == BlockType::Empty) continue;
            
            float blockX = -1 * (roadWidth / 2 * blockSize) + x * blockSize;
            float blockZ = startZ + z * blockSize;
            
            Entity block = registry.createEntity();
            
            switch (type) {
                case BlockType::Floor: {
                    // Alternate colors for visual depth
                    float r, g, b;
                    if (int(blockZ / blockSize) % 2 == 0) {
                        r = 0.2f; g = 0.6f; b = 0.2f; // Dark green
                    } else {
                        r = 0.3f; g = 0.8f; b = 0.3f; // Light green
                    }
                    
                    registry.addComponent(block, Transform3D{
                        Vec3{blockX, -10.0f, blockZ},
                        blockSize, floorHeight, blockSize,
                        r, g, b
                    });
                    registry.addComponent(block, MapBlockTag{});
                    registry.addComponent(block, Collider3D{
                        blockSize, floorHeight, blockSize,
                        true, false  // isFloor, not wall
                    });
                    break;
                }
                
                case BlockType::Wall: {
                    // Walls are vertical barriers
                    registry.addComponent(block, Transform3D{
                        Vec3{blockX, 50.0f, blockZ},
                        blockSize, 100.0f, blockSize,
                        0.6f, 0.3f, 0.1f  // Brown color
                    });
                    registry.addComponent(block, MapBlockTag{});
                    registry.addComponent(block, Collider3D{
                        blockSize, 100.0f, blockSize,
                        false, true  // not floor, isWall
                    });
                    break;
                }
                
                case BlockType::TallBlock: {
                    // Tall block = Floor block + Solid block on top
                    // This makes them modular - the solid block sits on the floor
                    
                    // First: Create the floor block (same as regular floor)
                    Entity floorBlock = registry.createEntity();
                    float r_floor, g_floor, b_floor;
                    if (int(blockZ / blockSize) % 2 == 0) {
                        r_floor = 0.2f; g_floor = 0.6f; b_floor = 0.2f; // Dark green
                    } else {
                        r_floor = 0.3f; g_floor = 0.8f; b_floor = 0.3f; // Light green
                    }
                    
                    registry.addComponent(floorBlock, Transform3D{
                        Vec3{blockX, -10.0f, blockZ},
                        blockSize, floorHeight, blockSize,
                        r_floor, g_floor, b_floor
                    });
                    registry.addComponent(floorBlock, MapBlockTag{});
                    registry.addComponent(floorBlock, Collider3D{
                        blockSize, floorHeight, blockSize,
                        true, false  // isFloor, not wall
                    });
                    
                    // Second: Create the solid block on top of the floor
                    // Floor top is at Y=-5, block height is 40, so center at Y=-5+20=15
                    const float solidBlockHeight = 40.0f;
                    registry.addComponent(block, Transform3D{
                        Vec3{blockX, 20.0f, blockZ},  // Sitting on floor
                        blockSize, solidBlockHeight, blockSize,
                        0.1f, 0.3f, 0.9f  // Blue
                    });
                    registry.addComponent(block, MapBlockTag{});
                    registry.addComponent(block, SolidBlockTag{});  // Mark as solid block
                    registry.addComponent(block, Collider3D{
                        blockSize, solidBlockHeight, blockSize,
                        true, false  // Can stand on top, not a wall
                    });
                    break;
                }
                
                case BlockType::ScorePoint: {
                    // Yellow score point on top of floor
                    // First create the floor
                    Entity floorBlock = registry.createEntity();
                    float r, g, b;
                    if (int(blockZ / blockSize) % 2 == 0) {
                        r = 0.2f; g = 0.6f; b = 0.2f;
                    } else {
                        r = 0.3f; g = 0.8f; b = 0.3f;
                    }
                    registry.addComponent(floorBlock, Transform3D{
                        Vec3{blockX, -10.0f, blockZ},
                        blockSize, floorHeight, blockSize,
                        r, g, b
                    });
                    registry.addComponent(floorBlock, MapBlockTag{});
                    registry.addComponent(floorBlock, Collider3D{
                        blockSize, floorHeight, blockSize,
                        true, false
                    });
                    
                    // Then create the score point above it
                    registry.addComponent(block, Transform3D{
                        Vec3{blockX, 10.0f, blockZ},
                        30.0f, scorePointHeight, 30.0f,
                        1.0f, 1.0f, 0.0f  // Yellow
                    });
                    registry.addComponent(block, MapBlockTag{});
                    registry.addComponent(block, ScorePointTag{10, false});
                    // No collider - player can walk through to collect
                    break;
                }
                
                default:
                    break;
            }
        }
    }
}

// Create a test enemy with AI components
// This function demonstrates how to create enemies with movement and shooting AI
void GenerateSystem::CreateTestEnemyWithAI(EntityManager& registry, float x, float y, float z, bool enableMovement, bool enableShooting) {
    Entity enemy = registry.createEntity();
    
    // Add Transform3D component for 3D positioning
    Transform3D transform;
    transform.pos = Vec3(x, y, z);
    transform.width = 30.0f;
    transform.height = 30.0f;
    transform.depth = 30.0f;
    transform.r = 1.0f;
    transform.g = 0.5f;
    transform.b = 0.0f;  // Orange color for enemies
    registry.addComponent(enemy, transform);
    
    // Add EnemyTag to mark this as an enemy
    registry.addComponent(enemy, EnemyTag{});
    
    // Add movement AI component if enabled
    if (enableMovement) {
        EnemyMoveToPlayer moveComponent;
        moveComponent.speed = 50.0f;           // 50 units per second
        moveComponent.detectionRange = 500.0f; // Can detect player within 500 units
        moveComponent.isActive = true;
        registry.addComponent(enemy, moveComponent);
    }
    
    // Add shooting AI component if enabled
    if (enableShooting) {
        EnemyShootAtPlayer shootComponent;
        shootComponent.shootCooldown = 2000.0f;      // Shoot every 2 seconds
        shootComponent.timeSinceLastShot = 0.0f;
        shootComponent.detectionRange = 400.0f;      // Can shoot player within 400 units
        shootComponent.projectileSpeed = 200.0f;     // Bullet speed: 200 units/sec
        shootComponent.isActive = true;
        registry.addComponent(enemy, shootComponent);
    }
}
