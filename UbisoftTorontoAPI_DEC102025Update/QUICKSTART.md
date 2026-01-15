# Quick Start Guide

## What's New

This update adds three major improvements to the 3D game engine:

1. **Full 3D Cube Rendering** - Cubes now render with all 6 faces instead of just the top
2. **Fixed First Frame Bug** - Player now properly collides with the ground from the very first frame
3. **Template-Based Map Generation** - Easy-to-use system for creating custom level layouts

## Getting Started

### Building the Project

#### Windows
```bash
generate-windows.bat
# Then open build/win64/UbiTorContestAPI.sln in Visual Studio
# Build and run (F5)
```

#### macOS
```bash
chmod +x generate-macos.sh
./generate-macos.sh
cd build/macos
make all
make run
```

### Playing the Game

**Controls:**
- `W/A/S/D` - Move forward/left/backward/right
- `SPACE` - Jump
- `LEFT/RIGHT ARROW` - Rotate camera and player

**Objective:**
- Navigate the procedurally generated road
- Avoid or jump over obstacles (blue tall blocks)
- Collect score points (yellow cubes)
- Don't fall into gaps!

## Creating Custom Maps

### Quick Example

Edit `src/Game/Core/GenerateSystem.cpp` and add:

```cpp
MapTemplate CreateMyLevel() {
    MapTemplate tmpl(5, 1);  // 5 blocks wide, 1 deep
    
    // Create your level layout
    tmpl.setBlock(0, 0, BlockType::Floor);
    tmpl.setBlock(1, 0, BlockType::Floor);
    tmpl.setBlock(2, 0, BlockType::TallBlock);  // Obstacle!
    tmpl.setBlock(3, 0, BlockType::ScorePoint); // Collectible!
    tmpl.setBlock(4, 0, BlockType::Floor);
    
    return tmpl;
}
```

Then in `MapGenerationSystem()`, change:
```cpp
MapTemplate tmpl = CreateTestTemplate();
```
to:
```cpp
MapTemplate tmpl = CreateMyLevel();
```

Rebuild and run to see your custom level!

## Block Types Reference

| Type | Description | Color | Player Interaction |
|------|-------------|-------|-------------------|
| `BlockType::Empty` | No block | - | Player falls through |
| `BlockType::Floor` | Standard ground | Green | Can walk and stand |
| `BlockType::Wall` | Solid barrier | Brown | Blocks movement |
| `BlockType::TallBlock` | Raised platform | Blue | Can stand on top |
| `BlockType::ScorePoint` | Collectible | Yellow | Walk through to collect |

## What Changed

### Fixed Issues
- **First Frame Bug**: Player now starts at correct height (Y=5) and collision works immediately
- **Collision Logic**: No longer requires player to already be on ground to detect ground

### New Features
- **3D Cube Rendering**: All blocks now show all 6 faces with proper shading
- **Back-face Culling**: Improves performance by not rendering faces pointing away from camera
- **Map Templates**: Easy system to design custom level layouts
- **Multiple Block Types**: 5 different block types for varied gameplay
- **Score Points**: Collectible items (framework ready, collection logic can be added)

### Code Structure
```
src/
├── Game/Core/
│   ├── RenderSystem.cpp     - NEW: RenderCube() function
│   ├── GenerateSystem.cpp   - NEW: Template-based generation
│   ├── CollisionSystem.cpp  - FIXED: Ground collision logic
│   └── ...
└── System/Component/
    └── Component.h          - NEW: BlockType, MapTemplate, ScorePointTag
```

## Common Tasks

### Change Road Width
In `GenerateSystem.cpp`:
```cpp
const int roadWidth = 5;  // Change this number
```

### Adjust Physics
In `ControlSystem.cpp`:
```cpp
const float gravity = -980.0f;      // Gravity strength
const float jumpVelocity = 400.0f;  // Jump height
const float forwardSpeed = 200.0f;  // Forward/back speed
const float strafeSpeed = 300.0f;   // Left/right speed
```

### Change Colors
In `GenerateSystem.cpp`, when creating blocks:
```cpp
registry.addComponent(block, Transform3D{
    Vec3{x, y, z},
    width, height, depth,
    r, g, b  // RGB values from 0.0 to 1.0
});
```

### Add New Block Type
1. Add to `BlockType` enum in `Component.h`
2. Add case in `GenerateMapFromTemplate()` in `GenerateSystem.cpp`
3. Define block properties (size, color, collision)

## Troubleshooting

### Player Falls Through Floor
- Check that floor blocks are being generated
- Verify player spawn height is correct (Y=5)
- Ensure collision system is running

### Cubes Don't Render
- Check that camera is positioned correctly
- Verify blocks are within render distance
- Ensure blocks have Transform3D and MapBlockTag components

### Collision Not Working
- Make sure Collider3D component is added to blocks
- Verify isFloor or isWall flags are set correctly
- Check that CollisionSystem::Update() is being called

## Next Steps

1. **Implement Score Collection**: Add logic to detect when player touches score points
2. **Add Sound Effects**: Play sounds for jumping, collecting, colliding
3. **Create Level Progression**: Design a sequence of increasingly difficult templates
4. **Add Enemies**: Create moving obstacles or enemies
5. **Improve Visuals**: Add textures, particles, or lighting effects

## Documentation

For more detailed information, see:
- `MAP_GENERATION_GUIDE.md` - Complete guide to creating custom maps
- `IMPLEMENTATION_SUMMARY.md` - Technical details of all changes
- `GAME_README.md` - Full game documentation
- `CHANGES_SUMMARY.md` - Previous change history

## Support

If you encounter issues:
1. Check that you're using the correct platform (Windows/macOS)
2. Verify all dependencies are installed (SDL3 on macOS, FreeGLUT on Windows)
3. Review the documentation files for examples
4. Check the code comments for implementation details

## Example Level Patterns

### Easy Start
```cpp
// All floor blocks
for (int x = 0; x < 5; x++) {
    tmpl.setBlock(x, 0, BlockType::Floor);
}
```

### Medium Challenge
```cpp
// Floor with one obstacle
for (int x = 0; x < 5; x++) {
    tmpl.setBlock(x, 0, BlockType::Floor);
}
tmpl.setBlock(2, 0, BlockType::TallBlock);  // Middle obstacle
```

### Hard Challenge
```cpp
// Floor with gaps
tmpl.setBlock(0, 0, BlockType::Floor);
tmpl.setBlock(1, 0, BlockType::Empty);      // Gap!
tmpl.setBlock(2, 0, BlockType::TallBlock);  // Jump to this
tmpl.setBlock(3, 0, BlockType::Empty);      // Gap!
tmpl.setBlock(4, 0, BlockType::Floor);
```

### Reward Section
```cpp
// Score points galore
for (int x = 0; x < 5; x++) {
    tmpl.setBlock(x, 0, BlockType::ScorePoint);
}
```

Happy coding! 🎮
