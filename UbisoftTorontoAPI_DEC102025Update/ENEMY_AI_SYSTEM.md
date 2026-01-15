# Enemy AI System Documentation

## 概述 (Overview)

新增的敌人AI系统提供了三个主要功能：
1. **敌人朝玩家移动** (Enemy Movement Toward Player)
2. **敌人朝玩家射击** (Enemy Shooting at Player)
3. **子弹运动** (Bullet Movement)

## 组件说明 (Component Description)

### 1. EnemyMoveToPlayer (敌人移动组件)

让敌人自动朝玩家方向移动。

**成员变量：**
```cpp
struct EnemyMoveToPlayer {
    float speed;              // 移动速度（单位/秒）
    float detectionRange;     // 检测范围（单位内可以检测到玩家）
    bool isActive;            // 是否激活此组件
};
```

**使用方法：**
```cpp
EnemyMoveToPlayer moveComponent;
moveComponent.speed = 50.0f;           // 每秒移动50个单位
moveComponent.detectionRange = 500.0f; // 500单位内检测玩家
moveComponent.isActive = true;
registry.AddComponent(enemyId, moveComponent);
```

### 2. EnemyShootAtPlayer (敌人射击组件)

让敌人自动朝玩家发射子弹。

**成员变量：**
```cpp
struct EnemyShootAtPlayer {
    float shootCooldown;      // 射击冷却时间（毫秒）
    float timeSinceLastShot;  // 上次射击后经过的时间（毫秒）
    float detectionRange;     // 射击范围（单位内可以射击玩家）
    float projectileSpeed;    // 子弹速度（单位/秒）
    bool isActive;            // 是否激活此组件
};
```

**使用方法：**
```cpp
EnemyShootAtPlayer shootComponent;
shootComponent.shootCooldown = 2000.0f;      // 每2秒射击一次
shootComponent.timeSinceLastShot = 0.0f;
shootComponent.detectionRange = 400.0f;      // 400单位内射击玩家
shootComponent.projectileSpeed = 200.0f;     // 子弹速度200单位/秒
shootComponent.isActive = true;
registry.AddComponent(enemyId, shootComponent);
```

### 3. Bullet (子弹组件)

表示飞行中的子弹。

**成员变量：**
```cpp
struct Bullet {
    Vec3 direction;           // 方向向量（归一化）
    float speed;              // 移动速度（单位/秒）
    float lifetime;           // 生命时间（毫秒）
    float damage;             // 伤害值
    bool isPlayerBullet;      // 是否为玩家子弹
};
```

**注意：** 子弹通常由 `EnemyShootAtPlayer` 系统自动创建，无需手动创建。

## 创建带AI的敌人 (Creating Enemies with AI)

### 方法1：使用辅助函数

```cpp
// 创建一个会移动和射击的敌人
GenerateSystem::CreateTestEnemyWithAI(
    registry,
    100.0f,      // x 坐标
    0.0f,        // y 坐标
    200.0f,      // z 坐标
    true,        // 启用移动
    true         // 启用射击
);
```

### 方法2：手动创建

```cpp
// 1. 创建实体
EntityID enemy = registry.CreateEntity();

// 2. 添加Transform3D组件（位置和外观）
Transform3D transform;
transform.pos = Vec3(100.0f, 0.0f, 200.0f);
transform.width = 30.0f;
transform.height = 30.0f;
transform.depth = 30.0f;
transform.r = 1.0f;
transform.g = 0.5f;
transform.b = 0.0f;  // 橙色
registry.AddComponent(enemy, transform);

// 3. 添加EnemyTag标签
registry.AddComponent(enemy, EnemyTag{});

// 4. 添加移动AI组件
EnemyMoveToPlayer moveComponent;
moveComponent.speed = 50.0f;
moveComponent.detectionRange = 500.0f;
moveComponent.isActive = true;
registry.AddComponent(enemy, moveComponent);

// 5. 添加射击AI组件
EnemyShootAtPlayer shootComponent;
shootComponent.shootCooldown = 2000.0f;
shootComponent.timeSinceLastShot = 0.0f;
shootComponent.detectionRange = 400.0f;
shootComponent.projectileSpeed = 200.0f;
shootComponent.isActive = true;
registry.AddComponent(enemy, shootComponent);
```

## 系统集成 (System Integration)

AI系统已经集成到主游戏循环中：

