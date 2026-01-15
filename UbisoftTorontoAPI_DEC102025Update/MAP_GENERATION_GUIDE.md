# Map Generation System Guide

## Overview
This guide explains how to use the new template-based map generation system to create custom 3D levels.

## Block Types

The system supports 5 different block types:

### 1. Empty (BlockType::Empty)
- No block is rendered at this position
- Useful for creating gaps in the floor
- Player will fall if they walk into empty spaces

### 2. Floor (BlockType::Floor)
- Standard ground block (green)
- Player can walk and stand on these
- Size: 100x10x100 units
- Position: Y=-10 (top surface at Y=-5)

### 3. Wall (BlockType::Wall)
- Vertical barrier blocks (brown)
- Completely blocks player movement
- Size: 100x100x100 units
- Position: Y=50 (centered vertically)
- Used for side boundaries and obstacles

### 4. TallBlock (BlockType::TallBlock)
- Raised platform blocks (blue)
- Player can stand on top
- Size: 100x60x100 units
- Position: Y=30 (top surface at Y=60)
- Creates elevated platforms and obstacles

### 5. ScorePoint (BlockType::ScorePoint)
- Collectible yellow cubes
- Player can walk through to collect
- Size: 30x30x30 units
- Position: Y=10 (floating above floor)
- Worth 10 points when collected
- Note: Automatically places a floor underneath

## Map Template Structure

```cpp
struct MapTemplate {
    int width;      // Width in blocks (X axis)
    int depth;      // Depth in blocks (Z axis)
    std::vector<BlockType> blocks; // Block data
    
    // Get block at position
    BlockType getBlock(int x, int z) const;
    
    // Set block at position
    void setBlock(int x, int z, BlockType type);
};
```

## Creating Custom Templates

### Example 1: Simple Floor Row
```cpp
MapTemplate CreateSimpleFloor() {
    const int roadWidth = 5;
    MapTemplate tmpl(roadWidth, 1);
    
    // Fill entire row with floor blocks
    for (int x = 0; x < roadWidth; x++) {
        tmpl.setBlock(x, 0, BlockType::Floor);
    }
    
    return tmpl;
}
```

### Example 2: Floor with Obstacle
```cpp
MapTemplate CreateObstacleRow() {
    const int roadWidth = 5;
    MapTemplate tmpl(roadWidth, 1);
    
    // Create floor
    for (int x = 0; x < roadWidth; x++) {
        tmpl.setBlock(x, 0, BlockType::Floor);
    }
    
    // Add obstacle in the middle
    tmpl.setBlock(2, 0, BlockType::TallBlock);
    
    return tmpl;
}
```

### Example 3: Gap Challenge
```cpp
MapTemplate CreateGapRow() {
    const int roadWidth = 5;
    MapTemplate tmpl(roadWidth, 1);
    
    // Create floor with gaps
    tmpl.setBlock(0, 0, BlockType::Floor);
    tmpl.setBlock(1, 0, BlockType::Empty); // Gap!
    tmpl.setBlock(2, 0, BlockType::Floor);
    tmpl.setBlock(3, 0, BlockType::Empty); // Gap!
    tmpl.setBlock(4, 0, BlockType::Floor);
    
    return tmpl;
}
```

### Example 4: Score Collection
```cpp
MapTemplate CreateScoreRow() {
    const int roadWidth = 5;
    MapTemplate tmpl(roadWidth, 1);
    
    // Create floor with score points
    for (int x = 0; x < roadWidth; x++) {
        if (x % 2 == 0) {
            tmpl.setBlock(x, 0, BlockType::ScorePoint);
        } else {
            tmpl.setBlock(x, 0, BlockType::Floor);
        }
    }
    
    return tmpl;
}
```

### Example 5: Multi-Row Template
```cpp
MapTemplate CreateComplexSection() {
    const int roadWidth = 5;
    const int depth = 3;
    MapTemplate tmpl(roadWidth, depth);
    
    // First row: normal floor
    for (int x = 0; x < roadWidth; x++) {
        tmpl.setBlock(x, 0, BlockType::Floor);
    }
    
    // Second row: obstacles
    for (int x = 0; x < roadWidth; x++) {
        tmpl.setBlock(x, 1, BlockType::Floor);
    }
    tmpl.setBlock(1, 1, BlockType::TallBlock);
    tmpl.setBlock(3, 1, BlockType::TallBlock);
    
    // Third row: score points
    for (int x = 0; x < roadWidth; x++) {
        tmpl.setBlock(x, 2, BlockType::ScorePoint);
    }
    
    return tmpl;
}
```

## Using Templates in Map Generation

### Default Random Generation
The `CreateTestTemplate()` function generates random variations:
- 60% chance: Normal floor
- 15% chance: Floor with tall block obstacle
- 15% chance: Floor with score points
- 10% chance: Floor with gaps

### Custom Generation
To use your custom template:

```cpp
void GenerateSystem::MapGenerationSystem(EntityManager& registry, float playerZ, float& nextSpawnZ) {
    const float blockSize = 100.0f;
    const float renderDistance = 1000.0f;
    
    while (nextSpawnZ < playerZ + renderDistance) {
        // Use your custom template instead of CreateTestTemplate()
        MapTemplate tmpl = CreateObstacleRow(); // Or any custom function
        GenerateMapFromTemplate(registry, tmpl, nextSpawnZ);
        
        // ... wall generation code ...
        
        nextSpawnZ += blockSize;
    }
}
```

