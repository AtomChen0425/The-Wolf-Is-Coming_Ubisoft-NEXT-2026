# System Architecture Diagram

## Scene Management System Flow

```
┌─────────────────────────────────────────────────────────────┐
│                      EngineSystem                           │
│  ┌───────────────────────────────────────────────────────┐  │
│  │                  SceneManager                         │  │
│  │  ┌──────────────┐  ┌──────────────┐  ┌────────────┐  │  │
│  │  │  StartScene  │  │ PlayingScene │  │ GameOver   │  │  │
│  │  │              │  │              │  │   Scene    │  │  │
│  │  │  OnEnter()   │  │  OnEnter()   │  │ OnEnter()  │  │  │
│  │  │  OnExit()    │  │  OnExit()    │  │ OnExit()   │  │  │
│  │  │  Update()    │  │  Update()    │  │ Update()   │  │  │
│  │  │  Render()    │  │  Render()    │  │ Render()   │  │  │
│  │  └──────┬───────┘  └──────┬───────┘  └──────┬─────┘  │  │
│  │         │                 │                 │         │  │
│  │         └─────────────────┴─────────────────┘         │  │
│  │                          │                            │  │
│  │                    Each scene has                     │  │
│  │                          ▼                            │  │
│  │                    ┌──────────┐                       │  │
│  │                    │UIManager │                       │  │
│  │                    └──────────┘                       │  │
│  └───────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘

Scene Transitions:
  StartScreen ─(SPACE)─> Playing ─(Fall)─> GameOver ─(R)─> StartScreen
                           │
                          (R)
                           │
                           ▼
                      StartScreen
```

## UI System Structure

```
┌─────────────────────────────────────────────────────────────┐
│                        UIManager                            │
│  ┌───────────────────────────────────────────────────────┐  │
│  │         std::vector<UIElement*> elements              │  │
│  └───────────────────────────────────────────────────────┘  │
│                          │                                  │
│                          ▼                                  │
│         ┌────────────────────────────────┐                  │
│         │         UIElement              │                  │
│         │  - position (x, y)             │                  │
│         │  - alignment (UIAlignment)     │                  │
│         │  - visible (bool)              │                  │
│         └────────────┬───────────────────┘                  │
│                      │                                      │
│                      ▼                                      │
│         ┌────────────────────────┐                          │
│         │       UIText           │                          │
│         │  - text (string)       │                          │
│         │  - color (r, g, b)     │                          │
│         │  - font                │                          │
│         │  + SetText()           │                          │
│         │  + SetColor()          │                          │
│         │  + Render()            │                          │
│         └────────────────────────┘                          │
└─────────────────────────────────────────────────────────────┘
```

## UI Alignment System

```
Screen Layout (1024 x 768):

  TopLeft         TopCenter          TopRight
      ┌──────────────┬──────────────┐
      │              │              │
      │              │              │
MiddleLeft ─────MiddleCenter────MiddleRight
      │              │              │
      │              │              │
      └──────────────┴──────────────┘
  BottomLeft    BottomCenter    BottomRight

Example Positions:
  TopLeft(10, 10) → Screen(10, 10)
  MiddleCenter(0, 0) → Screen(512, 384)
  TopCenter(-50, 20) → Screen(462, 20)
  BottomRight(10, 10) → Screen(1014, 758)
```

## Integration with Existing Systems

```
┌─────────────────────────────────────────────────────────────┐
│                    GameTest.cpp                             │
│                    Init() / Update() / Render()             │
└───────────────────────────┬─────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                     EngineSystem                            │
│  ┌──────────────────────────────────────────────────────┐   │
│  │              SceneManager                            │   │
│  │     (Manages scene transitions & UI)                 │   │
│  └──────────────────────────────────────────────────────┘   │
│                            │                                │
│  ┌─────────────────────────┼────────────────────────────┐   │
│  │  EntityManager         │     Camera3D                │   │
│  │  (ECS Registry)         │     (3D Camera)             │   │
│  └─────────────────────────┴────────────────────────────┘   │
│                            │                                │
│                            ▼                                │
│  ┌──────────────────────────────────────────────────────┐   │
│  │              Game Systems                            │   │
│  │  - RenderSystem                                      │   │
│  │  - MovementSystem                                    │   │
│  │  - ControlSystem                                     │   │
│  │  - CollisionSystem                                   │   │
│  │  - CameraSystem                                      │   │
│  │  - PhysicsSystem                                     │   │
│  │  - GenerateSystem                                    │   │
│  └──────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────┘
```

## File Structure

```
UbisoftTorontoAPI_DEC102025Update/
├── src/
│   ├── System/
│   │   ├── Scene/
│   │   │   ├── SceneManager.h          (Scene manager interface)
│   │   │   ├── SceneManager.cpp        (Scene manager implementation)
│   │   │   ├── GameScenes.h            (Concrete scenes header)
│   │   │   └── GameScenes.cpp          (Concrete scenes implementation)
│   │   ├── UI/
│   │   │   ├── UISystem.h              (UI system interface)
│   │   │   └── UISystem.cpp            (UI system implementation)
│   │   ├── ECSSystem.h                 (Modified: added SceneManager)
│   │   └── ECSSystem.cpp               (Modified: integrated SceneManager)
│   ├── Game/
│   │   └── ...                         (Unchanged)
│   └── ContestAPI/
│       └── ...                         (Unchanged)
├── SCENE_UI_DOCUMENTATION.md           (English documentation)
├── IMPLEMENTATION_SUMMARY_CN.md        (Chinese/English summary)
└── SCENE_UI_EXAMPLES.cpp               (Usage examples - not compiled)
```

## Data Flow

```
User Input → EngineSystem::Update()
                    │
                    ├─→ SceneManager::Update()
                    │         │
                    │         └─→ CurrentScene::Update()
                    │
                    └─→ Game Systems Update
                              (only in Playing state)

Render Pipeline:
    EngineSystem::Render()
            │
            ├─→ CameraSystem::Update()
            │
            ├─→ RenderSystem::Render() (3D scene)
            │
            └─→ SceneManager::Render()
                      │
                      └─→ CurrentScene::Render()
                                │
                                └─→ UIManager::Render()
                                          │
                                          └─→ UIElement::Render()
                                                    │
                                                    └─→ App::Print()
```

## Memory Management

```
Smart Pointers Used:
  EngineSystem
    └─ std::unique_ptr<EntityManager> registry
    └─ SceneManager sceneManager
          └─ std::unordered_map<string, std::unique_ptr<IScene>> scenes
                └─ StartScene, PlayingScene, GameOverScene
                      └─ UIManager uiManager
                            └─ std::vector<std::unique_ptr<UIElement>> elements
                                  └─ UIText, UIButton, etc.

No manual memory management required!
All cleanup is automatic.
```