```cpp
// 在 ECSSystem.cpp 的 Update() 函数中：
void EngineSystem::Update(const float deltaTimeMs) {
    // ...
    
    // 更新敌人AI（移动、射击、子弹）
    EnemyAISystem::Update(*registry, deltaTimeMs);
    
    // ...
}
```

## 系统工作流程 (System Workflow)

### 每帧更新顺序：

1. **UpdateEnemyMovement()** - 更新敌人移动
   - 查找玩家位置
   - 遍历所有带 `EnemyMoveToPlayer` 组件的敌人
   - 检查是否在检测范围内
   - 计算朝玩家的方向
   - 移动敌人

2. **UpdateEnemyShooting()** - 更新敌人射击
   - 查找玩家位置
   - 遍历所有带 `EnemyShootAtPlayer` 组件的敌人
   - 更新射击冷却计时器
   - 检查是否在射击范围内且冷却完成
   - 创建子弹实体并发射

3. **UpdateBullets()** - 更新子弹运动
   - 遍历所有子弹实体
   - 更新生命时间
   - 移动子弹
   - 删除过期或超出范围的子弹

## 渲染 (Rendering)

子弹会自动渲染为小型立方体：

```cpp
// 在 RenderSystem.cpp 中：
void RenderBullets3D(EntityManager& registry, Camera3D& camera) {
    View<Transform3D, Bullet> bulletView(registry);
    for (EntityID id : bulletView) {
        auto& t = bulletView.get<Transform3D>(id);
        RenderCube(t, camera);
    }
}
```

- 敌人子弹：红色 (1.0, 0.0, 0.0)
- 玩家子弹：可自定义颜色

## 使用示例 (Usage Examples)

### 示例1：创建一个追击型敌人

```cpp
// 只会追击玩家，不会射击
GenerateSystem::CreateTestEnemyWithAI(registry, 0.0f, 0.0f, 300.0f, true, false);
```

### 示例2：创建一个炮台型敌人

```cpp
// 只会射击，不会移动
GenerateSystem::CreateTestEnemyWithAI(registry, 100.0f, 50.0f, 200.0f, false, true);
```

### 示例3：创建一个全能型敌人

```cpp
// 既会追击又会射击
GenerateSystem::CreateTestEnemyWithAI(registry, -50.0f, 0.0f, 250.0f, true, true);
```

### 示例4：动态启用/禁用AI

```cpp
// 获取敌人的移动组件并暂时禁用
View<EnemyTag, EnemyMoveToPlayer> enemyView(registry);
for (EntityID id : enemyView) {
    auto& moveComponent = enemyView.get<EnemyMoveToPlayer>(id);
    moveComponent.isActive = false;  // 暂停移动
}
```

## 调试提示 (Debugging Tips)

1. **检查敌人是否在范围内：**
   - 移动范围由 `detectionRange` 控制
   - 射击范围由 `detectionRange` 控制
   - 确保敌人和玩家的距离小于这些值

2. **检查射击冷却：**
   - `shootCooldown` 以毫秒为单位
   - 2000.0f = 2秒

3. **检查组件是否激活：**
   - `isActive` 必须为 `true`

4. **查看子弹：**
   - 子弹是红色小立方体
   - 生命周期默认为5秒
   - 超出5000单位会自动删除

## 性能优化建议 (Performance Optimization)

1. **限制敌人数量：** 每个敌人每帧都会计算距离，大量敌人会影响性能
2. **调整检测范围：** 较小的检测范围可以减少不必要的计算
3. **增加射击冷却：** 减少子弹生成频率可以提高性能
4. **子弹池化：** 可以实现对象池来重用子弹实体（高级优化）

## 未来扩展 (Future Extensions)

可以基于这个系统进一步扩展：

1. **碰撞检测：** 添加子弹与玩家/敌人的碰撞
2. **不同子弹类型：** 速度快的、伤害高的、追踪型的等
3. **敌人AI变体：** 巡逻、躲避、团队协作等
4. **粒子效果：** 子弹轨迹、爆炸效果等
5. **音效：** 射击声、命中声等

## 文件位置 (File Locations)

- **组件定义：** `src/System/Component/Component.h`
- **系统头文件：** `src/Game/Core/EnemyAISystem.h`
- **系统实现：** `src/Game/Core/EnemyAISystem.cpp`
- **渲染支持：** `src/Game/Core/RenderSystem.cpp`
- **辅助函数：** `src/Game/Core/GenerateSystem.cpp`
