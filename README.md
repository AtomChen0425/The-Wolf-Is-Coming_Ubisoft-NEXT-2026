

# The Wolf Is Coming 🐺🐑

> A chaotic 3D roguelite survival shooter where the prey becomes the predator. Built entirely from scratch with a custom C++ engine.

## 🎮 Game Overview

![Gameplay](./docs/Game.gif)

Inspired by the classic Chinese fable of "The Wolf and the Sheep" and the adrenaline-fueled gameplay of *Vampire Survivors* and *Brotato*, **The Wolf Is Coming** turns the traditional escort mission on its head.

You play as a battle-hardened shepherd. Your duty isn't just to protect your flock—it's to weaponize them. Command a squad of armed sheep powered by intelligent flocking algorithms, unleash devastating firepower, and survive relentless waves of an ever-evolving wolf pack in an infinitely generated 3D world.

## ✨ Key Features

### ⚔️ Gameplay Mechanics

* **Squad-Based Bullet Heaven**: Rescue sheep to expand your flock. They aren't just extra lives; they are autonomous turrets that shoot alongside you.
* **Dynamic Enemy Taxonomy**: Face diverse wolf variants, including leaping **Hunters**, ranged **Snipers**, bullet-sponge **Tanks**, and **Magic** wolves that can infect your sheep.
* **Roguelike Progression**: Survive to earn powerful upgrades. Equip your player with cannons or distribute machine guns to your flock to turn the tide.
* **Infinite Exploration**: The world dynamically generates as you move, ensuring an endless battlefield with varied terrain and obstacles.

### 🛠️ Technical Highlights (Custom C++ Engine)

This game was built **without commercial engines** (like Unity or Unreal). All core systems were developed from scratch:

* **Custom ECS Architecture**: A highly optimized Entity-Component-System using Sparse Sets and Bitmasking, capable of handling hundreds of entities simultaneously with zero frame drops.
* **Custom 3D Rendering Pipeline**: Built on top of OpenGL (GLUT). Features a Hybrid Rendering approach with 3D map cubes and 2D character sprites, utilizing the **Painter's Algorithm**, custom NDC projection, and **Billboarding** for a perfect 2.5D perspective.
* **Advanced Flocking AI**: The sheep squad is driven by a custom **Boids Algorithm** (Separation, Alignment, Cohesion), heavily optimized with Spatial Partitioning. They naturally follow the player while dynamically scattering to evade threats.
* **High-Performance Collision**: Implements a **Spatial Hashing Grid** for the broad-phase and a custom **AABB 3D Solver** for the narrow-phase, ensuring smooth physical obstruction and efficient AOE bullet detection.

## 🕹️ Controls

| Action | Input |
| --- | --- |
| **Movement** | `W` `A` `S` `D` |
| **Jump** | `Spacebar` |
| **Aim / Look** | `Mouse Movement` |
| **Fire Weapon** | `Left Mouse Button` |
| **Confirm / Select** | `Enter` |

## 🚀 Build & Run Instructions

The core source code is located in the `src/Game` and `src/System` directories.

**To build and run the project on a Windows machine:**

1. Clone or download the repository to your local machine.
2. Navigate to the project's root directory.
3. Execute the batch script: `generate-windows.bat`.
4. Navigate to the generated build folder: `build/win64/`.
5. Open the `UbiTorContestAPI.sln` file in **Visual Studio**.
6. Set the game project as the startup project and press the **Run** button (or `F5`) to start the game.

## 📂 Architecture Overview

* **`src/System/ECS/`**: The core Data-Oriented Entity-Component-System framework.
* **`src/Game/Core/RenderSystem.cpp`**: The custom 3D projection and rendering pipeline.
* **`src/Game/Core/SheepSystem.cpp` & `WolfSystem.cpp**`: AI behavior trees and Steering Behaviors.
* **`src/System/Physic/Collision.cpp`**: Math library for spatial hashing and AABB intersection.
* **`src/Game/Core/GenerateSystem.cpp`**: Chunk-based procedural map generation.
