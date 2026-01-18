#include "GenerateSystem.h"
#include "../../System/Component/Component.h"
#include "Core/WolfSystem.h"
#include "../../ContestAPI/app.h"
#include <set>
#include <utility>
#include <cmath>
#include <cstdlib>

//void GenerateSystem::CreatePlayer25D(EntityManager& registry)
//{
//    Entity entity = registry.createEntity();
//
//
//    registry.addComponent(entity, Position{ Vec2{ 400.0f, 400.0f } }); // 
//    registry.addComponent(entity, Position3D{ 400.0f, 400.0f,0.0f });
//    registry.addComponent(entity, Velocity3D{});
//    registry.addComponent(entity, Velocity{ Vec2{ 0.0f, 0.0f } });     // 
//    registry.addComponent(entity, PlayerTag{ true });                // 
//    registry.addComponent(entity, RigidBody{ 20.0f, 10.0f, Vec2{0.0f,0.0f} }); // 
//    registry.addComponent(entity, Health{ 100, 100 });          //
//
//    CSimpleSprite* pSprite = App::CreateSprite("data/TestData/Test.bmp", 8, 4);
//
//    // 
//    const float speed = 1.0f / 15.0f;
//    pSprite->CreateAnimation(ANIM_BACKWARDS, speed, { 0,1,2,3,4,5,6,7 });
//    pSprite->CreateAnimation(ANIM_LEFT, speed, { 8,9,10,11,12,13,14,15 });
//    pSprite->CreateAnimation(ANIM_RIGHT, speed, { 16,17,18,19,20,21,22,23 });
//    pSprite->CreateAnimation(ANIM_FORWARDS, speed, { 24,25,26,27,28,29,30,31 });
//    pSprite->SetScale(1.0f);
//
//    // 
//    registry.addComponent(entity, SpriteComponent{ pSprite, 0 });
//}
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
    const float blockSize = config.blockSize;       // Length of each road block
    const float renderDistance = config.renderDistance; // How far ahead to render
    const float deleteDistance = config.deleteDistance;  // How far behind to delete

    if (nextSpawnZ < blockSize) {
        nextSpawnZ = blockSize;
    }

    // --- 1. Spawn road sections ahead of player using templates ---
    while (nextSpawnZ < playerZ + renderDistance) {
        // Generate a random template for this section
        MapTemplate tmpl = CreateTestTemplate();
        GenerateMapFromTemplate(registry, tmpl, nextSpawnZ);

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
    std::vector<Entity> toDestroy;

    for (EntityID id : view) {
        auto& t = view.get<Transform3D>(id);
        if (t.pos.z < playerZ - deleteDistance) {
            toDestroy.push_back({ id, registry.getEntityVersion(id) });
        }
    }

    // Also despawn collected score points
    View<ScorePointTag, Transform3D> scoreView(registry);
    for (EntityID id : scoreView) {
        auto& scoreTag = scoreView.get<ScorePointTag>(id);
        auto& t = scoreView.get<Transform3D>(id);
        if (t.pos.z < playerZ - deleteDistance || scoreTag.collected) {
            toDestroy.push_back({ id, registry.getEntityVersion(id) });
        }
    }

    for (const Entity& e : toDestroy) {
        registry.destroyEntity(e);
    }
}
void GenerateSystem::CreatePlayer3D(EntityManager& registry) {
    Entity entity = registry.createEntity();
    WeaponInventory inventory;
    registry.addComponent(entity, Transform3D{
        Vec3{
            config.playerSpawnX,    
            10.0f,  
            config.playerSpawnZ,
        },
        20.0f,   // width
        30.0f,   // height
        20.0f,   // depth
        config.playerColorR,    
        config.playerColorG,    
        config.playerColorB,    
        });
    registry.addComponent(entity, Velocity3D{});
    registry.addComponent(entity, PlayerTag{ true, 0, 0, 0 });
    registry.addComponent(entity, PlayerStats{}); // Add player stats for upgrades
    registry.addComponent(entity, PhysicsTag{}); //
    Weapon startingWeapon;
    startingWeapon.type = WeaponType::Pistol;
    startingWeapon.name = "Basic Pistol";
    startingWeapon.damage = 20.0f;
    startingWeapon.fireRate = 50.0f;  // Fast fire rate
    startingWeapon.currentCooldown = 0.0f;
    startingWeapon.projectileSpeed = config.bulletSpeed;
    startingWeapon.projectileSize = 5.0f;
    startingWeapon.projectileLife = 1000.0f;
    startingWeapon.explosionRadius = 0.0f;
    startingWeapon.knockback = 300.0f;
    startingWeapon.r = 0.5f;
    startingWeapon.g = 0.0f;
    startingWeapon.b = 0.5f;
    inventory.weapons.push_back(startingWeapon);
    registry.addComponent(entity, inventory);
    registry.addComponent(entity, Health{ 100, 100 }); //
    CSimpleSprite* pSprite = App::CreateSprite("data/TestData/Test.bmp", 8, 4);

    // 
    const float speed = 1.0f / 15.0f;
    pSprite->CreateAnimation(ANIM_BACKWARDS, speed, { 0,1,2,3,4,5,6,7 });
    pSprite->CreateAnimation(ANIM_LEFT, speed, { 8,9,10,11,12,13,14,15 });
    pSprite->CreateAnimation(ANIM_RIGHT, speed, { 16,17,18,19,20,21,22,23 });
    pSprite->CreateAnimation(ANIM_FORWARDS, speed, { 24,25,26,27,28,29,30,31 });
    pSprite->SetScale(0.8f);

    // 


    registry.addComponent(entity, SpriteComponent{ pSprite, 0 });
}

