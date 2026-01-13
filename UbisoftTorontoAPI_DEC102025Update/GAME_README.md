# 3D Game Engine - User Guide

## Overview
This is a simple 3D game engine built on top of the Ubisoft Toronto Contest API. The game features a player character that can move and jump on a procedurally generated road.

## Features
- **3D Rendering**: Simple perspective projection rendering system
- **Player Movement**: WASD controls for moving on the road
- **Jump Mechanics**: Spacebar to jump with gravity physics
- **Procedural Road**: Infinite road generation that extends as the player moves forward
- **Camera System**: Third-person camera that follows the player

## Controls
- `W` - Move forward
- `S` - Move backward
- `A` - Move left (strafe)
- `D` - Move right (strafe)
- `SPACE` - Jump

## Game Structure

### Source Code Organization
The code is organized into two main directories:

#### `src/Game/Core/`
Contains the game-specific systems:
- **CameraSystem**: Manages the 3D camera following the player
- **ControlSystem**: Handles player input and movement
- **GenerateSystem**: Creates the player and procedurally generates the road
- **RenderSystem**: Renders the 3D scene using perspective projection
- **CollisionSystem**: Handles collision detection (currently unused for 3D)
- **MovementSystem**: Handles entity movement (currently unused for 3D)
- **PhysicsSystem**: Handles physics simulation (currently unused for 3D)

#### `src/System/`
Contains the core engine systems:
- **ECS/**: Entity Component System implementation
- **Component/**: Component definitions (Transform3D, Velocity3D, PlayerTag, etc.)
- **Math/**: Vector and matrix math utilities
- **Render/**: Rendering helper functions
- **ECSSystem**: Main engine system that coordinates updates and rendering

## How It Works

### 3D Rendering
The engine uses a simple perspective projection to convert 3D world coordinates to 2D screen coordinates. The camera is positioned behind and above the player, providing a third-person view.

### Physics
The player has:
- **Gravity**: Constant downward acceleration
- **Ground Collision**: Prevents falling through the road
- **Jump**: Initial upward velocity when pressing spacebar

### Road Generation
The road is made of blocks that are procedurally generated ahead of the player and despawned behind them to maintain performance. The road is 5 blocks wide (100 units), and the player is constrained to stay within these boundaries.

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
- Add obstacles on the road
- Implement scoring system
- Add more complex terrain
- Include collectible items
- Add sound effects
- Improve graphics with textures
