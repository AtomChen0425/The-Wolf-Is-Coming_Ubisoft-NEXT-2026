# Scene Management and UI System - Implementation Summary

## 概述 (Overview)

本次更新为游戏引擎添加了两个核心系统：
- **场景管理系统 (Scene Management System)**: 用于管理游戏不同的状态
- **UI系统 (UI System)**: 统一管理UI的生成和布局

This update adds two core systems to the game engine:
- **Scene Management System**: For managing different game states
- **UI System**: Unified management of UI generation and layout

---

## 新增文件 (New Files)

### Scene Management System
1. **src/System/Scene/SceneManager.h** - Scene Manager interface and IScene base class
2. **src/System/Scene/SceneManager.cpp** - Scene Manager implementation
3. **src/System/Scene/GameScenes.h** - Concrete scene implementations (Start, Playing, GameOver)
4. **src/System/Scene/GameScenes.cpp** - Scene implementations

### UI System
1. **src/System/UI/UISystem.h** - UI system interface (UIElement, UIText, UIManager)
2. **src/System/UI/UISystem.cpp** - UI system implementation

### Documentation
1. **SCENE_UI_DOCUMENTATION.md** - Complete documentation in English
2. **SCENE_UI_EXAMPLES.cpp** - Usage examples (not compiled, reference only)

---

## 系统架构 (System Architecture)

### Scene Management System 架构

```
IScene (Interface)
├── StartScene      - 开始画面
├── PlayingScene    - 游戏中画面
└── GameOverScene   - 游戏结束画面

SceneManager
├── RegisterScene()   - 注册场景
├── SwitchToScene()   - 切换场景
├── Update()          - 更新当前场景
└── Render()          - 渲染当前场景
```

### UI System 架构

```
UIElement (Base Class)
└── UIText - 文本UI元素

UIManager
├── AddText()    - 添加文本元素
├── Clear()      - 清空所有元素
└── Render()     - 渲染所有元素

UIAlignment (9种对齐方式)
├── TopLeft, TopCenter, TopRight
├── MiddleLeft, MiddleCenter, MiddleRight
└── BottomLeft, BottomCenter, BottomRight
```

---

## 核心功能 (Core Features)

### Scene Management System

1. **场景生命周期管理 (Scene Lifecycle Management)**
   - `OnEnter()`: 进入场景时初始化
   - `OnExit()`: 退出场景时清理
   - `Update()`: 每帧更新逻辑
   - `Render()`: 渲染场景UI

2. **场景切换 (Scene Transitions)**
   - 自动调用前一个场景的 `OnExit()`
   - 自动调用新场景的 `OnEnter()`
   - 无缝场景切换

3. **内置场景 (Built-in Scenes)**
   - **StartScene**: 显示游戏标题和操作说明
   - **PlayingScene**: 游戏进行中（可添加HUD）
   - **GameOverScene**: 显示最终分数和重新开始选项

### UI System

1. **UI元素管理 (UI Element Management)**
   - 统一的UI元素创建和销毁
   - 自动内存管理（使用智能指针）
   - 可见性控制

2. **布局系统 (Layout System)**
   - 9种对齐锚点（TopLeft, MiddleCenter等）
   - 相对定位系统
   - 自动屏幕坐标转换

3. **文本渲染 (Text Rendering)**
   - 颜色自定义（RGB）
   - 字体支持（所有GLUT字体）
   - 位置和对齐控制

---

## 集成说明 (Integration Details)

### 修改的现有文件 (Modified Existing Files)

1. **src/System/ECSSystem.h**
   - 添加 `SceneManager` 成员
   - 添加 `InitializeScenes()` 方法
   - 添加访问器方法 (`GetSceneManager()`, `GetRegistry()`, `GetCamera()`)

2. **src/System/ECSSystem.cpp**
   - 实现场景初始化
   - 集成场景切换逻辑
   - 用SceneManager替代硬编码的UI渲染

