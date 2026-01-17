#include "GenerateSystem.h"
#include "../../System/Component/Component.h"
#include "Core/WolfSystem.h"
#include "../../ContestAPI/app.h"
#include <set>
#include <utility>

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

void GenerateSystem::MapGenerationSystem(EntityManager& registry, float playerZ, float& nextSpawnZ, const GameConfig& config) {
    const float blockSize = config.blockSize;       // Length of each road block
    const float renderDistance = config.renderDistance; // How far ahead to render
    const float deleteDistance = config.deleteDistance;  // How far behind to delete
    
    if (nextSpawnZ < blockSize) {
        nextSpawnZ = blockSize;
    }
    
    // --- 1. Spawn road sections ahead of player using templates ---
    while (nextSpawnZ < playerZ + renderDistance) {
        // Generate a random template for this section
        MapTemplate tmpl = CreateTestTemplate(config);
        GenerateMapFromTemplate(registry, tmpl, nextSpawnZ, config);
        
        // Create side walls for this section
        const int roadWidth = config.roadWidth;
        const float wallX = roadWidth / 2 * blockSize + blockSize / 2;
        
        // Left wall
        Entity leftWall = registry.createEntity();
        registry.addComponent(leftWall, Transform3D{
            Vec3{-wallX, config.wallHeight / 2.0f, nextSpawnZ},
            20.0f, config.wallHeight, blockSize,
            config.wallColorR, config.wallColorG, config.wallColorB  // Brown from config
        });
        registry.addComponent(leftWall, MapBlockTag{});
        registry.addComponent(leftWall, Collider3D{
            20.0f, config.wallHeight, blockSize,
            false, true  // not floor, isWall
        });
        
        // Right wall
        Entity rightWall = registry.createEntity();
        registry.addComponent(rightWall, Transform3D{
            Vec3{wallX, config.wallHeight / 2.0f, nextSpawnZ},
            20.0f, config.wallHeight, blockSize,
            config.wallColorR, config.wallColorG, config.wallColorB  // Brown from config
        });
        registry.addComponent(rightWall, MapBlockTag{});
        registry.addComponent(rightWall, Collider3D{
            20.0f, config.wallHeight, blockSize,
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
void GenerateSystem::CreatePlayer3D(EntityManager& registry, const GameConfig& config) {
    Entity entity = registry.createEntity();
    // Create player as a small cube, starting at center of road on the ground
    // Floor blocks are at Y=-10 with height=10, so top is at Y=-5
    // Player with height=30 should be at Y=-5+15=10 to stand on the ground
    WeaponInventory inventory;
    registry.addComponent(entity, Transform3D{ 
        Vec3{
            config.playerSpawnX,    // x: from config
            10.0f,   // y: on the ground (floor top at -5, player height/2 = 15, so 10)
            config.playerSpawnZ,   // z: from config
        },
        20.0f,   // width
        30.0f,   // height
        20.0f,   // depth
        config.playerColorR,    // r: from config
        config.playerColorG,    // g: from config
        config.playerColorB,    // b: from config
    });
    registry.addComponent(entity, Velocity3D{});
    registry.addComponent(entity, PlayerTag{true, 0, 0, 0});
    registry.addComponent(entity, PlayerStats{});  // Add player stats for upgrades
    registry.addComponent(entity, PhysicsTag{}); //
    registry.addComponent(entity, inventory);
}

// Create a default simple template
MapTemplate GenerateSystem::CreateDefaultTemplate(const GameConfig& config) {
    const int roadWidth = config.roadWidth;
    MapTemplate tmpl(roadWidth, 1); // roadWidth wide, 1 deep
    
    // Fill with floor blocks
    for (int x = 0; x < roadWidth; x++) {
        tmpl.setBlock(x, 0, BlockType::Floor);
    }
    
    return tmpl;
}

// Create a more interesting test template with obstacles
MapTemplate GenerateSystem::CreateTestTemplate(const GameConfig& config) {
    const int roadWidth = config.roadWidth;
    MapTemplate tmpl(roadWidth, 1);
    
    // Randomly decide what to put in this row
    float chance = Rand01();
    
    if (chance < config.normalFloorChance) {
        // Normal floor
        for (int x = 0; x < roadWidth; x++) {
            tmpl.setBlock(x, 0, BlockType::Floor);
        }
    } else if (chance < config.normalFloorChance + config.obstacleChance) {
        // Floor with a tall block obstacle in the middle
        for (int x = 0; x < roadWidth; x++) {
            tmpl.setBlock(x, 0, BlockType::Floor);
        }
        int obstacleX = (int)(Rand01() * roadWidth);
        tmpl.setBlock(obstacleX, 0, BlockType::TallBlock);
    } else if (chance < config.normalFloorChance + config.obstacleChance + config.scorePointChance) {
        // Floor with score points
        for (int x = 0; x < roadWidth; x++) {
            tmpl.setBlock(x, 0, BlockType::Floor);
        }
        int scoreX = (int)(Rand01() * roadWidth);
        tmpl.setBlock(scoreX, 0, BlockType::ScorePoint);
    } else if (chance < config.normalFloorChance + config.obstacleChance + config.scorePointChance + config.upgradePointChance) {
        // Floor with upgrade point (rare!)
        for (int x = 0; x < roadWidth; x++) {
            tmpl.setBlock(x, 0, BlockType::Floor);
        }
        // Don't add upgrade point to template, we'll spawn it separately in chunk generation
    } else {
        // Gap with some floor blocks missing
        for (int x = 0; x < roadWidth; x++) {
            if (Rand01() > config.gapFloorChance) {
                tmpl.setBlock(x, 0, BlockType::Floor);
            }
        }
    }
    
    return tmpl;
}

// Generate map blocks from a template at a specific Z position
void GenerateSystem::GenerateMapFromTemplate(EntityManager& registry, const MapTemplate& mapTemplate, float startZ, const GameConfig& config) {
    const float blockSize = config.blockSize;
    const float floorHeight = config.floorHeight;
    const float tallBlockHeight = config.tallBlockHeight;
    const float scorePointHeight = config.scorePointHeight;
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
                        r = config.floorColor1R; g = config.floorColor1G; b = config.floorColor1B; // Dark green
                    } else {
                        r = config.floorColor2R; g = config.floorColor2G; b = config.floorColor2B; // Light green
                    }
                    
                    registry.addComponent(block, Transform3D{
                        Vec3{blockX, -floorHeight / 2.0f, blockZ},
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
                        Vec3{blockX, config.wallHeight / 2.0f, blockZ},
                        blockSize, config.wallHeight, blockSize,
                        config.wallColorR, config.wallColorG, config.wallColorB  // Brown color from config
                    });
                    registry.addComponent(block, MapBlockTag{});
                    registry.addComponent(block, Collider3D{
                        blockSize, config.wallHeight, blockSize,
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
                        r_floor = config.floorColor1R; g_floor = config.floorColor1G; b_floor = config.floorColor1B; // Dark green
                    } else {
                        r_floor = config.floorColor2R; g_floor = config.floorColor2G; b_floor = config.floorColor2B; // Light green
                    }
                    
                    registry.addComponent(floorBlock, Transform3D{
                        Vec3{blockX, -floorHeight/2, blockZ},
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
                    registry.addComponent(block, Transform3D{
                        Vec3{blockX, tallBlockHeight/2- floorHeight / 2, blockZ},  // Sitting on floor
                        blockSize, tallBlockHeight, blockSize,
                        config.tallBlockColorR, config.tallBlockColorG, config.tallBlockColorB  // Blue from config
                    });
                    registry.addComponent(block, MapBlockTag{});
                    registry.addComponent(block, SolidBlockTag{});  // Mark as solid block
                    registry.addComponent(block, Collider3D{
                        blockSize, tallBlockHeight, blockSize,
                        true, false  // Can stand on top, not a wall
                    });
					registry.addComponent(block, Health{ 50,50 });
                    break;
                }
                
                case BlockType::ScorePoint: {
                    // Yellow score point on top of floor
                    // First create the floor
                    Entity floorBlock = registry.createEntity();
                    float r, g, b;
                    if (int(blockZ / blockSize) % 2 == 0) {
                        r = config.floorColor1R; g = config.floorColor1G; b = config.floorColor1B;
                    } else {
                        r = config.floorColor2R; g = config.floorColor2G; b = config.floorColor2B;
                    }
                    registry.addComponent(floorBlock, Transform3D{
                        Vec3{blockX, -floorHeight / 2, blockZ},
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
                        config.scorePointColorR, config.scorePointColorG, config.scorePointColorB  // Yellow from config
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
void GenerateChunk(EntityManager& registry, int chunkX, int chunkZ, const GameConfig& config) {
    const float blockSize = config.blockSize;
    const int chunkSize = config.chunkSize;
    const float floorHeight = config.floorHeight;
    const float chunkWorldSize = blockSize * chunkSize;

    // Calculate world position for this chunk
    float chunkWorldX = chunkX * chunkWorldSize;
    float chunkWorldZ = chunkZ * chunkWorldSize;

    // Generate only floor blocks for this chunk - simplified version
    for (int localZ = 0; localZ < chunkSize; localZ++) {
        for (int localX = 0; localX < chunkSize; localX++) {
            float blockX = chunkWorldX + localX * blockSize;
            float blockZ = chunkWorldZ + localZ * blockSize;

            // Create floor block
            Entity floor = registry.createEntity();

            // Alternate colors for visual depth (checkerboard pattern)
            float r, g, b;
            if ((localX + localZ) % 2 == 0) {
                r = config.floorColor1R;
                g = config.floorColor1G;
                b = config.floorColor1B;
            }
            else {
                r = config.floorColor2R;
                g = config.floorColor2G;
                b = config.floorColor2B;
            }

            registry.addComponent(floor, Transform3D{
                Vec3{blockX, -floorHeight / 2.0f, blockZ},
                blockSize, floorHeight, blockSize,
                r, g, b
                });
            registry.addComponent(floor, ChunkTag{ chunkX, chunkZ });
            registry.addComponent(floor, MapBlockTag{});
            registry.addComponent(floor, Collider3D{
                blockSize, floorHeight, blockSize,
                true, false  // isFloor, not wall
                });
        }
    }
}

void DespawnDistantChunks(EntityManager& registry, float playerX, float playerZ, std::set<std::pair<int, int>>& loadedChunks, const GameConfig& config) {
    const float blockSize = config.blockSize;
    const int chunkSize = config.chunkSize;
    const int renderRadius = config.chunkRenderRadius;
    const float chunkWorldSize = blockSize * chunkSize;
    const float despawnDistance = chunkWorldSize * (renderRadius + 2);  // Add buffer

    // Calculate player chunk
    int playerChunkX = (int)floor(playerX / chunkWorldSize);
    int playerChunkZ = (int)floor(playerZ / chunkWorldSize);

    // Find chunks to unload
    std::vector<std::pair<int, int>> chunksToUnload;
    for (const auto& chunkKey : loadedChunks) {
        int dx = chunkKey.first - playerChunkX;
        int dz = chunkKey.second - playerChunkZ;

        // If chunk is too far, mark for unloading
        if (abs(dx) > renderRadius + 1 || abs(dz) > renderRadius + 1) {
            chunksToUnload.push_back(chunkKey);
        }
    }

    // Despawn entities in chunks being unloaded
    for (const auto& chunkKey : chunksToUnload) {
        View<ChunkTag> view(registry);
        std::vector<EntityID> toDestroy;

        for (EntityID id : view) {
            auto& chunkTag = view.get<ChunkTag>(id);
            if (chunkTag.chunkX == chunkKey.first && chunkTag.chunkZ == chunkKey.second) {
                toDestroy.push_back(id);
            }
        }

        for (EntityID id : toDestroy) {
            registry.destroyEntity(id);
        }

        // Remove from loaded chunks
        loadedChunks.erase(chunkKey);
    }
}
// Chunk-based generation system for infinite 4-direction map
void GenerateSystem::ChunkGenerationSystem(EntityManager& registry, float playerX, float playerZ, std::set<std::pair<int, int>>& loadedChunks, const GameConfig& config) {
    const float blockSize = config.blockSize;
    const int chunkSize = config.chunkSize;
    const int renderRadius = config.chunkRenderRadius;
    const float chunkWorldSize = blockSize * chunkSize;
    
    // Calculate which chunk the player is in
    int playerChunkX = (int)floor(playerX / chunkWorldSize);
    int playerChunkZ = (int)floor(playerZ / chunkWorldSize);
    
    // Generate chunks around the player
    for (int dx = -renderRadius; dx <= renderRadius; dx++) {
        for (int dz = -renderRadius; dz <= renderRadius; dz++) {
            int chunkX = playerChunkX + dx;
            int chunkZ = playerChunkZ + dz;
            std::pair<int, int> chunkKey = {chunkX, chunkZ};
            
            // Only generate if not already loaded
            if (loadedChunks.find(chunkKey) == loadedChunks.end()) {
                GenerateChunk(registry, chunkX, chunkZ, config);
                loadedChunks.insert(chunkKey);
            }
        }
    }
    
    // Despawn distant chunks
    DespawnDistantChunks(registry, playerX, playerZ, loadedChunks, config);
}
void GenerateSystem::GenerateWolf(EntityManager& registry) {
    // Implementation for generating a wolf
    View<PlayerTag, Transform3D> playerView(registry);
    Vec3 playerPos;
    bool foundPlayer = false;
    for (EntityID id : playerView) {
        auto& playerTransform = playerView.get<Transform3D>(id);
        playerPos = playerTransform.pos;
        foundPlayer = true;
        break;
    }
    if (!foundPlayer) return;

    constexpr float kRadius = 3000.0f;
    constexpr float kPi = 3.14159265358979323846f;

    float theta = RandRange(0.0f, 2.0f * kPi);
    float r = sqrtf(Rand01()) * kRadius;

    float dx = cosf(theta) * r;
    float dz = sinf(theta) * r;

    Vec3 spawnPos{
        playerPos.x + dx,
        20.0f,          // keep same Y; adjust if you have a fixed ground height
        playerPos.z + dz
    };

    // 3) Create wolf entity
    Entity wolf = registry.createEntity();
    registry.addComponent(wolf, Transform3D{
                spawnPos,
                20.0f, 20.0f, 20.0f,  // Size (slightly bigger than sheep)
                0.4f, 0.2f, 0.1f      // Color (dark brown/gray)
        });
    registry.addComponent(wolf, Velocity3D{ Vec3{0,0,0} });
    registry.addComponent(wolf, WolfTag{});
    registry.addComponent(wolf, WolfComponent{}); // Use default parameters
    registry.addComponent(wolf, PhysicsTag{ true }); // Enable physics for ground collision
    registry.addComponent(wolf, EnemyTag{});
    registry.addComponent(wolf, Health{ 100, 100 }); // So they can be targeted like sheep
}
