# Scene Management and UI System Documentation

## Overview

This document describes the Scene Management and UI systems that have been added to the game engine.

## Scene Management System

### Location
- `src/System/Scene/SceneManager.h`
- `src/System/Scene/SceneManager.cpp`
- `src/System/Scene/GameScenes.h`
- `src/System/Scene/GameScenes.cpp`

### Features

The Scene Management system provides a clean way to manage different game states:

1. **IScene Interface**: Base class for all scenes
   - `OnEnter()`: Called when entering the scene
   - `OnExit()`: Called when exiting the scene
   - `Update(deltaTimeMs)`: Update logic for the scene
   - `Render()`: Render logic for the scene
   - `GetName()`: Returns the scene name

2. **SceneManager**: Manages scene transitions
   - `RegisterScene(name, scene)`: Register a scene with a name
   - `SwitchToScene(name)`: Switch to a different scene
   - `Update(deltaTimeMs)`: Update current scene
   - `Render()`: Render current scene
   - `GetCurrentSceneName()`: Get the name of the current scene

3. **Built-in Scenes**:
   - **StartScene**: Initial screen with game instructions
   - **PlayingScene**: Main gameplay scene
   - **GameOverScene**: Game over screen with score display

### Usage Example

```cpp
// In EngineSystem constructor
void EngineSystem::InitializeScenes() {
    sceneManager.RegisterScene("StartScreen", std::make_unique<StartScene>(this));
    sceneManager.RegisterScene("Playing", std::make_unique<PlayingScene>(this));
    sceneManager.RegisterScene("GameOver", std::make_unique<GameOverScene>(this));
    
    sceneManager.SwitchToScene("StartScreen");
}

// Switching scenes
sceneManager.SwitchToScene("Playing");
```

### Creating New Scenes

To create a new scene:

1. Inherit from `IScene`
2. Implement all virtual methods
3. Register the scene with `SceneManager`

```cpp
class MyCustomScene : public IScene {
public:
    void OnEnter() override {
        // Setup scene
    }
    
    void OnExit() override {
        // Cleanup scene
    }
    
    void Update(float deltaTimeMs) override {
        // Update logic
    }
    
    void Render() override {
        // Render logic
    }
    
    std::string GetName() const override { 
        return "MyCustomScene"; 
    }
};
```

## UI System

### Location
- `src/System/UI/UISystem.h`
- `src/System/UI/UISystem.cpp`

### Features

The UI system provides unified management of UI elements with layout support:

1. **UIElement**: Base class for all UI elements
   - Position management (x, y coordinates)
   - Alignment support (9 anchor points)
   - Visibility control

2. **UIText**: Text element for displaying text
   - Configurable position, color, and font
   - Supports all GLUT fonts

3. **UIManager**: Manages UI element lifecycle
   - `AddText()`: Add a text element
   - `Clear()`: Clear all UI elements
   - `Render()`: Render all visible elements

4. **UIAlignment Options**:
   - TopLeft, TopCenter, TopRight
   - MiddleLeft, MiddleCenter, MiddleRight
   - BottomLeft, BottomCenter, BottomRight

### Usage Example

```cpp
UIManager uiManager;

// Add text with center alignment
uiManager.AddText("GAME OVER", -70, -100, 
                  1.0f, 0.0f, 0.0f,  // Red color
                  UIAlignment::MiddleCenter);

// Add text with top-left alignment
uiManager.AddText("Score: 1000", 10, 10,
                  1.0f, 1.0f, 1.0f,  // White color
                  UIAlignment::TopLeft);

// Render all UI elements
uiManager.Render();

// Clear UI
uiManager.Clear();
```

### Alignment System

The alignment system automatically positions UI elements relative to screen anchors:

- **TopLeft**: Absolute positioning from top-left corner
- **MiddleCenter**: Position relative to screen center
- **BottomRight**: Position relative to bottom-right corner
- etc.

Example:
```cpp
// Text will be centered on screen, offset by (-50, -50) from center
uiManager.AddText("Centered Text", -50, -50, 
                  1.0f, 1.0f, 1.0f,
                  UIAlignment::MiddleCenter);
```

## Integration with Existing Code

The systems are integrated into the EngineSystem:

1. **Scene Management**: Handles all scene transitions automatically
2. **UI Rendering**: Replaces hardcoded UI calls with managed UI system
3. **Backward Compatible**: Existing game logic remains unchanged

## Benefits

1. **Separation of Concerns**: UI and game logic are separated
2. **Easy to Extend**: Add new scenes and UI elements easily
3. **Clean Code**: Removes hardcoded UI from game logic
4. **Maintainable**: Centralized UI and scene management
5. **Flexible**: Support for multiple alignment options and custom scenes
