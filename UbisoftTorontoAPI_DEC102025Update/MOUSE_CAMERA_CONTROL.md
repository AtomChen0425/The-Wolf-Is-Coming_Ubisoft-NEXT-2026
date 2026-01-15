# Mouse Camera Control - Implementation Guide

## 概述 (Overview)

本文档说明了新增的鼠标摄像机控制功能。

This document describes the newly added mouse camera control feature.

---

## 功能特性 (Features)

### 鼠标控制 (Mouse Control)

1. **俯仰角控制 (Pitch Control)**
   - 鼠标垂直移动控制摄像机的上下视角
   - 范围限制在 ±80° 以防止摄像机翻转
   - 提供自然的第一人称视角体验

2. **偏航角控制 (Yaw Control)**  
   - 鼠标水平移动控制摄像机的左右旋转
   - 无角度限制，可以 360° 旋转
   - 与玩家的旋转方向同步

3. **备用控制 (Alternative Controls)**
   - 方向键（左/右）仍可用于偏航控制
   - 为不习惯鼠标控制的玩家提供选择

---

## 技术实现 (Technical Implementation)

### Camera3D 结构更新

```cpp
struct Camera3D {
    // ... existing fields ...
    
    // NEW: Camera pitch angle (up/down viewing)
    float pitchAngle = 0.0f;
    
    // NEW: Mouse tracking state
    float lastMouseX = 0.0f;
    float lastMouseY = 0.0f;
    bool mouseInitialized = false;
};
```

### 鼠标输入处理 (Mouse Input Handling)

在 `CameraSystem.cpp` 中实现：

```cpp
// Get mouse position
float mouseX, mouseY;
App::GetMousePos(mouseX, mouseY);

// Calculate delta with sensitivity
const float mouseSensitivity = 0.003f;
float deltaX = (mouseX - camera.lastMouseX) * mouseSensitivity;
float deltaY = (mouseY - camera.lastMouseY) * mouseSensitivity;

// Update angles
camera.rotationAngle += deltaX;  // Yaw
camera.pitchAngle -= deltaY;     // Pitch (inverted Y)

// Clamp pitch
const float maxPitch = 1.4f;  // ~80 degrees
const float minPitch = -1.4f;
camera.pitchAngle = clamp(camera.pitchAngle, minPitch, maxPitch);
```

### 投影更新 (Projection Update)

在 `RenderHelper.cpp` 中更新了投影函数以支持俯仰角：

```cpp
// First rotate around Y axis (yaw)
float rotatedX = rx * cosYaw + rz * sinYaw;
float rotatedZ = -rx * sinYaw + rz * cosYaw;

// Then apply pitch rotation
float pitchedY = ry * cosPitch - rotatedZ * sinPitch;
float pitchedZ = ry * sinPitch + rotatedZ * cosPitch;

// Project to screen
outX = rotatedX * (fov / pitchedZ) + centerX;
outY = pitchedY * (fov / pitchedZ) + centerY;
```

---

## 使用方法 (Usage)

### 基本控制 (Basic Controls)

**移动鼠标：**
- 向左/右移动 → 摄像机左右旋转（偏航）
- 向上/下移动 → 摄像机上下俯仰

**键盘备用：**
- ← 左箭头 → 向左旋转
- → 右箭头 → 向右旋转

### 调整灵敏度 (Adjusting Sensitivity)

可在 `CameraSystem.cpp` 中修改灵敏度：

```cpp
const float mouseSensitivity = 0.003f; // 增大=更快，减小=更慢
```

### 调整俯仰限制 (Adjusting Pitch Limits)

可在 `CameraSystem.cpp` 中修改俯仰角度限制：

```cpp
const float maxPitch = 1.4f;  // 最大向上角度（弧度）
const float minPitch = -1.4f; // 最大向下角度（弧度）
```

---

## 文件修改 (Files Modified)

1. **src/System/ECSSystem.h**
   - 添加 `pitchAngle` 字段
   - 添加鼠标追踪变量

2. **src/Game/Core/CameraSystem.cpp**
   - 实现鼠标输入处理
   - 计算俯仰和偏航角度
   - 更新摄像机位置

3. **src/System/Render/RenderHelper.cpp**
   - 更新 `Project()` 函数支持俯仰
   - 更新 `ProjectToScreenWithDepth()` 支持俯仰

4. **src/Game/Core/RenderSystem.cpp**
   - 更新 `MaxDepthInCameraSpace()` 支持俯仰

5. **src/System/Scene/GameScenes.cpp**
   - 更新开始界面 UI 显示新控制方式

---

## 工作原理 (How It Works)

### 旋转顺序 (Rotation Order)

1. **偏航旋转 (Yaw Rotation)**
   - 绕 Y 轴旋转（世界坐标系）
   - 改变玩家和摄像机的水平朝向

2. **俯仰旋转 (Pitch Rotation)**  
   - 绕 X 轴旋转（摄像机坐标系）
   - 改变摄像机的垂直视角

### 坐标转换流程 (Coordinate Transform Pipeline)

```
World Space → Camera-Relative → Yaw Rotation → Pitch Rotation → Screen Projection
世界空间 → 相对摄像机 → 偏航旋转 → 俯仰旋转 → 屏幕投影
```

### 为什么限制俯仰角？ (Why Clamp Pitch?)

防止"万向节锁"（Gimbal Lock）问题：
- 当俯仰角接近 ±90° 时，摄像机可能翻转
- 限制在 ±80° 提供足够的视角范围同时保持稳定
- 用户体验更好，避免迷失方向

---

## 调试技巧 (Debugging Tips)

### 查看当前角度

可以在 `RenderSystem.cpp` 中添加调试信息：

```cpp
App::Print(50, 100, 
    ("Pitch: " + std::to_string(camera.pitchAngle) + 
     " Yaw: " + std::to_string(camera.rotationAngle)).c_str(),
    1.0f, 1.0f, 0.0f);
```

### 测试步骤

1. 启动游戏
2. 移动鼠标上下左右
3. 观察摄像机视角变化
4. 测试边界情况（最大向上/向下）
5. 确认方向键仍然工作

---

## 常见问题 (FAQ)

**Q: 鼠标移动没有反应？**
A: 检查 `App::GetMousePos()` 是否正常工作。确保游戏窗口有焦点。

**Q: 摄像机旋转太快/太慢？**
A: 调整 `mouseSensitivity` 值。

**Q: 可以禁用鼠标控制吗？**
A: 可以通过添加一个开关变量来控制是否启用鼠标输入。

**Q: 为什么 Y 轴是反的？**
A: 屏幕 Y 坐标是从上到下递增的，所以需要反转以匹配预期的"向上看"行为。

---

## 未来增强 (Future Enhancements)

可能的改进：
- [ ] 添加鼠标灵敏度设置选项
- [ ] 添加反转 Y 轴选项
- [ ] 添加鼠标平滑处理（减少抖动）
- [ ] 支持手柄摇杆控制
- [ ] 添加视角重置功能

---

## 参考资料 (References)

- **欧拉角旋转**: https://en.wikipedia.org/wiki/Euler_angles
- **万向节锁**: https://en.wikipedia.org/wiki/Gimbal_lock
- **3D 投影**: https://en.wikipedia.org/wiki/3D_projection

---

**实现完成！Enjoy the new camera control! 🎮**