// Create a default simple template
MapTemplate GenerateSystem::CreateDefaultTemplate() {
    const int roadWidth = config.roadWidth;
    MapTemplate tmpl(roadWidth, 1); // roadWidth wide, 1 deep

    // Fill with floor blocks
    for (int x = 0; x < roadWidth; x++) {
        tmpl.setBlock(x, 0, BlockType::Floor);
    }

    return tmpl;
}

MapTemplate GenerateSystem::CreateTestTemplate() {
    const int roadWidth = config.roadWidth;
    MapTemplate tmpl(roadWidth, 1);

    // Randomly decide what to put in this row
    float chance = Rand01();

    if (chance < config.normalFloorChance) {
        // Normal floor
        for (int x = 0; x < roadWidth; x++) {
            tmpl.setBlock(x, 0, BlockType::Floor);
        }
    }
    else if (chance < config.normalFloorChance + config.obstacleChance) {
        // Floor with a tall block obstacle in the middle
        for (int x = 0; x < roadWidth; x++) {
            tmpl.setBlock(x, 0, BlockType::Floor);
        }
        int obstacleX = (int)(Rand01() * roadWidth);
        tmpl.setBlock(obstacleX, 0, BlockType::TallBlock);
    }
    else if (chance < config.normalFloorChance + config.obstacleChance + config.scorePointChance) {
        // Floor with score points
        for (int x = 0; x < roadWidth; x++) {
            tmpl.setBlock(x, 0, BlockType::Floor);
        }
        int scoreX = (int)(Rand01() * roadWidth);
        tmpl.setBlock(scoreX, 0, BlockType::ScorePoint);
    }
    else if (chance < config.normalFloorChance + config.obstacleChance + config.scorePointChance + config.upgradePointChance) {
        // Floor with upgrade point (rare!)
        for (int x = 0; x < roadWidth; x++) {
            tmpl.setBlock(x, 0, BlockType::Floor);
        }
        // Don't add upgrade point to template, we'll spawn it separately in chunk generation
    }
    else {
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
void GenerateSystem::GenerateMapFromTemplate(EntityManager& registry, const MapTemplate& mapTemplate, float startZ) {
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
                }
                else {
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
                    config.wallColorR, config.wallColorG, config.wallColorB  
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
                Entity floorBlock = registry.createEntity();
                float r_floor, g_floor, b_floor;
                if (int(blockZ / blockSize) % 2 == 0) {
                    r_floor = config.floorColor1R; g_floor = config.floorColor1G; b_floor = config.floorColor1B; // Dark green
                }
                else {
                    r_floor = config.floorColor2R; g_floor = config.floorColor2G; b_floor = config.floorColor2B; // Light green
                }

                registry.addComponent(floorBlock, Transform3D{
                    Vec3{blockX, -floorHeight / 2, blockZ},
                    blockSize, floorHeight, blockSize,
                    r_floor, g_floor, b_floor
                    });
                registry.addComponent(floorBlock, MapBlockTag{});
                registry.addComponent(floorBlock, Collider3D{
                    blockSize, floorHeight, blockSize,
                    true, false  // isFloor, not wall
                    });

                registry.addComponent(block, Transform3D{
                    Vec3{blockX, tallBlockHeight / 2 - floorHeight / 2, blockZ},  // Sitting on floor
                    blockSize, tallBlockHeight, blockSize,
                    config.tallBlockColorR, config.tallBlockColorG, config.tallBlockColorB  // Blue from config
                    });
                registry.addComponent(block, MapBlockTag{});
                registry.addComponent(block, Collider3D{
                    blockSize, tallBlockHeight, blockSize,
                    true, false  // Can stand on top, not a wall
                    });
                registry.addComponent(block, Health{ 50,50 });
                break;
            }

            case BlockType::ScorePoint: {
                Entity floorBlock = registry.createEntity();
                float r, g, b;
                if (int(blockZ / blockSize) % 2 == 0) {
                    r = config.floorColor1R; g = config.floorColor1G; b = config.floorColor1B;
                }
                else {
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
                //registry.addComponent(block, MapBlockTag{});
                registry.addComponent(block, ScorePointTag{ 10, false });
                // No collider - player can walk through to collect
                break;
            }

            default:
                break;
            }
        }
    }
}
void GenerateChunk(EntityManager& registry, int chunkX, int chunkZ) {
    const float blockSize = config.blockSize;
    const int chunkSize = config.chunkSize;
    const float floorHeight = config.floorHeight;
    const float chunkWorldSize = blockSize * chunkSize;

    // Calculate world position for this chunk
    float chunkWorldX = chunkX * chunkWorldSize;
    float chunkWorldZ = chunkZ * chunkWorldSize;

    for (int localZ = 0; localZ < chunkSize; localZ++) {
        for (int localX = 0; localX < chunkSize; localX++) {
            float blockX = chunkWorldX + localX * blockSize;
            float blockZ = chunkWorldZ + localZ * blockSize;

            // Create floor block
            Entity floor = registry.createEntity();

            // Alternate colors for visual depth
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

void DespawnDistantChunks(EntityManager& registry, float playerX, float playerZ, std::set<std::pair<int, int>>& loadedChunks) {
    const float blockSize = config.blockSize;
    const int chunkSize = config.chunkSize;
    const int renderRadius = config.chunkRenderRadius;
    const float chunkWorldSize = blockSize * chunkSize;

	// calculate player chunk
    int playerChunkX = (int)std::floor(playerX / chunkWorldSize);
    int playerChunkZ = (int)std::floor(playerZ / chunkWorldSize);

	// collect chunks to unload
    std::vector<std::pair<int, int>> chunksToUnload;
    for (const auto& chunkKey : loadedChunks) {
        int dx = chunkKey.first - playerChunkX;
        int dz = chunkKey.second - playerChunkZ;

        // If chunk is out of render radius, mark for unloading
        if (std::abs(dx) > renderRadius + 1 || std::abs(dz) > renderRadius + 1) {
            chunksToUnload.push_back(chunkKey);
        }
    }

	// if nothing to unload, exit early
    if (chunksToUnload.empty()) {
        return;
    }
    std::set<std::pair<int, int>> chunksToUnloadSet(chunksToUnload.begin(), chunksToUnload.end());

    View<ChunkTag> view(registry);
    std::vector<Entity> toDestroy;

    for (EntityID id : view) {
        auto& chunkTag = view.get<ChunkTag>(id);

        if (chunksToUnloadSet.count(std::make_pair(chunkTag.chunkX, chunkTag.chunkZ))) {
  
            toDestroy.push_back({ id, registry.getEntityVersion(id) });

        }
    }

    for (const auto& e : toDestroy) {
        registry.destroyEntity(e);
    }

    for (const auto& chunkKey : chunksToUnload) {
        loadedChunks.erase(chunkKey);
    }
}

//void DespawnDistantChunks(EntityManager& registry, float playerX, float playerZ, std::set<std::pair<int, int>>& loadedChunks) {
//    const float blockSize = config.blockSize;
//    const int chunkSize = config.chunkSize;
//    const int renderRadius = config.chunkRenderRadius;
//    const float chunkWorldSize = blockSize * chunkSize;
//    const float despawnDistance = chunkWorldSize * (renderRadius + 2);  // Add buffer
//
//    // Calculate player chunk
//    int playerChunkX = (int)std::floor(playerX / chunkWorldSize);
//    int playerChunkZ = (int)std::floor(playerZ / chunkWorldSize);
//
//    // Find chunks to unload
//    std::vector<std::pair<int, int>> chunksToUnload;
//    for (const auto& chunkKey : loadedChunks) {
//        int dx = chunkKey.first - playerChunkX;
//        int dz = chunkKey.second - playerChunkZ;
//
//        // If chunk is too far, mark for unloading
//        if (std::abs(dx) > renderRadius + 1 || std::abs(dz) > renderRadius + 1) {
//            chunksToUnload.push_back(chunkKey);
//        }
//    }
//
//    // Early exit if no chunks to unload
//    if (chunksToUnload.empty()) {
//        return;
//    }
//
//    // Create view once for all chunks to despawn
//    View<ChunkTag> view(registry);
//    
//    // Despawn entities in chunks being unloaded
//    for (const auto& chunkKey : chunksToUnload) {
//        std::vector<Entity> toDestroy;
//
//        for (EntityID id : view) {
//            auto& chunkTag = view.get<ChunkTag>(id);
//            if (chunkTag.chunkX == chunkKey.first && chunkTag.chunkZ == chunkKey.second) {
//                toDestroy.push_back({ id, registry.getEntityVersion(id) });
//            }
//        }
//
//        for (const Entity& e : toDestroy) {
//            registry.destroyEntity(e);
//        }
//
//        // Remove from loaded chunks
//        loadedChunks.erase(chunkKey);
//    }
//}

// Chunk-based generation system for infinite 4-direction map
void GenerateSystem::ChunkGenerationSystem(EntityManager& registry, float playerX, float playerZ, std::set<std::pair<int, int>>& loadedChunks) {
    const float blockSize = config.blockSize;
    const int chunkSize = config.chunkSize;
    const int renderRadius = config.chunkRenderRadius;
    const float chunkWorldSize = blockSize * chunkSize;

    // Calculate which chunk the player is in
    int playerChunkX = (int)std::floor(playerX / chunkWorldSize);
    int playerChunkZ = (int)std::floor(playerZ / chunkWorldSize);

    // Generate chunks around the player
    for (int dx = -renderRadius; dx <= renderRadius; dx++) {
        for (int dz = -renderRadius; dz <= renderRadius; dz++) {
            int chunkX = playerChunkX + dx;
            int chunkZ = playerChunkZ + dz;
            std::pair<int, int> chunkKey = { chunkX, chunkZ };

            // Only generate if not already loaded
            if (loadedChunks.find(chunkKey) == loadedChunks.end()) {
                GenerateChunk(registry, chunkX, chunkZ);
                loadedChunks.insert(chunkKey);
            }
        }
    }

    // Despawn distant chunks
    DespawnDistantChunks(registry, playerX, playerZ, loadedChunks);
}
void GenerateSystem::GenerateWolf(EntityManager& registry) {
    // Generate basic wolf type for backward compatibility
    GenerateSystem::GenerateWolfOfType(registry, WolfType::Basic);
}

void GenerateSystem::GenerateWolfOfType(EntityManager& registry, WolfType type) {
    // Find player position to spawn wolf around them
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

    constexpr float kRadius = 1700.0f;

    // Random position around player in a circle
    float theta = RandRange(0.0f, 2.0f * PI);
    float r = sqrtf(RandRange(0.5f, 1.0f)) * kRadius;

    float dx = cosf(theta) * r;
    float dz = sinf(theta) * r;

    Vec3 spawnPos{
        playerPos.x + dx,
        20.0f,
        playerPos.z + dz
    };

    // Use WolfSystem to create wolf of specific type
    WolfSystem::InitWolfOfType(registry, spawnPos.x, spawnPos.z, type);
}
