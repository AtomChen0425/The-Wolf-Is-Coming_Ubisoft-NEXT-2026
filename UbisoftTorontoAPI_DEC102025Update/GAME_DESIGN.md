# Vampire Survivors-style Roguelike Game

## Game Overview
This is a Vampire Survivors-inspired roguelike game built on an Entity Component System (ECS) architecture. The player automatically attacks nearby enemies while moving around to avoid them. Enemies spawn continuously and drop experience when defeated, allowing the player to level up.

## Game Features

### 🎮 Core Gameplay
- **Player Movement**: Use WASD or Left Stick to move the player character
- **Jump Mechanic**: Press Space to jump (2.5D gameplay)
- **Auto-Attack**: The player automatically fires projectiles at the nearest enemy
- **Experience & Leveling**: Defeat enemies to gain experience and level up

### 👾 Enemy Types
The game features 4 different enemy types, each represented by a colored square:

| Type | Color | Speed | Health | Score Value | Spawn Rate |
|------|-------|-------|--------|-------------|------------|
| **WEAK** | 🟥 Red | 180 px/s | 50 HP | 10 exp | 50% |
| **NORMAL** | 🟩 Green | 120 px/s | 100 HP | 20 exp | 30% |
| **TANK** | 🟦 Blue | 70 px/s | 250 HP | 50 exp | 15% |
| **ELITE** | 🟪 Purple | 130 px/s | 200 HP | 40 exp | 5% |

### 💪 Player Stats
- **Health**: 100 HP (loses 10 HP per enemy collision)
- **Starting Level**: 1
- **Auto-Weapon Stats**:
  - Cooldown: 500ms
  - Bullet Speed: 400 px/s
  - Damage: 20 HP
  - Range: 500 px

## Code Organization

### 📁 Directory Structure

```
src/
├── Game/
│   └── Core/                    # Game systems (gameplay logic)
│       ├── CollisionSystem.*    # Handles bullet-enemy and player-enemy collisions
│       ├── ControlSystem.*      # Player input and movement control
│       ├── GenerateSystem.*     # Entity creation (player, enemies, bullets)
│       ├── MovementSystem.*     # Entity position updates
│       ├── RenderSystem.*       # Rendering and UI display
│       └── WeaponSystem.*       # Auto-attack weapon logic
│
└── System/
    ├── Component/
    │   └── Component.h          # Component definitions
    ├── ECS/
    │   └── ECS.h                # Entity Component System implementation
    ├── Math/                    # Vector math utilities
    ├── Physic/
    │   └── Collision.*          # Collision detection helpers
    └── Render/
        └── RenderHelper.h       # Rendering utilities
```

### 🔧 Component Definitions

#### Core Components
- **Position3D**: 3D position (x, z for ground, y for height)
- **Velocity3D**: 3D velocity
- **Health**: Current and max health
- **RigidBody**: Physics properties (mass, radius, force)
- **SpriteComponent**: Sprite rendering

#### Game-Specific Components
- **PlayerTag**: Marks player entity
- **EnemyTag**: Marks enemy entities
- **BulletTag**: Marks bullet entities
- **EnemyTypeComponent**: Enemy type, color, speed, score value
- **BulletComponent**: Bullet damage and lifetime
- **Experience**: Current exp, exp to next level, level
- **AutoWeapon**: Auto-attack weapon stats

### 🎯 System Responsibilities

#### GenerateSystem
Creates game entities:
- `CreatePlayer()`: Spawns player with all components
- `SpawnEnemy()`: Spawns random enemy type
- `SpawnEnemyOfType()`: Spawns specific enemy type
- `CreateBullet()`: Creates projectile

#### MovementSystem
Updates entity positions:
- `SpriteMovementSystem25D()`: Player movement with physics
- `EnemyMovementSystem3D()`: Enemies move toward player
- `BulletMovementSystem()`: Bullet movement
- `BulletLifetimeSystem()`: Removes expired bullets

#### WeaponSystem
Auto-attack logic:
- Finds nearest enemy within range
- Fires bullets automatically on cooldown
- Updates weapon cooldown timer

#### CollisionSystem
Collision detection and response:
- `CheckBulletEnemyCollision()`: Damages/kills enemies, grants exp
- `CheckPlayerEnemyCollision()`: Damages player, applies knockback

#### RenderSystem
Rendering and UI:
- `RenderSystem25D()`: Renders enemies, bullets, and sprites in 2.5D
- Displays HUD: health, level, exp, enemy count, bullet count
- Applies camera offset for scrolling
- Y-sorting for depth

#### ControlSystem
Player input:
- Reads controller/keyboard input
- Updates player velocity
- Handles sprite animations

### 🎨 Rendering System

The game uses a **2.5D rendering approach**:
- **3D positions** (x, z, y) are projected to 2D screen space
- **Camera system** follows the player with smooth lerping
- **Shadows** are drawn at ground level (y=0)
- **Entities** are drawn at their height (y offset)
- **Y-sorting** ensures proper depth ordering

### 🔄 Update Loop

The main game loop (`EngineSystem::Update`) runs systems in this order:
1. **Enemy Spawning**: Spawn enemies at regular intervals
2. **ControlSystem**: Process player input
3. **WeaponSystem**: Auto-fire bullets at enemies
4. **CollisionSystem**: Detect and resolve collisions
5. **MovementSystem**: Update all entity positions
6. **UpdateCamera**: Smooth camera follow
7. **RenderSystem**: Draw everything and UI

## Game Balance

### Experience Scaling
- Level 1 requires: 100 exp
- Each level increases requirement by 20%
- Formula: `nextLevelExp = currentLevelExp * 1.2`

### Enemy Spawn Rate
- Base interval: 1200ms
- Minimum interval: 280ms
- Spawns one enemy per interval

### Difficulty Progression
As the player levels up:
- Enemy types remain balanced by spawn rate
- Player needs to upgrade weapons (future feature)
- More enemies spawn over time (spawn rate could scale)

## Future Enhancements

Potential additions to make it more like Vampire Survivors:
- ⚔️ Multiple weapon types
- 📦 Weapon upgrades on level up
- 🎯 Different attack patterns (orbit, beam, area)
- 💎 Collectible items and power-ups
- 🏆 Boss enemies
- 📊 Permanent upgrades/meta-progression
- 🎵 Sound effects and music
- 💀 Death screen with stats

## How to Build

### Windows
```batch
generate-windows.bat
# Open solution in Visual Studio
# Build and run
```

### MacOS
```bash
./generate-macos.sh
cd build/macos
make all
make run
```

## Controls
- **Movement**: WASD or Left Stick
- **Jump**: Space
- **Auto-Fire**: Automatic when enemies in range

## Credits
Built using:
- **ECS Architecture**: Custom implementation with Sparse Set
- **Rendering**: OpenGL via FreeGLUT
- **Platform**: Ubisoft NEXT API

---

**Note**: Enemy colors are clearly defined and easy to distinguish:
- Red = Fast but weak
- Green = Balanced
- Blue = Slow tank
- Purple = Elite variant
