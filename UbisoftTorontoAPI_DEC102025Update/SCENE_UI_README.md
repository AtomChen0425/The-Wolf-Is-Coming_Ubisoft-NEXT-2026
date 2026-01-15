# Scene Management & UI System - Quick Start Guide

欢迎使用新的场景管理和UI系统！/ Welcome to the new Scene Management and UI System!

## 📋 What's New?

这次更新添加了两个强大的系统来简化游戏开发：

This update adds two powerful systems to simplify game development:

1. **Scene Management System (场景管理系统)** - Manage different game states cleanly
2. **UI System (UI系统)** - Unified UI creation and layout management

## 🚀 Quick Start

### For Users (游戏玩家)

游戏现在有更好的界面和状态管理。功能保持不变！

The game now has better UI and state management. All features remain the same!

**Controls (控制):**
- **SPACE** - Start game / Jump (开始游戏/跳跃)
- **WASD** - Move (移动)
- **Arrow Keys** - Rotate camera (旋转摄像机)
- **R** - Reset game (重置游戏)

### For Developers (开发者)

#### Building the Game (构建游戏)

**Windows:**
```bash
generate-windows.bat
# Open build/win64/UbiTorContestAPI.sln in Visual Studio
# Build and run
```

**macOS:**
```bash
./generate-macos.sh
cd build/macos
make all
make run
```

#### Key Features (主要功能)

1. **Easy Scene Creation (简单的场景创建)**
   ```cpp
   class MyScene : public IScene {
       // Implement OnEnter, OnExit, Update, Render
   };
   ```

2. **9-Point UI Alignment (9点对齐)**
   ```cpp
   uiManager.AddText("Score: 100", 0, 0, 1.0f, 1.0f, 0.0f, 
                     UIAlignment::TopRight);
   ```

3. **Automatic Memory Management (自动内存管理)**
   - No manual cleanup needed!
   - Smart pointers handle everything

## 📚 Documentation

- **English Guide**: `SCENE_UI_DOCUMENTATION.md`
- **中英文总结**: `IMPLEMENTATION_SUMMARY_CN.md`
- **System Architecture**: `ARCHITECTURE_DIAGRAM.md`
- **Code Examples**: `SCENE_UI_EXAMPLES.cpp`

## 🎯 Common Tasks

### Add a New Scene (添加新场景)

```cpp
// 1. Create scene class
class PauseScene : public IScene {
    void OnEnter() override { /* Setup */ }
    void OnExit() override { /* Cleanup */ }
    void Update(float dt) override { /* Logic */ }
    void Render() override { /* Draw */ }
    std::string GetName() const override { return "Pause"; }
};

// 2. Register in EngineSystem::InitializeScenes()
sceneManager.RegisterScene("Pause", std::make_unique<PauseScene>(this));

// 3. Switch to it
sceneManager.SwitchToScene("Pause");
```

### Add UI Elements (添加UI元素)

```cpp
UIManager ui;

// Add text with alignment
ui.AddText("PAUSED", 0, 0, 1.0f, 1.0f, 0.0f, UIAlignment::MiddleCenter);
ui.AddText("HP: 100", 10, 10, 1.0f, 0.0f, 0.0f, UIAlignment::TopLeft);

// Render all UI
ui.Render();

// Clear when done
ui.Clear();
```

## 🔧 System Architecture

```
GameTest.cpp
    ↓
EngineSystem
    ├─→ SceneManager ────→ Scenes (Start, Playing, GameOver)
    │                         └─→ UIManager ─→ UI Elements
    ├─→ EntityManager
    └─→ Game Systems
```

## ✅ What's Changed?

### Added (新增):
- ✅ Scene management with lifecycle
- ✅ UI system with 9-point alignment
- ✅ Three built-in scenes (Start, Playing, GameOver)
- ✅ Comprehensive documentation

### Modified (修改):
- ✅ ECSSystem now uses SceneManager
- ✅ UI rendering is now centralized

### Unchanged (未改):
- ✅ All game logic
- ✅ ECS system
- ✅ Physics and collision
- ✅ Camera system
- ✅ Controls

## 🎨 UI Alignment Examples

```
TopLeft(10,10)      TopCenter(0,10)       TopRight(10,10)
      ↓                    ↓                      ↓
    Text                 Text                   Text
    
MiddleLeft(10,0)   MiddleCenter(0,0)    MiddleRight(10,0)
      ↓                    ↓                      ↓
    Text                 Text                   Text
    
BottomLeft(10,10)  BottomCenter(0,10)   BottomRight(10,10)
      ↓                    ↓                      ↓
    Text                 Text                   Text
```

## 🐛 Troubleshooting

### Build Issues (构建问题)
- Run the generate script again (重新运行生成脚本)
- Make sure CMake is installed (确保安装了CMake)
- Clean build directory if needed (必要时清理build目录)

### Scene Not Switching (场景不切换)
- Check scene is registered (检查场景已注册)
- Verify scene name is correct (验证场景名称正确)

### UI Not Showing (UI不显示)
- Call UIManager::Render() in scene's Render() (在Render()中调用UIManager::Render())
- Check element visibility (检查元素可见性)
- Verify color values (0.0-1.0 range) (验证颜色值范围)

## 💡 Tips

1. **Scene Names**: Use descriptive names like "MainMenu", "Pause", "Victory"
2. **UI Alignment**: Use MiddleCenter for dialogs, TopLeft for HUD
3. **Memory**: Don't worry about cleanup - smart pointers handle it!
4. **Colors**: RGB values are 0.0 to 1.0 (not 0-255)

## 🚧 Future Enhancements

Possible additions (可能的扩展):
- [ ] UIButton (buttons with click handlers)
- [ ] UIPanel (background panels)
- [ ] UIImage (sprite-based UI)
- [ ] UIProgressBar (health bars, loading bars)
- [ ] Scene transitions (fade in/out)
- [ ] UI animations

## 📞 Support

For questions or issues:
- Check documentation in this folder
- Review SCENE_UI_EXAMPLES.cpp
- Open an issue on GitHub

---

**Happy Coding! 祝编程愉快！** 🎮✨