3. **.gitignore**
   - 添加示例文件排除规则

### 无需修改的文件 (No Changes Required)

- **CMakeLists.txt**: 使用 `GLOB_RECURSE` 自动检测新文件
- 所有游戏逻辑文件：现有功能完全保留

---

## 使用方法 (Usage)

### 添加新场景 (Adding a New Scene)

```cpp
// 1. 创建场景类 (Create scene class)
class MyScene : public IScene {
public:
    void OnEnter() override {
        uiManager.Clear();
        uiManager.AddText("My Scene", 0, 0, 1.0f, 1.0f, 1.0f, UIAlignment::MiddleCenter);
    }
    void OnExit() override { uiManager.Clear(); }
    void Update(float deltaTimeMs) override { /* 更新逻辑 */ }
    void Render() override { uiManager.Render(); }
    std::string GetName() const override { return "MyScene"; }
    
private:
    UIManager uiManager;
};

// 2. 注册场景 (Register scene)
sceneManager.RegisterScene("MyScene", std::make_unique<MyScene>());

// 3. 切换场景 (Switch to scene)
sceneManager.SwitchToScene("MyScene");
```

### UI布局示例 (UI Layout Examples)

```cpp
UIManager ui;

// 左上角 (Top-left)
ui.AddText("HP: 100", 10, 10, 1.0f, 0.0f, 0.0f, UIAlignment::TopLeft);

// 屏幕中央 (Center)
ui.AddText("PAUSED", 0, 0, 1.0f, 1.0f, 0.0f, UIAlignment::MiddleCenter);

// 底部中央 (Bottom-center)
ui.AddText("Press SPACE", 0, 30, 1.0f, 1.0f, 1.0f, UIAlignment::BottomCenter);

// 渲染 (Render)
ui.Render();
```

---

## 设计优势 (Design Benefits)

1. **关注点分离 (Separation of Concerns)**
   - UI逻辑与游戏逻辑分离
   - 场景管理与游戏更新分离

2. **易于扩展 (Easy to Extend)**
   - 添加新场景只需继承 `IScene`
   - 添加新UI元素类型只需继承 `UIElement`

3. **代码清晰 (Clean Code)**
   - 移除了硬编码的UI调用
   - 统一的场景管理接口

4. **维护性强 (Maintainable)**
   - 集中式UI管理
   - 清晰的场景生命周期

5. **灵活性高 (Flexible)**
   - 9种对齐选项支持各种布局需求
   - 可自定义场景行为

---

## 兼容性 (Compatibility)

- ✅ 向后兼容：所有现有游戏功能保持不变
- ✅ 自动构建：CMake自动检测新文件
- ✅ 跨平台：支持Windows和macOS
- ✅ 无额外依赖：仅使用现有的Contest API

---

## 测试建议 (Testing Recommendations)

在Windows或macOS上运行游戏后，应验证：

1. **场景切换**
   - 按SPACE从开始画面进入游戏
   - 按R重置回到开始画面
   - 玩家掉落后显示游戏结束画面

2. **UI显示**
   - 所有文本正确对齐和显示
   - 颜色正确渲染
   - 分数在游戏结束时正确显示

3. **游戏逻辑**
   - 移动、跳跃、碰撞检测仍然正常工作
   - 摄像机跟随玩家正常
   - 地图生成正常

---

## 未来扩展建议 (Future Enhancement Suggestions)

1. **更多UI元素类型**
   - UIButton (按钮)
   - UIPanel (面板)
   - UIImage (图片)
   - UIProgressBar (进度条)

2. **场景过渡效果**
   - 淡入淡出
   - 滑动效果

3. **UI动画**
   - 文本闪烁
   - 缩放动画
   - 位置动画

4. **更多场景**
   - 暂停菜单
   - 设置菜单
   - 关卡选择

---

## 联系方式 (Contact)

如有问题或建议，请通过GitHub Issues联系。

For questions or suggestions, please contact via GitHub Issues.