### Pattern-Based Generation
Create different patterns based on distance:

```cpp
MapTemplate GetTemplateForDistance(float z) {
    int section = (int)(z / 1000.0f) % 4;
    
    switch (section) {
        case 0: return CreateSimpleFloor();    // Easy section
        case 1: return CreateObstacleRow();    // Medium difficulty
        case 2: return CreateGapRow();         // Hard section
        case 3: return CreateScoreRow();       // Reward section
        default: return CreateSimpleFloor();
    }
}
```

## 3D Rendering Improvements

### Full Cube Rendering
All blocks now render with 6 faces instead of just the top:
- **Front face**: Full brightness
- **Back face**: 60% brightness (darker)
- **Left face**: 70% brightness
- **Right face**: 80% brightness
- **Top face**: 90% brightness
- **Bottom face**: 50% brightness (darkest)

### Back-Face Culling
Faces pointing away from the camera are not rendered, improving performance.

### Depth Sorting
Blocks are sorted by distance from camera and rendered far-to-near for proper transparency.

## Collision System

### Floor Collision
- Player lands on top of floor and tall blocks
- Ground level is automatically calculated
- Player can jump from any floor surface

### Wall Collision
- Walls block horizontal movement
- Player is pushed out of walls if collision occurs
- Walls cannot be jumped over (too tall)

### First Frame Fix
The collision system now works correctly from frame 1:
- Player starts at correct height (Y=5)
- Collision detection doesn't require initial grounding
- Player will fall and land properly on first frame

## Score Collection System

### ScorePointTag Component
```cpp
struct ScorePointTag {
    int points;      // Points awarded (default: 10)
    bool collected;  // Whether it's been collected
};
```

### Implementing Collection
Add this to your collision or control system:

```cpp
void CheckScoreCollection(EntityManager& registry) {
    View<PlayerTag, Transform3D> playerView(registry);
    View<ScorePointTag, Transform3D> scoreView(registry);
    
    for (EntityID playerId : playerView) {
        auto& playerPos = playerView.get<Transform3D>(playerId).pos;
        
        for (EntityID scoreId : scoreView) {
            auto& scoreTag = scoreView.get<ScorePointTag>(scoreId);
            auto& scorePos = scoreView.get<Transform3D>(scoreId).pos;
            
            if (!scoreTag.collected) {
                // Check if player is close enough to collect
                float dx = playerPos.x - scorePos.x;
                float dy = playerPos.y - scorePos.y;
                float dz = playerPos.z - scorePos.z;
                float distSq = dx*dx + dy*dy + dz*dz;
                
                if (distSq < 50.0f * 50.0f) { // Collection radius
                    scoreTag.collected = true;
                    // Add points to player score
                    gScore += scoreTag.points;
                }
            }
        }
    }
}
```

## Coordinate System

### World Space
- **X axis**: Left (-) to Right (+)
- **Y axis**: Down (-) to Up (+)
- **Z axis**: Back (-) to Forward (+)

### Block Positions
- Road width: 5 blocks (500 units)
- Block size: 100x100 units in X-Z plane
- Center X positions: -200, -100, 0, +100, +200

### Heights
- Floor top: Y = -5
- Player standing: Y = 5 (bottom at -5)
- Tall block top: Y = 60
- Wall top: Y = 100

## Tips and Best Practices

1. **Balance Difficulty**: Mix easy and hard sections
2. **Visual Variety**: Use different block types for visual interest
3. **Collectibles**: Place score points as rewards after difficult sections
4. **Testing**: Test your templates in-game to ensure they're fun and fair
5. **Performance**: Keep render distance reasonable (default: 1000 units)

## Common Patterns

### Zigzag Path
```cpp
// Alternate left and right obstacles
for (int i = 0; i < 5; i++) {
    if (i % 2 == 0) {
        tmpl.setBlock(1, 0, BlockType::TallBlock);
    } else {
        tmpl.setBlock(3, 0, BlockType::TallBlock);
    }
}
```

### Corridor
```cpp
// Create walls on sides, floor in middle
tmpl.setBlock(0, 0, BlockType::Wall);
tmpl.setBlock(1, 0, BlockType::Floor);
tmpl.setBlock(2, 0, BlockType::Floor);
tmpl.setBlock(3, 0, BlockType::Floor);
tmpl.setBlock(4, 0, BlockType::Wall);
```

### Jump Challenge
```cpp
// Floor - Gap - TallBlock - Gap - Floor
tmpl.setBlock(0, 0, BlockType::Floor);
tmpl.setBlock(1, 0, BlockType::Empty);
tmpl.setBlock(2, 0, BlockType::TallBlock);
tmpl.setBlock(3, 0, BlockType::Empty);
tmpl.setBlock(4, 0, BlockType::Floor);
```

## Future Enhancements

Possible additions to the system:
- Moving platforms
- Rotating obstacles
- Timed score bonuses
- Multiple player start positions
- Checkpoint system
- Different visual themes per section
