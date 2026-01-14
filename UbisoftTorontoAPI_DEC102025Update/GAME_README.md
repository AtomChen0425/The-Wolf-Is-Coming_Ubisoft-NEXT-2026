# 3D Game Engine - User Guide

## Overview
This is a simple 3D game engine built on top of the Ubisoft Toronto Contest API. The game features a player character that can move and jump on a procedurally generated road with various obstacles and collectibles.

## Features
- **3D Rendering**: Full 3D cube rendering with all 6 faces and proper depth sorting
- **Player Movement**: WASD controls for moving on the road
- **Camera Rotation**: Arrow keys to rotate the camera view
- **Jump Mechanics**: Spacebar to jump with gravity physics
- **Procedural Map Generation**: Infinite road generation with template-based level design
- **Block Types**: Multiple block types including floors, walls, obstacles, and collectibles
- **Collision System**: Full 3D collision detection for floors, walls, and obstacles
- **Camera System**: Third-person camera that follows and rotates with the player

## Controls
- `W` - Move forward
- `S` - Move backward
- `A` - Move left (strafe)
- `D` - Move right (strafe)
- `SPACE` - Jump
- `LEFT ARROW` - Rotate camera/player left
- `RIGHT ARROW` - Rotate camera/player right

## Game Structure

### Source Code Organization
The code is organized into two main directories:

#### `src/Game/Core/`
Contains the game-specific systems:
- **CameraSystem**: Manages the 3D camera following and rotating with the player
- **ControlSystem**: Handles player input, movement, and rotation
- **GenerateSystem**: Creates the player and procedurally generates the map using templates
- **RenderSystem**: Renders the full 3D scene with all 6 faces of cubes
- **CollisionSystem**: Handles 3D collision detection for floors, walls, and obstacles
- **MovementSystem**: Handles entity movement (currently unused for 3D)
- **PhysicsSystem**: Handles physics simulation (currently unused for 3D)

#### `src/System/`
Contains the core engine systems:
- **ECS/**: Entity Component System implementation
- **Component/**: Component definitions (Transform3D, Velocity3D, PlayerTag, BlockType, MapTemplate, etc.)
- **Math/**: Vector and matrix math utilities
- **Render/**: Rendering helper functions
- **ECSSystem**: Main engine system that coordinates updates and rendering

## How It Works

### 3D Rendering
The engine uses perspective projection to convert 3D world coordinates to 2D screen coordinates. Each cube is now rendered with all 6 faces, with back-face culling to improve performance. Different faces have different brightness levels to create better depth perception.

### Physics
The player has:
- **Gravity**: Constant downward acceleration (-980 pixels/s²)
- **Ground Collision**: Prevents falling through floors and allows landing on raised platforms
- **Wall Collision**: Blocks horizontal movement when hitting walls
- **Jump**: Initial upward velocity when pressing spacebar (only when on ground)

### Map Generation
The map is generated using a template-based system:
- **Templates**: Define block layouts with different types (floor, wall, obstacle, score point)
- **Block Types**: 5 different types for varied gameplay
- **Procedural**: Generates sections ahead of player and despawns behind
- **Customizable**: Easy to create custom level patterns

See `MAP_GENERATION_GUIDE.md` for detailed information on creating custom maps.

## Building the Project

### Windows
1. Run `generate-windows.bat`
2. Open the Visual Studio solution in `build/win64`
3. Build and run from Visual Studio

### macOS
1. Run `./generate-macos.sh`
2. Navigate to `build/macos`
3. Run `make all` to build
4. Run `make run` to execute the game

## Future Enhancements
- Score collection system implementation
- More complex terrain patterns
- Power-ups and special abilities
- Enemies and hazards
- Sound effects and music
- Improved graphics with textures
- Multiplayer support
